import numpy as np
from scipy.optimize import curve_fit

# X 轴数据
x_data = np.array([22, 20, 18, 16, 14, 12, 10])
# Y 轴数据
y_data = np.array([-75, -100, -140, -175, -225, -280, -380])


# 定义拟合函数形式，这里假设我们想要拟合一个指数函数
def exponential_func(x, a, b):
    return a * np.exp(-b * x)


# 使用 curve_fit 函数拟合数据
popt, pcov = curve_fit(exponential_func, x_data, y_data)

# 输出拟合参数  
print("拟合参数: a=%5.3f, b=%5.3f" % (popt[0], popt[1]))