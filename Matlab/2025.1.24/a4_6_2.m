% 参数定义（根据朱诺市的实际情况）
epsilon = 0.0001;             % 调整epsilon来控制凹陷形状
w_max = 80000;               % 增加最大游客数，假设为80,000人
mu = 0.6;                    % 系数mu，表示管理每名游客的成本，假设为0.6
v = 1.5;                     % 系数v，表示每名游客的平均消费额的单位系数，假设为1.5
B_max = 30000000;            % 增加最大预算限制，假设为$30,000,000
z_value = 500;               % 平均消费额，假设为$500
k = 2;                       % 市场需求系数，假设为2

% 收入函数的调整，使得在某些特定的游客数量和消费额下，收入呈现凹陷
E = @(w, z, k) w .* z .* (1 - epsilon * (w - w_max/2).^2) .* k;  % 新增市场需求系数
F1 = @(w) w - w_max;                           % 游客数量约束
F2 = @(w, z) mu * w + v * z - B_max;          % 预算约束

% 灵敏度分析的参数范围
w_range = linspace(0, w_max, 100);    % 游客数量的范围
z_range = linspace(0, 1000, 100);     % 消费额的范围

% 计算不同游客数量和消费额下的收入
[W, Z] = meshgrid(w_range, z_range);  % 创建游客数量和消费额的网格
E_values = E(W, Z, k);                % 计算对应的收入

% 可视化灵敏度分析结果：生成热力图和散点图
figure('Position', [100, 100, 1200, 500], 'Color', 'white');
fontName = 'Times New Roman';
titleFontSize = 16;
labelFontSize = 14;
legendFontSize = 12;
lineWidth = 2;

% 第一个子图：散点图
subplot(1, 2, 1)
scatter3(W(:), Z(:), E_values(:)/1e6, 10, E_values(:)/1e6, 'filled');
colormap(parula);  % 使用平和的渐变配色方案
colorbar;
xlabel('\it{Tourist Number}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Consumption ($)}', 'FontName', fontName, 'FontSize', labelFontSize);
zlabel('\it{Revenue} \rm{(Million $)}', 'FontName', fontName, 'FontSize', labelFontSize);
title('\bf{Tourism Revenue Scatter Plot}', 'FontName', fontName, 'FontSize', titleFontSize);
grid on;

% 第二个子图：热力图
subplot(1, 2, 2);
h = heatmap(w_range, z_range, E_values, 'Colormap', parula, 'Title', 'Heatmap of Tourism Revenue');

% 设置热力图的标签
h.XLabel = '\it{Tourist Number}';  % 设置x轴标签
h.YLabel = '\it{Consumption ($)}';  % 设置y轴标签

% 整体标题
sgtitle('\bf{Tourism Revenue Optimization Analysis for Juneau}', ...
    'FontName', fontName, ...
    'FontSize', titleFontSize + 2);

% 调整图形外观
set(gcf, 'PaperPositionMode', 'auto');
