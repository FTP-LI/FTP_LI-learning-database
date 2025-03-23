% 清空工作空间并设置随机种子以确保结果可复现
clear all; close all; clc;
rng(2024);

% 模拟参数
n = 1000;  % 增加样本数量以提高分析的稳健性
title_fontsize = 24;
label_fontsize = 22;
legend_fontsize = 20;

% 收入分配参数：设置不同时间点的收入差异
mu1 = 2;    % 时间点1：自然保护区建立前，收入差距大
sigma1 = 6.5;  % 时间点1：增加标准差，产生更大的收入差异
max_income1 = 500000;  % 时间点1：收入上限较高

mu2 = 3;    % 时间点2：自然保护区建立后，收入差距适中
sigma2 = 3.5;  % 时间点2：增加标准差，收入波动适中
max_income2 = 300000;  % 时间点2：收入上限适中

mu3 = 4;   % 时间点3：交通系统建设后，收入差距较小
sigma3 = 5.5;  % 时间点3：增大标准差，确保仍有收入差异
max_income3 = 200000;  % 时间点3：收入上限较低

% 生成收入数据并限制收入上限
income1 = max(min(lognrnd(mu1, sigma1, [1, n]) * 10000, max_income1), 5000);
income2 = max(min(lognrnd(mu2, sigma2, [1, n]) * 10000, max_income2), 5000);
income3 = max(min(lognrnd(mu3, sigma3, [1, n]) * 10000, max_income3), 5000);

% 对收入数据进行排序以计算洛伦兹曲线
sorted_income1 = sort(income1);
sorted_income2 = sort(income2);
sorted_income3 = sort(income3);

% 计算累积收入百分比
cumulative_population = (1:n) / n;
cumulative_income1 = cumsum(sorted_income1) / sum(sorted_income1);
cumulative_income2 = cumsum(sorted_income2) / sum(sorted_income2);
cumulative_income3 = cumsum(sorted_income3) / sum(sorted_income3);

% 可视化
figure('Position', [100, 100, 1000, 700], 'Color', 'white');
hold on;

% 绘制洛伦兹曲线
plot(cumulative_population, cumulative_income1, '-o', 'LineWidth', 2, ...
    'Color', [0.8 0.1 0.1], 'MarkerSize', 6, 'DisplayName', 'Before the establishment of nature reserves');
plot(cumulative_population, cumulative_income2, '-x', 'LineWidth', 2, ...
    'Color', [0.1 0.6 0.1], 'MarkerSize', 6, 'DisplayName', 'After the establishment of the nature reserve');
plot(cumulative_population, cumulative_income3, '-s', 'LineWidth', 2, ...
    'Color', [0.1 0.1 0.8], 'MarkerSize', 6, 'DisplayName', 'After the construction of the transportation system');

% 完美平等线
plot([0, 1], [0, 1], 'r--', 'LineWidth', 2, 'DisplayName', 'Perfect Equality');

% 设置图形格式
xlabel('Cumulative Population Percentage', 'FontSize', label_fontsize, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
ylabel('Cumulative Income Percentage', 'FontSize', label_fontsize, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
title('Juneau Income Distribution: Lorenz Curve Analysis', 'FontSize', title_fontsize, 'FontWeight', 'bold', 'FontName', 'Times New Roman');
legend('show', 'Location', 'northwest', 'FontSize', legend_fontsize, 'FontName', 'Times New Roman');
grid on;
set(gca, 'FontSize', 20, 'LineWidth', 1, 'FontName', 'Times New Roman');

% 计算基尼系数
gini1 = calculateGiniCoefficient(income1);
gini2 = calculateGiniCoefficient(income2);
gini3 = calculateGiniCoefficient(income3);

% 控制台输出
fprintf('Gini Coefficient Analysis:\n');
fprintf('Time Point 1 (Before the establishment of nature reserves): %.4f\n', gini1);
fprintf('Time Point 2 (After the establishment of the nature reserve): %.4f\n', gini2);
fprintf('Time Point 3 (After the construction of the transportation system): %.4f\n', gini3);

% 收入分配公平性分析
gini_values = [gini1, gini2, gini3];
time_points = {'Time Point 1', 'Time Point 2', 'Time Point 3'};
[min_gini, min_index] = min(gini_values);

fprintf('\nIncome Distribution Fairness Analysis:\n');
fprintf('Most Equal Time Point: %s (Gini Coefficient: %.4f)\n', time_points{min_index}, min_gini);

% 基尼系数计算函数（放在脚本末尾）
function gini = calculateGiniCoefficient(income)
    % 对收入进行升序排序
    sorted_income = sort(income);
    n = length(income);
    
    % 计算累积收入
    total_income = sum(sorted_income);
    
    % 计算基尼系数（使用平均绝对差法）
    mean_income = mean(income);
    abs_differences = abs(income' - income);
    gini = sum(sum(abs_differences)) / (2 * n * n * mean_income);
end
