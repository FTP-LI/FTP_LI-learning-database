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

% 定义拉格朗日函数
L = @(w, z, k, lambda1, lambda2) E(w, z, k) - lambda1 * F1(w) - lambda2 * F2(w, z);

% 求解最优化问题
syms w z lambda1 lambda2;
eqns = [diff(L(w, z, k, lambda1, lambda2), w) == 0, ...
        diff(L(w, z, k, lambda1, lambda2), z) == 0, ...
        F1(w) == 0, ...
        F2(w, z) == 0];
sol = solve(eqns, [w, z, lambda1, lambda2]);

% 获取数值解
w_opt_num = double(vpa(sol.w));
z_opt_num = double(vpa(sol.z));
lambda1_opt_num = double(vpa(sol.lambda1));
lambda2_opt_num = double(vpa(sol.lambda2));

% 输出最优解
disp('最优解：');
disp(['最优游客数 w: ', num2str(w_opt_num)]);
disp(['最优消费额 z: ', num2str(z_opt_num)]);
disp(['拉格朗日乘子 lambda1: ', num2str(lambda1_opt_num)]);
disp(['拉格朗日乘子 lambda2: ', num2str(lambda2_opt_num)]);

% 创建图形窗口并设置基本参数
figure('Position', [100, 100, 1200, 500], 'Color', 'white');
fontName = 'Times New Roman';
titleFontSize = 16;
labelFontSize = 14;
legendFontSize = 12;
lineWidth = 2;

% 为3D图生成更合适的数据范围
w_values_3d = linspace(0, w_max, 100);
z_values_3d = linspace(0, 1000, 100);  % 增加消费范围到1000
[W, Z] = meshgrid(w_values_3d, z_values_3d);

% 收入函数
E_func = @(w, z) w .* z .* (1 - epsilon * (w - w_max/2).^2) .* k;  % 新增市场需求系数
E_values = E_func(W, Z);

% 第一个子图：3D surface plot
subplot(1, 2, 1)
surf(W, Z, E_values/1e6, 'EdgeColor', 'none', 'FaceAlpha', 0.8);  % 将收入转换为百万单位
colormap('jet');
c = colorbar('FontName', fontName, 'FontSize', labelFontSize);
ylabel(c, 'Revenue (Million $)', 'FontName', fontName, 'FontSize', labelFontSize);

% 设置3D图属性
xlabel('\it{Tourist Number}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Consumption ($)}', 'FontName', fontName, 'FontSize', labelFontSize);
zlabel('\it{Revenue} \rm{(Million $)}', 'FontName', fontName, 'FontSize', labelFontSize);
title('\bf{Tourism Revenue Surface}', 'FontName', fontName, 'FontSize', titleFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out');
grid on;

% 为2D图生成数据
w_values_2d = linspace(0, w_max, 1000);
E_values_2d = arrayfun(@(w) E_func(w, z_value), w_values_2d);

% 第二个子图：2D plot
subplot(1, 2, 2)
plot(w_values_2d/1000, E_values_2d/1e6, 'LineWidth', lineWidth, 'Color', [0.2 0.4 0.8]);
hold on;

% 检查最优点是否有效
if ~isempty(w_opt_num) && ~isnan(w_opt_num) && w_opt_num > 0 && z_opt_num > 0
    plot(w_opt_num/1000, E_func(w_opt_num, z_opt_num)/1e6, 'ro', 'MarkerSize', 10, 'LineWidth', 2); % 红色圆点并加大标记
end

% 设置2D图属性
xlabel('\it{Tourist Number} \rm{(Thousands)}', 'FontName', fontName, 'FontSize', labelFontSize);
ylabel('\it{Revenue} \rm{(Million $)}', 'FontName', fontName, 'FontSize', labelFontSize);
title('\bf{Revenue with Fixed Consumption}', 'FontName', fontName, 'FontSize', titleFontSize);
set(gca, 'FontName', fontName, 'FontSize', labelFontSize, 'Box', 'on', ...
    'LineWidth', 1.2, 'TickDir', 'out', 'TickLength', [0.02 0.02]);
grid on;

% 添加图例
legend('\it{Revenue Function}', '\it{Optimal Point}', ...
    'Location', 'northeast', ...
    'FontName', fontName, ...
    'FontSize', legendFontSize);

% 整体标题
sgtitle('\bf{Tourism Revenue Optimization Analysis for Juneau}', ...
    'FontName', fontName, ...
    'FontSize', titleFontSize + 2);

% 调整图形外观
set(gcf, 'PaperPositionMode', 'auto');
