import numpy as np
import matplotlib.pyplot as plt

# 数据
categories = ['Category A', 'Category B', 'Category C', 'Category D']
values = [10, 20, 15, 30]

# 创建柱状图
plt.bar(categories, values, color=['red', 'green', 'blue', 'yellow'])

# 添加标题和标签
plt.title('柱状分析图示例')
plt.xlabel('分类')
plt.ylabel('数值')

# 显示图形
plt.show()