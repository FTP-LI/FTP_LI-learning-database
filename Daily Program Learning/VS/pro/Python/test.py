import pandas as pd

# 读取Excel文件
df = pd.read_excel('input_data.xlsx')

# 按照seller_no、product_no、warehouse_no进行分组
grouped = df.groupby(['seller_no', 'product_no', 'warehouse_no'])

# 遍历每个分组并将其输出到不同的Excel文件
for group_name, group_data in grouped:
    # 创建一个新的Excel文件名
    file_name = f"{group_name[0]}_{group_name[1]}_{group_name[2]}.xlsx"
    
    # 将分组数据写入新的Excel文件
    group_data.to_excel(file_name, index=False)