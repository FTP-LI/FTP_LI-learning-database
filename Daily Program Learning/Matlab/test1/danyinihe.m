data = xlsread('data.xlsx', 'Sheet1', 'A:B');
x = data(:, 1);
y = data(:, 2);

% 移动平均处理
windowSize = 3; % 移动平均窗口大小
yMovingAvg = movmean(y, windowSize);

% 多项式拟合
degree = 2;
poti = polyfit(x, yMovingAvg, degree);

xFit = min(x):0.1:max(x);
yFit = polyval(poti, xFit);

plot(x, y, 'o', xFit, yFit);
hold on;
plot(x, yMovingAvg, 'r--', 'LineWidth', 2);
xlabel('体重/KG');
ylabel('身高/mm');
title('体重与身高关系');
legend('数据样本', '拟合曲线', '移动平均处理');
hold off;

fitFunction = poly2str(poti, 'x');
fprintf('拟合的函数为: %s\n', fitFunction);