% 定义第一个函数
function1 = @(x) -0.04208*x.^2 + 11.9553*x + 1045.5834;

% 定义第二个函数
function2 = @(x) -0.048877*x.^2 + 12.1796*x + 1061.5325;

% 定义拟合范围
x = 50:50;

% 计算两个函数的拟合值
fit1 = function1(x);
fit2 = function2(x);

% 计算平均值
average = (fit1 + fit2) / 2;

% 将输出格式设置为长数字格式
format longG

average = round(average);
%进行取整

% 输出平均值
disp(average);