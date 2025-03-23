% 参数定义
e = 5000;    % 需求量的常数项
f = 0.8;     % 房价对需求量的影响系数
g = 0.02;    % 家庭收入对需求量的影响系数
h = 1000;    % 人口增长率对需求量的影响系数
I = 50000;   % 家庭收入（美元）
R = 0.02;    % 人口增长率（2%）
J = 1000;    % 供给量的常数项
k = 0.6;     % 房价对供给量的影响系数
l = 0.4;     % 建筑成本对供给量的影响系数
T = 2000;    % 建筑成本
beta = 0.1;  % 需求量对自身变化的响应系数
gamma = 0.05; % 供给量对自身变化的响应系数
sigma = 0.3;  % 房价对供需差异的响应系数

% 初始条件
Qd0 = 5000; Qs0 = 3000; P0 = 400000;  % 初始需求量、供给量、房价

% 定义微分方程
odeFunc = @(t, y) [
    beta * (e - f * y(3) + g * I + h * R - y(1));
    gamma * (J + k * y(3) - l * T - y(2));
    sigma * (y(1) - y(2));
];

% 时间范围和求解
tSpan = [0 100];
y0 = [Qd0, Qs0, P0];
[t, y] = ode45(odeFunc, tSpan, y0);

% 提取结果
Qd = y(:, 1); Qs = y(:, 2); P = y(:, 3);
W1 = 0.5; W2 = 0.25;
Pr = W1 * P + W2 * Qd + W2 * Qs;

% 创建图形窗口并设置样式
figure('Position', [100, 100, 1200, 900], 'Color', 'white');

% 设置全局绘图参数
fontName = 'Times New Roman';
titleFontSize = 20;
labelFontSize = 14;
lineWidth = 2;
colorScheme = [0.2 0.4 0.8];  % 专业的深蓝色

% 设置子图布局
spacing = 0.1;
margin = 0.1;
width = (1 - 2*margin - spacing)/2;
height = (1 - 2*margin - spacing)/2;

% 第一个子图：需求量
subplot('Position', [margin margin width height])
plot(t, Qd, 'Color', colorScheme, 'LineWidth', lineWidth);
title('\bf{Housing Demand Q_d(t)}', 'FontName', fontName, 'FontSize', titleFontSize);
xlabel('\it{Time t}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Demand}', 'FontName', fontName, 'FontSize', labelFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out', 'TickLength', [0.02 0.02]);
grid on; grid minor;

% 第二个子图：供给量
subplot('Position', [margin+width+spacing margin width height])
plot(t, Qs, 'Color', colorScheme, 'LineWidth', lineWidth);
title('\bf{Housing Supply Q_s(t)}', 'FontName', fontName, 'FontSize', titleFontSize);
xlabel('\it{Time t}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Supply}', 'FontName', fontName, 'FontSize', labelFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out', 'TickLength', [0.02 0.02]);
grid on; grid minor;

% 第三个子图：房价
subplot('Position', [margin margin+height+spacing width height])
plot(t, P, 'Color', colorScheme, 'LineWidth', lineWidth);
title('\bf{Housing Price P(t)}', 'FontName', fontName, 'FontSize', titleFontSize);
xlabel('\it{Time t}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Price}', 'FontName', fontName, 'FontSize', labelFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out', 'TickLength', [0.02 0.02]);
grid on; grid minor;

% 第四个子图：居民压力指数
subplot('Position', [margin+width+spacing margin+height+spacing width height])
plot(t, Pr, 'Color', colorScheme, 'LineWidth', lineWidth);
title('\bf{Resident Pressure Index Pr(t)}', 'FontName', fontName, 'FontSize', titleFontSize);
xlabel('\it{Time t}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Pressure Index}', 'FontName', fontName, 'FontSize', labelFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out', 'TickLength', [0.02 0.02]);
grid on; grid minor;

% 添加整体标题
sgtitle('\bf{Dynamic Analysis of Housing Market System}', ...
    'FontName', fontName, ...
    'FontSize', titleFontSize + 4);

% 导出高分辨率图片
set(gcf, 'PaperPositionMode', 'auto');
print('-dpng', 'housing_market_analysis.png', '-r300');
