import pandas as pd

# 加载 CSV 文件
file_path = '测试集划分航段_完善缺失航段.csv'  # 请替换为实际文件路径
df = pd.read_csv(file_path)

# 定义修改 segment_number 格式的函数
def modify_segment_format(x):
    # 假设原始格式为 "船号-航段号"，如 H1-1
    parts = x.split('-')  # 分割船号和航段号
    if len(parts) == 2:  # 确保格式正确
        ship_number = parts[0][1:]  # 获取船号，去掉前面的'H'
        segment_number = parts[1]  # 获取航段号
        return f"Ship{ship_number}-Segment{segment_number}"  # 按要求格式返回
    else:
        return x  # 如果格式不符合，保持原样

# 修改 segment_number 列的格式
df['segment_number'] = df['segment_number'].apply(modify_segment_format)

# 可选：打印更新后的前几行数据查看结果
print(df.head())

# 保存修改后的 DataFrame 为新的 CSV 文件
updated_file_path = '测试集划分航段.csv'  # 请替换为您希望保存的文件路径
df.to_csv(updated_file_path, index=False)

print(f"更新后的数据集已保存至: {updated_file_path}")
