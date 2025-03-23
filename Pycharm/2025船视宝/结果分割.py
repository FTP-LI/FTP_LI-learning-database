import pandas as pd

# 加载CSV文件
file1_path = '测试集划分航段_原数据.csv'
file2_path = '测试集划分航段_预测结果.csv'

file1 = pd.read_csv(file1_path)
file2 = pd.read_csv(file2_path)

# 将两个文件中的 slice_time 列转换为统一格式
file1['slice_time'] = pd.to_datetime(file1['slice_time'])
file2['slice_time'] = pd.to_datetime(file2['slice_time'], errors='coerce')

# 找到两个文件中相同的行
common_rows = pd.merge(file1, file2, on=file1.columns.tolist(), how='inner')

# 删除两个文件中相同的行
file1_remaining = file1.loc[~file1.index.isin(common_rows.index)]
file2_remaining = file2.loc[~file2.index.isin(common_rows.index)]

# 合并剩余的部分到一个新的DataFrame
remaining_data = pd.concat([file1_remaining, file2_remaining], ignore_index=True)

# 保存剩余的部分到新的CSV文件
remaining_file_path = 'remaining_data.csv'
remaining_data.to_csv(remaining_file_path, index=False)

# 输出新文件路径
print("剩余部分已保存到:", remaining_file_path)
