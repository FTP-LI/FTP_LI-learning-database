% [前面的代码保持不变，直到创建图形之前]

% 创建自定义渐变色系
color1 = [240, 249, 232]/255;  % 浅绿
color2 = [186, 228, 188]/255;  % 中绿
color3 = [123, 204, 196]/255;  % 青绿
color4 = [67, 162, 202]/255;   % 浅蓝
color5 = [8, 104, 172]/255;    % 深蓝
custom_colormap = [color1; color2; color3; color4; color5];
custom_colormap = interp1(linspace(0,1,size(custom_colormap,1)), custom_colormap, linspace(0,1,256));

% 创建单个大图
figure('Position', [100, 100, 1500, 500], 'Color', 'white');

% 子图1：需求量与房价关系的3D散点图
subplot(1, 3, 1);
scatter3(sensitivity_results(:,1), sensitivity_results(:,2), sensitivity_results(:,8), 80, sensitivity_results(:,8), 'filled');
colormap(gca, custom_colormap);
cb = colorbar;
ylabel(cb, 'Demand Level', 'FontName', 'Times New Roman', 'FontSize', 10);
xlabel('e - Demand Constant', 'FontName', 'Times New Roman', 'FontSize', 12);
ylabel('f - Price Sensitivity', 'FontName', 'Times New Roman', 'FontSize', 12);
zlabel('Q_d(t)', 'FontName', 'Times New Roman', 'FontSize', 12);
title('\bf{(a) Housing Demand Sensitivity}', 'FontName', 'Times New Roman', 'FontSize', 14);
grid on;
view(45, 30);
set(gca, 'Box', 'on', 'LineWidth', 1);

% 子图2：供给量与人口增长率关系的3D散点图
subplot(1, 3, 2);
scatter3(sensitivity_results(:,3), sensitivity_results(:,4), sensitivity_results(:,9), 80, sensitivity_results(:,9), 'filled');
colormap(gca, custom_colormap);
cb = colorbar;
ylabel(cb, 'Supply Level', 'FontName', 'Times New Roman', 'FontSize', 10);
xlabel('g - Income Sensitivity', 'FontName', 'Times New Roman', 'FontSize', 12);
ylabel('h - Population Sensitivity', 'FontName', 'Times New Roman', 'FontSize', 12);
zlabel('Q_s(t)', 'FontName', 'Times New Roman', 'FontSize', 12);
title('\bf{(b) Housing Supply Sensitivity}', 'FontName', 'Times New Roman', 'FontSize', 14);
grid on;
view(45, 30);
set(gca, 'Box', 'on', 'LineWidth', 1);

% 子图3：需求量热力图
subplot(1, 3, 3);
imagesc(e_range, f_range, Qd_heatmap');
colormap(gca, custom_colormap);
cb = colorbar;
ylabel(cb, 'Demand Level Q_d(t)', 'FontName', 'Times New Roman', 'FontSize', 10);
xlabel('e - Demand Constant', 'FontName', 'Times New Roman', 'FontSize', 12);
ylabel('f - Price Sensitivity', 'FontName', 'Times New Roman', 'FontSize', 12);
title('\bf{(c) Demand Level Distribution}', 'FontName', 'Times New Roman', 'FontSize', 14);
set(gca, 'YDir', 'normal', 'Box', 'on', 'LineWidth', 1);
axis square;

% 调整整体布局
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPosition', [0 0 15 5]);

% 添加总标题
sgtitle('\bf{Housing Market Sensitivity Analysis}', 'FontName', 'Times New Roman', 'FontSize', 16);

% 调整子图之间的间距
set(gcf, 'Units', 'normalized');
ha = axes('Position', [0 0 1 1], 'Visible', 'off');
set(gcf, 'Units', 'pixels');

% 保存高分辨率图像
print('combined_sensitivity_analysis', '-dpng', '-r300');