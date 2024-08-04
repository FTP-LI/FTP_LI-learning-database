% 数据导入
PH = [3, 5, 6, 7, 9];
temperature = [15, 25, 30, 35, 45];

% 响应变量矩阵
Removal_rate = [
    9.35,	9.35,	9.6,	4.53,	4.1;
    9.29,	9.26,	9.17	,4.45	,3.91;
    9.45,	9.39,	9.36	,4.56	,4.07;
    8.41,	8.48,	8.35	,4.29	,3.58;
    8.39,	8.32,	8.22,	3.9	,3.47;
]

% 将温度和PH值转为线性数组
X = [];
for i = 1:length(temperature)
    for j = 1:length(PH)
        X = [X; temperature(i), PH(j)];
    end
end

% 调整响应变量为列向量
Y = reshape(Removal_rate', [], 1);

% 执行多次多项式回归
% 创建包含多次项的模型矩阵
X_poly = [X, X(:,1).^2, X(:,2).^2, X(:,1).*X(:,2), X(:,1).^3, X(:,2).^3, X(:,1).^4, X(:,2).^4];
mdl = fitlm(X_poly, Y);

disp(mdl)

% 提取回归系数和截距
coefficients = mdl.Coefficients.Estimate;
intercept = coefficients(1);
slope_temp = coefficients(2);
slope_PH = coefficients(3);
slope_temp2 = coefficients(4);
slope_PH2 = coefficients(5);
slope_temp_PH = coefficients(6);
slope_temp3 = coefficients(7);
slope_PH3 = coefficients(8);
slope_temp4 = coefficients(9);
slope_PH4 = coefficients(10);

% 计算回归方程
regression_equation = sprintf('Removal Rate = %.2f + %.2f * Temperature + %.2f * PH + %.2f * Temperature^2 + %.2f * PH^2 + %.2f * Temperature * PH + %.2f * Temperature^3 + %.2f * PH^3 + %.2f * Temperature^4 + %.2f * PH^4', ...
    intercept, slope_temp, slope_PH, slope_temp2, slope_PH2, slope_temp_PH, slope_temp3, slope_PH3, slope_temp4, slope_PH4);

% 绘制散点图和回归面
scatter3(X(:,1), X(:,2), Y, 'filled');
hold on;

% 创建用于绘制回归面的网格
[xGrid, yGrid] = meshgrid(min(X(:,1)):1:max(X(:,1)), ...
    min(X(:,2)):1:max(X(:,2)));

% 计算回归面上的预测值
zFit = intercept + slope_temp * xGrid + slope_PH * yGrid + ...
    slope_temp2 * xGrid.^2 + slope_PH2 * yGrid.^2 + ...
    slope_temp_PH * xGrid .* yGrid + ...
    slope_temp3 * xGrid.^3 + slope_PH3 * yGrid.^3 + ...
    slope_temp4 * xGrid.^4 + slope_PH4 * yGrid.^4;

% 绘制回归面
mesh(xGrid, yGrid, zFit, 'FaceAlpha', 0.5);
xlabel('温度','FontSize',20);
ylabel('PH','FontSize',20);
zlabel('总吸附量mg/g','FontSize',20);
title('当吸附剂用量为1g/L时的总吸附量多项式回归分析','FontSize',20);
legend('数据点', '回归曲线', 'Location', 'Best','FontSize',20);

% 显示回归方程
disp('回归方程:');
disp(regression_equation);

% 设置图形属性
grid on;
hold off;
