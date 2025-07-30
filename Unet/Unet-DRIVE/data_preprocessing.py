import os
import numpy as np
import cv2
from PIL import Image
import torch
from torch.utils.data import Dataset, DataLoader
import torchvision.transforms as transforms
import random
from sklearn.model_selection import train_test_split
import pickle

class DRIVEDataset(Dataset):
    """DRIVE数据集类"""
    def __init__(self, images, masks, labels, transform=None):
        self.images = images
        self.masks = masks
        self.labels = labels
        self.transform = transform
    
    def __len__(self):
        return len(self.images)
    
    def __getitem__(self, idx):
        image = self.images[idx]
        mask = self.masks[idx]
        label = self.labels[idx]
        
        if self.transform:
            image = self.transform(image)
            label = self.transform(label)
        
        return image, label, mask

def clahe_equalized(imgs):
    """CLAHE对比度限制自适应直方图均衡化"""
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
    imgs_equalized = np.empty(imgs.shape)
    for i in range(imgs.shape[0]):
        imgs_equalized[i,:,:,0] = clahe.apply(np.array(imgs[i,:,:,0], dtype=np.uint8))
    return imgs_equalized

def adjust_gamma(imgs, gamma=1.2):
    """Gamma校正"""
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255 for i in np.arange(0, 256)]).astype("uint8")
    new_imgs = np.empty(imgs.shape)
    for i in range(imgs.shape[0]):
        new_imgs[i,:,:,0] = cv2.LUT(np.array(imgs[i,:,:,0], dtype=np.uint8), table)
    return new_imgs

def rgb2gray(rgb):
    """RGB转灰度图"""
    return np.dot(rgb[...,:3], [0.299, 0.587, 0.114])

def dataset_normalized(imgs):
    """数据标准化"""
    imgs_normalized = np.empty(imgs.shape)
    imgs_std = np.std(imgs)
    imgs_mean = np.mean(imgs)
    imgs_normalized = (imgs - imgs_mean) / imgs_std
    for i in range(imgs.shape[0]):
        imgs_normalized[i] = ((imgs_normalized[i] - np.min(imgs_normalized[i])) / 
                             (np.max(imgs_normalized[i]) - np.min(imgs_normalized[i]))) * 255
    return imgs_normalized

def preprocess_images(images):
    """图像预处理流程"""
    # 转换为灰度图
    if len(images.shape) == 4 and images.shape[3] == 3:
        images_gray = np.empty((images.shape[0], images.shape[1], images.shape[2], 1))
        for i in range(images.shape[0]):
            images_gray[i,:,:,0] = rgb2gray(images[i])
        images = images_gray
    
    # 标准化
    images = dataset_normalized(images)
    
    # CLAHE均衡化
    images = clahe_equalized(images)
    
    # Gamma校正
    images = adjust_gamma(images, 1.2)
    
    return images

def extract_random_patches(images, labels, masks, patch_size=48, num_patches_per_image=9500):
    """随机提取图像块"""
    all_patches = []
    all_labels = []
    all_masks = []
    
    for i in range(images.shape[0]):
        img = images[i,:,:,0]
        label = labels[i,:,:,0]
        mask = masks[i,:,:,0]
        
        h, w = img.shape
        
        for _ in range(num_patches_per_image):
            # 随机选择中心点
            center_x = random.randint(patch_size//2, w - patch_size//2)
            center_y = random.randint(patch_size//2, h - patch_size//2)
            
            # 提取patch
            x1 = center_x - patch_size//2
            x2 = center_x + patch_size//2
            y1 = center_y - patch_size//2
            y2 = center_y + patch_size//2
            
            patch_img = img[y1:y2, x1:x2]
            patch_label = label[y1:y2, x1:x2]
            patch_mask = mask[y1:y2, x1:x2]
            
            if patch_img.shape == (patch_size, patch_size):
                all_patches.append(patch_img)
                all_labels.append(patch_label)
                all_masks.append(patch_mask)
    
    return np.array(all_patches), np.array(all_labels), np.array(all_masks)

def load_drive_data(drive_path):
    """加载DRIVE数据集"""
    # 训练数据路径
    train_images_path = os.path.join(drive_path, 'training', 'images')
    train_labels_path = os.path.join(drive_path, 'training', '1st_manual')
    train_masks_path = os.path.join(drive_path, 'training', 'mask')
    
    # 测试数据路径
    test_images_path = os.path.join(drive_path, 'test', 'images')
    test_masks_path = os.path.join(drive_path, 'test', 'mask')
    
    # 加载训练图像
    train_images = []
    train_labels = []
    train_masks = []
    
    for i in range(21, 41):  # 训练集编号21-40
        # 加载原图
        img_path = os.path.join(train_images_path, f'{i:02d}_training.tif')
        if os.path.exists(img_path):
            img = np.array(Image.open(img_path))
            train_images.append(img)
        
        # 加载标签
        label_path = os.path.join(train_labels_path, f'{i:02d}_manual1.gif')
        if os.path.exists(label_path):
            label = np.array(Image.open(label_path))
            train_labels.append(label)
        
        # 加载掩码
        mask_path = os.path.join(train_masks_path, f'{i:02d}_training_mask.gif')
        if os.path.exists(mask_path):
            mask = np.array(Image.open(mask_path))
            train_masks.append(mask)
    
    # 加载测试图像
    test_images = []
    test_masks = []
    
    for i in range(1, 21):  # 测试集编号01-20
        # 加载原图
        img_path = os.path.join(test_images_path, f'{i:02d}_test.tif')
        if os.path.exists(img_path):
            img = np.array(Image.open(img_path))
            test_images.append(img)
        
        # 加载掩码
        mask_path = os.path.join(test_masks_path, f'{i:02d}_test_mask.gif')
        if os.path.exists(mask_path):
            mask = np.array(Image.open(mask_path))
            test_masks.append(mask)
    
    return (np.array(train_images), np.array(train_labels), np.array(train_masks),
            np.array(test_images), np.array(test_masks))

def prepare_drive_dataset(drive_path, output_path, patch_size=48, num_patches_per_image=9500):
    """准备DRIVE数据集"""
    print("正在加载DRIVE数据集...")
    train_images, train_labels, train_masks, test_images, test_masks = load_drive_data(drive_path)
    
    print(f"训练集: {len(train_images)}张图像")
    print(f"测试集: {len(test_images)}张图像")
    
    # 预处理训练图像
    print("正在预处理训练图像...")
    train_images_processed = preprocess_images(train_images)
    
    # 预处理标签（二值化）
    train_labels_processed = np.expand_dims(train_labels, axis=3)
    train_labels_processed = (train_labels_processed > 0).astype(np.float32)
    
    # 处理掩码
    train_masks_processed = np.expand_dims(train_masks, axis=3)
    train_masks_processed = (train_masks_processed > 0).astype(np.float32)
    
    # 提取训练patches
    print("正在提取训练patches...")
    train_patches, train_patch_labels, train_patch_masks = extract_random_patches(
        train_images_processed, train_labels_processed, train_masks_processed,
        patch_size, num_patches_per_image
    )
    
    print(f"提取的训练patches数量: {len(train_patches)}")
    
    # 划分训练集和验证集 (90% 训练, 10% 验证)
    train_patches, val_patches, train_patch_labels, val_patch_labels = train_test_split(
        train_patches, train_patch_labels, test_size=0.1, random_state=42
    )
    
    # 预处理测试图像
    print("正在预处理测试图像...")
    test_images_processed = preprocess_images(test_images)
    test_masks_processed = np.expand_dims(test_masks, axis=3)
    test_masks_processed = (test_masks_processed > 0).astype(np.float32)
    
    # 保存处理好的数据
    os.makedirs(output_path, exist_ok=True)
    
    print("正在保存处理好的数据...")
    
    # 保存训练数据
    np.save(os.path.join(output_path, 'train_patches.npy'), train_patches)
    np.save(os.path.join(output_path, 'train_labels.npy'), train_patch_labels)
    np.save(os.path.join(output_path, 'val_patches.npy'), val_patches)
    np.save(os.path.join(output_path, 'val_labels.npy'), val_patch_labels)
    
    # 保存测试数据
    np.save(os.path.join(output_path, 'test_images.npy'), test_images_processed)
    np.save(os.path.join(output_path, 'test_masks.npy'), test_masks_processed)
    
    # 保存原始测试图像（用于可视化）
    np.save(os.path.join(output_path, 'test_images_original.npy'), test_images)
    
    print(f"数据预处理完成！")
    print(f"训练patches: {len(train_patches)}")
    print(f"验证patches: {len(val_patches)}")
    print(f"测试图像: {len(test_images_processed)}")
    
    return {
        'train_patches': len(train_patches),
        'val_patches': len(val_patches),
        'test_images': len(test_images_processed)
    }

import numpy as np
import os
import cv2
from PIL import Image
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import seaborn as sns
from tqdm import tqdm

def read_picture(path):
    """读取图片函数"""
    try:
        # 尝试用PIL读取
        img = Image.open(path)
        return np.array(img)
    except:
        # 如果PIL失败，用OpenCV读取
        img = cv2.imread(path)
        if img is not None:
            return cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        else:
            raise ValueError(f"无法读取图片: {path}")

def preprocess_single_image(image):
    """单张图像的预处理流程：灰度转换 → 标准化 → CLAHE → Gamma校正"""
    # 1. 转换为灰度图
    if len(image.shape) == 3:
        gray_img = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    else:
        gray_img = image
    
    # 2. 标准化 (0-255范围)
    normalized = cv2.normalize(gray_img, None, 0, 255, cv2.NORM_MINMAX).astype(np.uint8)
    
    # 3. CLAHE均衡化
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
    clahe_img = clahe.apply(normalized)
    
    # 4. Gamma校正
    gamma = 1.2
    inv_gamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** inv_gamma) * 255 for i in np.arange(0, 256)]).astype("uint8")
    gamma_img = cv2.LUT(clahe_img, table)
    
    return gamma_img

def process_drive_dataset():
    """处理DRIVE数据集的主函数 - 包含水平翻转数据增强"""
    
    output_dir = r"c:\Users\ATLAS\Desktop\天工论文\Unet-DRIVE\处理好的数据集"
    os.makedirs(output_dir, exist_ok=True)
    
    # 修改：将可视化结果也保存到处理好的数据集目录下
    vis_dir = output_dir  # 直接使用output_dir作为可视化目录
    
    print("=" * 60)
    print("开始处理DRIVE数据集")
    print("处理流程：灰度转换 → 标准化 → CLAHE均衡化 → Gamma校正 → 水平翻转增强")
    print("=" * 60)
    
    # 处理训练集和测试集
    for name in ['training', 'test']:
        print(f"\n正在处理 {name} 数据集...")
        
        if name == 'training':
            picture_path = r"c:\Users\ATLAS\Desktop\天工论文\Unet-DRIVE\DRIVE\training\images"
            label_path = r"c:\Users\ATLAS\Desktop\天工论文\Unet-DRIVE\DRIVE\training\1st_manual"
        else:
            picture_path = r"c:\Users\ATLAS\Desktop\天工论文\Unet-DRIVE\DRIVE\test\images"
            label_path = r"c:\Users\ATLAS\Desktop\天工论文\Unet-DRIVE\DRIVE\test\1st_manual"
        
        # 检查路径是否存在
        if not os.path.exists(picture_path):
            print(f"错误: 路径不存在 {picture_path}")
            continue
            
        # 获取文件列表 - 这是缺失的关键代码！
        picturenames = sorted([f for f in os.listdir(picture_path) if f.endswith(('.tif', '.jpg', '.png'))])
        
        if label_path and os.path.exists(label_path):
            labelnames = sorted([f for f in os.listdir(label_path) if f.endswith(('.gif', '.tif', '.jpg', '.png'))])
        else:
            labelnames = []
        
        print(f"找到 {len(picturenames)} 张图像")
        if labelnames:
            print(f"找到 {len(labelnames)} 张标签")
        
        data = []
        label = []
        
        # 处理每张图像
        for i, pic_name in enumerate(tqdm(picturenames, desc=f"处理{name}图像")):
            try:
                # 读取原始图像
                dp = os.path.join(picture_path, pic_name)
                original_img = read_picture(dp)
                
                # 调整尺寸到512x512
                resized_img = cv2.resize(original_img, (512, 512))
                
                # 应用预处理流程
                processed_img = preprocess_single_image(resized_img)
                
                # 转换为CHW格式并归一化到[0,1]
                p = processed_img.reshape(1, 512, 512).astype(np.float32) / 255.0
                data.append(p)
                
                # 水平翻转增强 - 增加一倍数据
                flipped_img = cv2.flip(processed_img, 1)  # 水平翻转
                p_flipped = flipped_img.reshape(1, 512, 512).astype(np.float32) / 255.0
                data.append(p_flipped)
                
                # 处理标签
                if i < len(labelnames) and label_path and os.path.exists(label_path):
                    lp = os.path.join(label_path, labelnames[i])
                    label_img = read_picture(lp)
                    
                    # 调整标签尺寸
                    label_resized = cv2.resize(label_img, (512, 512))
                    
                    if len(label_resized.shape) == 3:
                        label_resized = cv2.cvtColor(label_resized, cv2.COLOR_RGB2GRAY)
                    
                    l = label_resized.reshape(1, 512, 512)
                    l = (l > 0).astype(np.float32)
                    label.append(l)
                    
                    # 标签也进行水平翻转
                    l_flipped = cv2.flip(label_resized, 1).reshape(1, 512, 512)
                    l_flipped = (l_flipped > 0).astype(np.float32)
                    label.append(l_flipped)
                
                # 只可视化第一张图像的处理步骤
                if i == 0:  # 只显示第一张图片的处理流程
                    vis_path = os.path.join(vis_dir, f"{name}_{pic_name}_preprocessing.png")
                    visualize_new_preprocessing_steps(original_img, resized_img, processed_img, 
                                                     f"{name}_{pic_name}", vis_path)
                
            except Exception as e:
                print(f"处理图像 {pic_name} 时出错: {e}")
                continue
        
        # 转换为numpy数组
        data = np.array(data)
        print(f"图像数据形状: {data.shape}")
        
        if len(label) > 0:  # 修复：使用len()而不是直接判断数组
            label = np.array(label)
            print(f"标签数据形状: {label.shape}")
            # 创建配对数据集
            dataset = np.array([(d, l) for d, l in zip(data, label)], dtype=object)
        else:
            # 只有图像，没有标签
            dataset = data
        
        # 保存数据集到正确路径
        save_path = os.path.join(output_dir, f"{name}dataset.npy")
        np.save(save_path, dataset, allow_pickle=True)
        print(f"数据集已保存到: {save_path}")
        
        # 打印统计信息
        print(f"\n{name}数据集统计:")
        print("-" * 30)
        print(f"原始图像数量: {len(picturenames)}")
        print(f"增强后数据数量: {len(data)} (包含水平翻转)")
        print(f"数据增强倍数: 2x")
        print(f"图像形状: {data[0].shape}")
        print(f"图像数值范围: [{data.min():.3f}, {data.max():.3f}]")
        if len(label) > 0:  # 修复：使用len()而不是直接判断数组
            print(f"标签数量: {len(label)}")
            print(f"标签形状: {label[0].shape}")
            print(f"血管像素比例: {label.mean():.4f}")
    
    # 生成数据集概览
    generate_dataset_overview(output_dir)
    
    print("\n" + "=" * 60)
    print("数据预处理完成！")
    print("=" * 60)

def visualize_new_preprocessing_steps(original_img, resized_img, processed_img, img_name, save_path=None):
    """可视化新的预处理步骤 - 2x3布局"""
    fig, axes = plt.subplots(2, 3, figsize=(15, 10))
    fig.suptitle(f'图像预处理流程 - {img_name}', fontsize=16, fontweight='bold')
    
    # 第一行：处理步骤
    # 1. 原始图像
    axes[0, 0].imshow(original_img)
    axes[0, 0].set_title('1. 原始图像', fontsize=12)
    axes[0, 0].axis('off')
    
    # 2. 调整尺寸
    axes[0, 1].imshow(resized_img)
    axes[0, 1].set_title('2. 尺寸调整 (512×512)', fontsize=12)
    axes[0, 1].axis('off')
    
    # 3. 转换为灰度图
    gray_img = cv2.cvtColor(resized_img, cv2.COLOR_RGB2GRAY) if len(resized_img.shape) == 3 else resized_img
    axes[0, 2].imshow(gray_img, cmap='gray')
    axes[0, 2].set_title('3. 灰度转换', fontsize=12)
    axes[0, 2].axis('off')
    
    # 第二行：高级处理
    # 4. 标准化
    normalized = cv2.normalize(gray_img, None, 0, 255, cv2.NORM_MINMAX)
    axes[1, 0].imshow(normalized, cmap='gray')
    axes[1, 0].set_title('4. 标准化', fontsize=12)
    axes[1, 0].axis('off')
    
    # 5. CLAHE均衡化
    clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
    clahe_img = clahe.apply(normalized.astype(np.uint8))
    axes[1, 1].imshow(clahe_img, cmap='gray')
    axes[1, 1].set_title('5. CLAHE均衡化', fontsize=12)
    axes[1, 1].axis('off')
    
    # 6. 最终结果 (Gamma校正)
    axes[1, 2].imshow(processed_img, cmap='gray')
    axes[1, 2].set_title('6. Gamma校正 (最终结果)', fontsize=12)
    axes[1, 2].axis('off')
    
    plt.tight_layout()
    
    if save_path:
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"预处理流程图已保存: {save_path}")
    
    plt.show()

def generate_dataset_overview(output_dir):
    """生成数据集概览图"""
    try:
        # 加载数据集
        train_data = np.load(os.path.join(output_dir, "trainingdataset.npy"), allow_pickle=True)
        test_data = np.load(os.path.join(output_dir, "testdataset.npy"), allow_pickle=True)
        
        fig, axes = plt.subplots(2, 3, figsize=(15, 10))
        fig.suptitle('DRIVE数据集概览', fontsize=16, fontweight='bold')
        
        # 训练集样本展示
        if len(train_data) > 0 and isinstance(train_data[0], (list, tuple, np.ndarray)) and len(train_data[0]) == 2:
            # 有标签的情况
            sample_img = train_data[0][0].transpose(1, 2, 0) if train_data[0][0].shape[0] <= 3 else train_data[0][0][0]
            sample_label = train_data[0][1][0]
            
            axes[0, 0].imshow(sample_img, cmap='gray' if len(sample_img.shape) == 2 else None)
            axes[0, 0].set_title('训练集样本图像')
            axes[0, 0].axis('off')
            
            axes[0, 1].imshow(sample_label, cmap='gray')
            axes[0, 1].set_title('训练集样本标签')
            axes[0, 1].axis('off')
            
            # 叠加显示
            overlay = sample_img.copy() if len(sample_img.shape) == 3 else np.stack([sample_img]*3, axis=-1)
            overlay[:,:,0] = np.where(sample_label > 0.5, 1, overlay[:,:,0])
            axes[0, 2].imshow(overlay)
            axes[0, 2].set_title('图像+标签叠加')
            axes[0, 2].axis('off')
        
        # 测试集样本展示
        if len(test_data) > 0:
            if isinstance(test_data[0], (list, tuple)) and len(test_data[0]) == 2:
                test_img = test_data[0][0].transpose(1, 2, 0) if test_data[0][0].shape[0] <= 3 else test_data[0][0][0]
            else:
                test_img = test_data[0].transpose(1, 2, 0) if test_data[0].shape[0] <= 3 else test_data[0][0]
            
            axes[1, 0].imshow(test_img, cmap='gray' if len(test_img.shape) == 2 else None)
            axes[1, 0].set_title('测试集样本图像')
            axes[1, 0].axis('off')
        
        # 数据集统计
        stats_text = f"""
数据集统计:

训练集:
• 样本数量: {len(train_data)}
• 图像尺寸: 512×512
• 数据类型: float32
• 数值范围: [0.0, 1.0]

测试集:
• 样本数量: {len(test_data)}
• 图像尺寸: 512×512
• 数据类型: float32
• 数值范围: [0.0, 1.0]
        """
        
        axes[1, 1].axis('off')
        axes[1, 1].text(0.05, 0.95, stats_text, transform=axes[1, 1].transAxes, 
                        fontsize=11, verticalalignment='top', 
                        bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        axes[1, 1].set_title('数据集统计信息')
        
        # 处理流程图
        axes[1, 2].axis('off')
        flow_text = """
预处理流程:

1. 读取原始图像
   ↓
2. 调整尺寸至512×512
   ↓
3. 转换为CHW格式
   ↓
4. 归一化至[0,1]
   ↓
5. 保存为.npy格式
        """
        axes[1, 2].text(0.05, 0.95, flow_text, transform=axes[1, 2].transAxes, 
                        fontsize=11, verticalalignment='top', 
                        bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        axes[1, 2].set_title('预处理流程')
        
        plt.tight_layout()
        
        # 修改：将概览图也保存到处理好的数据集目录下
        overview_path = os.path.join(output_dir, "dataset_overview.png")
        plt.savefig(overview_path, dpi=300, bbox_inches='tight')
        plt.show()
        
        print(f"数据集概览图已保存: {overview_path}")
        
    except Exception as e:
        print(f"生成数据集概览时出错: {e}")

if __name__ == "__main__":
    # 设置matplotlib中文字体
    plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
    plt.rcParams['axes.unicode_minus'] = False
    
    # 运行数据预处理
    process_drive_dataset()