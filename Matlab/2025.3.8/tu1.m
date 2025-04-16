% 加载数据
data = readtable('船舶AIS信息数据训练集.csv');

% 提取相关列 (lon, lat, hdg, sog, cog, draught)
data_subset = data{:, {'lon', 'lat', 'hdg', 'sog', 'cog', 'draught'}};

% 创建箱线图
figure;
boxplot(data_subset, 'Labels', {'lon', 'lat', 'hdg', 'sog', 'cog', 'draught'}, 'Widths', 0.6);

% 自定义标题和轴标签
title('训练集数据分析箱线图', 'FontSize', 14, 'FontName', 'SimHei');
xlabel('变量', 'FontSize', 12, 'FontName', 'SimHei');
ylabel('值', 'FontSize', 12, 'FontName', 'SimHei');

% 设置字体以避免中文乱码问题
set(gca, 'FontName', 'SimHei', 'FontSize', 12);
set(gca, 'TickLabelInterpreter', 'none');  % 防止刻度标签解析错误

% 修改箱线的颜色和填充
h = findobj(gca, 'Tag', 'Box');
num_boxes = length(h);

% 定义渐变色（从浅蓝到深蓝的渐变色系）
cmap = [linspace(0.6, 0, num_boxes)', linspace(0.8, 0, num_boxes)', linspace(1, 0.8, num_boxes)'];

for i = 1:num_boxes
    % 获取渐变颜色
    color = cmap(i, :);
    % 使用patch函数填充箱子并调整透明度和边框颜色
    patch(get(h(i), 'XData'), get(h(i), 'YData'), color, 'FaceAlpha', 0.6, 'EdgeColor', 'black', 'LineWidth', 1.5);
end

% 打开网格
grid on;
