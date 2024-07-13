% 参数设置和函数定义
x = linspace(0, 10, 100);  % x轴范围和点数

% 函数1：sin函数
y1 = sin(x);

% 函数2：cos函数
y2 = cos(x);

% 函数3：正弦函数的平方
y3 = sin(x).^2;

% 函数4：指数函数
y4 = exp(-x);

% 绘图
figure;

% 函数1：sin函数，红色实线
plot(x, y1, 'r-', 'LineWidth', 1.5);
hold on;  % 保持当前图形，使得后续的图像都在同一幅图中绘制

% 函数2：cos函数，蓝色虚线
plot(x, y2, 'b--', 'LineWidth', 1.5);

% 函数3：sin^2函数，绿色点划线
plot(x, y3, 'g-.', 'LineWidth', 1.5);

% 函数4：指数函数，黑色点线
plot(x, y4, 'k:', 'LineWidth', 1.5);
xlabel('x');
ylabel('y');
legend('sin(x)', 'cos(x)', 'sin^2(x)', 'exp(-x)', 'Location', 'best');
grid on;

% 可选：调整y轴范围，使得所有函数的波形更为清晰
ylim([-1.2, 1.2]);

% 结束绘图
hold off;


