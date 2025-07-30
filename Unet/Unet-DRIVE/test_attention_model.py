import torch
import numpy as np
import cv2
import os
import matplotlib.pyplot as plt
import matplotlib
from sklearn.metrics import roc_auc_score, average_precision_score, jaccard_score, f1_score, accuracy_score, precision_score, recall_score
import pandas as pd
from tqdm import tqdm
from Unet_Attention import AttentionUNet

# 设置matplotlib中文字体
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False
# 调整字体大小
plt.rcParams['font.size'] = 14  # 从12调整到14
plt.rcParams['axes.titlesize'] = 16  # 从14调整到16
plt.rcParams['axes.labelsize'] = 14  # 添加轴标签字体大小
plt.rcParams['legend.fontsize'] = 14  # 添加图例字体大小

def calculate_metrics(y_true, y_pred, y_prob):
    """
    计算各种评价指标
    
    Args:
        y_true: 真实标签 (0或1)
        y_pred: 预测标签 (0或1)
        y_prob: 预测概率 (0-1之间)
    
    Returns:
        dict: 包含所有评价指标的字典
    """
    # 确保数据是一维的
    y_true = y_true.flatten()
    y_pred = y_pred.flatten()
    y_prob = y_prob.flatten()
    
    # 计算各种指标
    auc_roc = roc_auc_score(y_true, y_prob)
    auc_pr = average_precision_score(y_true, y_prob)
    jaccard = jaccard_score(y_true, y_pred)
    f1 = f1_score(y_true, y_pred)
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred)
    recall = recall_score(y_true, y_pred)  # 敏感性
    
    # 计算特异性 (Specificity)
    tn = np.sum((y_true == 0) & (y_pred == 0))
    fp = np.sum((y_true == 0) & (y_pred == 1))
    specificity = tn / (tn + fp) if (tn + fp) > 0 else 0
    
    return {
        'AUC-ROC': auc_roc,
        'AUC-PR': auc_pr,
        'Jaccard系数': jaccard,
        'F1分数': f1,
        '准确率': accuracy,
        '敏感性': recall,
        '特异性': specificity,
        '精确率': precision
    }

def post_process_prediction(pred, min_area=50):
    """
    使用OpenCV进行后处理，移除小的连通区域并进行形态学操作
    
    Args:
        pred: 预测结果 (numpy array)
        min_area: 最小连通区域面积
    
    Returns:
        处理后的预测结果
    """
    # 确保输入是uint8类型
    pred = pred.astype(np.uint8)
    
    # 查找连通组件
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(pred, connectivity=8)
    
    # 创建输出图像
    output = np.zeros_like(pred)
    
    # 保留面积大于阈值的连通组件（跳过背景，标签0）
    for i in range(1, num_labels):
        if stats[i, cv2.CC_STAT_AREA] >= min_area:
            output[labels == i] = 1
    
    # 形态学闭运算连接断裂的血管
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    output = cv2.morphologyEx(output, cv2.MORPH_CLOSE, kernel)
    
    return output

# 获取脚本所在目录的绝对路径
script_dir = os.path.dirname(os.path.abspath(__file__))

# 设置模型导入
from Unet import UNet

# 加载数据集
dataset = np.load(os.path.join(script_dir, '处理好的数据集', 'testdataset.npy'), allow_pickle=True)

print(f"数据集长度: {len(dataset)}")
print(f"第一个数据项类型: {type(dataset[0])}")

# 正确的模型加载方式
model = AttentionUNet(1, 1)  # 创建模型实例
# 修改前：
# model.load_state_dict(torch.load("训练好的模型/bestmodel.pt"))

# 修改后：使用绝对路径
import os
script_dir = os.path.dirname(os.path.abspath(__file__))
model_path = os.path.join(script_dir, "训练好的模型", "bestmodel.pt")
model.load_state_dict(torch.load(model_path))
model.eval()  # 设置为评估模式

def normalize_for_display(img):
    """将图像标准化到0-255范围以便显示"""
    if img is None:
        return None
    
    img = img.astype(np.float32)
    
    # 移除多余的维度
    while len(img.shape) > 2 and img.shape[0] == 1:
        img = img[0]
    
    if len(img.shape) > 2:
        img = img[0] if img.shape[0] == 1 else img[:, :, 0]
    
    # 标准化到0-255
    img_min, img_max = img.min(), img.max()
    if img_max > img_min:
        img = ((img - img_min) / (img_max - img_min) * 255).astype(np.uint8)
    else:
        img = np.zeros_like(img, dtype=np.uint8)
    
    return img

# 评估所有测试样本
print("\n开始评估所有测试样本...")
all_metrics = []
all_predictions = []
all_labels = []
all_probabilities = []

# 保存第一个样本用于可视化
first_sample_data = None
first_sample_label = None
first_sample_output = None
first_sample_output_processed = None

# 可以调整的参数
PREDICTION_THRESHOLD = 0.7  # 提高阈值以减少假阳性
USE_POST_PROCESSING = True  # 是否使用后处理

print(f"使用预测阈值: {PREDICTION_THRESHOLD}")
print(f"使用后处理: {USE_POST_PROCESSING}")

with torch.no_grad():
    for i in tqdm(range(len(dataset)), desc="评估进度"):
        # 获取数据项
        data_item = dataset[i]
        
        if isinstance(data_item, (list, tuple)) and len(data_item) >= 2:
            data = data_item[0]
            label = data_item[1]
        else:
            # 处理4维数据格式 (2, 1, 512, 512)
            if len(data_item.shape) == 4 and data_item.shape[0] == 2:
                data = data_item[0]  # 图像
                label = data_item[1]  # 标签
            else:
                continue
        
        # 确保数据类型正确
        if isinstance(data, np.ndarray):
            if data.dtype == object:
                data = np.array(data, dtype=np.float32)
            else:
                data = data.astype(np.float32)
        
        if isinstance(label, np.ndarray):
            if label.dtype == object:
                label = np.array(label, dtype=np.float32)
            else:
                label = label.astype(np.float32)
        
        # 准备输入张量
        if len(data.shape) == 3:  # (C, H, W)
            input_tensor = torch.from_numpy(data).unsqueeze(0)  # (1, C, H, W)
        elif len(data.shape) == 2:  # (H, W)
            input_tensor = torch.from_numpy(data).unsqueeze(0).unsqueeze(0)  # (1, 1, H, W)
        else:
            continue
        
        # 模型推理
        output = model(input_tensor)
        output_prob = torch.sigmoid(output).detach().numpy()
        
        # 使用调整后的阈值
        output_pred = (output_prob > PREDICTION_THRESHOLD).astype(np.uint8)
        
        # 后处理（可选）
        if USE_POST_PROCESSING:
            output_pred_processed = post_process_prediction(output_pred[0, 0])
        else:
            output_pred_processed = output_pred[0, 0]
        
        # 处理标签
        if len(label.shape) == 3:  # (C, H, W)
            label = label[0]  # 取第一个通道
        
        # 将标签转换为二值
        label_binary = (label > 0.5).astype(np.uint8)
        
        # 保存第一个样本用于可视化
        if i == 0:
            first_sample_data = data
            first_sample_label = label
            first_sample_output = output_prob[0, 0]
            first_sample_output_processed = output_pred_processed
        
        # 计算指标（使用后处理的结果）
        try:
            sample_metrics = calculate_metrics(
                label_binary, 
                output_pred_processed, 
                output_prob[0, 0]
            )
            all_metrics.append(sample_metrics)
            
            # 保存用于整体计算
            all_predictions.append(output_pred_processed.flatten())
            all_labels.append(label_binary.flatten())
            all_probabilities.append(output_prob[0, 0].flatten())
            
        except Exception as e:
            print(f"样本 {i} 评估出错: {e}")
            continue

# 计算统计结果
print("\n计算统计结果...")

# 转换为DataFrame便于计算统计量
metrics_df = pd.DataFrame(all_metrics)

# 计算平均值和标准差
mean_metrics = metrics_df.mean()
std_metrics = metrics_df.std()

# 计算整体像素级别的指标
all_labels_flat = np.concatenate(all_labels)
all_predictions_flat = np.concatenate(all_predictions)
all_probabilities_flat = np.concatenate(all_probabilities)

overall_metrics = calculate_metrics(
    all_labels_flat, 
    all_predictions_flat, 
    all_probabilities_flat
)

# 打印结果
print("\n" + "="*60)
print("模型性能评估结果")
print("="*60)

print("\n按样本平均的结果:")
print("| 评价指标 | 数值 | 标准差 |")
print("|---------|------|--------|")

for metric in mean_metrics.index:
    if metric in ['准确率', '敏感性', '特异性', '精确率']:
        # 百分比显示
        mean_val = mean_metrics[metric] * 100
        std_val = std_metrics[metric] * 100
        print(f"| {metric} | {mean_val:.2f}% | {std_val:.2f}% |")
    else:
        # 小数显示
        mean_val = mean_metrics[metric]
        std_val = std_metrics[metric]
        print(f"| {metric} | {mean_val:.4f} | {std_val:.4f} |")

print("\n整体像素级别的结果:")
print("| 评价指标 | 数值 |")
print("|---------|------|")

for metric, value in overall_metrics.items():
    if metric in ['准确率', '敏感性', '特异性', '精确率']:
        print(f"| {metric} | {value*100:.2f}% |")
    else:
        print(f"| {metric} | {value:.4f} |")

if first_sample_data is not None:
    print("\n显示第一个测试样本的结果...")
    
    # 准备显示图像
    if len(first_sample_data.shape) == 3:
        original_display = normalize_for_display(first_sample_data[0])
    else:
        original_display = normalize_for_display(first_sample_data)

    label_display = normalize_for_display(first_sample_label)
    output_display = normalize_for_display(first_sample_output)
    
    # 使用matplotlib显示（1x3布局）
    fig, axes = plt.subplots(1, 3, figsize=(18, 6))
    fig.suptitle('视网膜血管分割结果对比', fontsize=20, fontweight='bold', y=0.92)  # 调整主标题位置
    
    # 原图像
    axes[0].imshow(original_display, cmap='gray')
    axes[0].set_title('原始图像', fontsize=16, pad=10)  # 简化标题，减少pad
    axes[0].axis('off')
    
    # 真实标签
    axes[1].imshow(label_display, cmap='gray')
    axes[1].set_title('真实标签', fontsize=16, pad=10)  # 简化标题，减少pad
    axes[1].axis('off')
    
    # 原始预测结果
    axes[2].imshow(output_display, cmap='gray')
    axes[2].set_title('模型预测', fontsize=16, pad=10)  # 简化标题，减少pad
    axes[2].axis('off')
    
    # 调整子图间距
    plt.subplots_adjust(left=0.05, right=0.95, top=0.82, bottom=0.05, 
                       wspace=0.15)  # 调整top和bottom值
    
    # 移除预测阈值显示
    
    # 保存图片到指定路径而不是显示
    results_dir = os.path.join(script_dir, '模型结果')
    os.makedirs(results_dir, exist_ok=True)
    
    # 保存高质量的对比图
    comparison_path = os.path.join(results_dir, 'vessel_segmentation_comparison.png')
    plt.savefig(comparison_path, dpi=300, bbox_inches='tight', 
                facecolor='white', edgecolor='none', pad_inches=0.2)
    print(f"对比图已保存到: {comparison_path}")
    
    # 关闭图形以释放内存
    plt.close(fig)

# 保存结果
results_dir = os.path.join(script_dir, '模型结果')
os.makedirs(results_dir, exist_ok=True)

# 保存图像
# 保存单独的图片文件
if first_sample_data is not None:
    if original_display is not None:
        cv2.imwrite(os.path.join(results_dir, 'original.png'), original_display)
    if label_display is not None:
        cv2.imwrite(os.path.join(results_dir, 'ground_truth.png'), label_display)
    if output_display is not None:
        cv2.imwrite(os.path.join(results_dir, 'raw_prediction.png'), output_display)
    # 移除后处理图片的保存，因为已经不再使用后处理

# 保存详细指标
metrics_df.to_csv(os.path.join(results_dir, 'detailed_metrics.csv'), index=False)

# 保存汇总结果
summary_results = {
    'metric': list(mean_metrics.index),
    'mean': list(mean_metrics.values),
    'std': list(std_metrics.values),
    'overall': [overall_metrics[metric] for metric in mean_metrics.index]
}

summary_df = pd.DataFrame(summary_results)
summary_df.to_csv(os.path.join(results_dir, 'summary_metrics.csv'), index=False)

print(f"\n详细结果已保存到: {results_dir}")
print("- detailed_metrics.csv: 每个样本的详细指标")
print("- summary_metrics.csv: 汇总统计结果")
print("- original.png, ground_truth.png, raw_prediction.png, processed_prediction.png: 第一个样本的可视化结果")
print("\n评估完成！")
cv2.waitKey(0)  # 等待按键
cv2.destroyAllWindows()  # 关闭所有窗口

# 在现有代码中修改这些参数

# 降低预测阈值，让原始预测更接近后处理结果
PREDICTION_THRESHOLD = 0.3  # 从0.7降低到0.5

# 调整后处理参数
def post_process_prediction(pred, min_area=20, kernel_size=2):  # 减小参数
    """
    使用更温和的后处理参数
    """
    pred = pred.astype(np.uint8)
    
    # 使用更小的最小连通区域阈值
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(pred, connectivity=8)
    output = np.zeros_like(pred)
    
    for i in range(1, num_labels):
        if stats[i, cv2.CC_STAT_AREA] >= min_area:  # 降低阈值
            output[labels == i] = 1
    
    # 使用更小的形态学核
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (kernel_size, kernel_size))
    output = cv2.morphologyEx(output, cv2.MORPH_CLOSE, kernel)
    
    return output
