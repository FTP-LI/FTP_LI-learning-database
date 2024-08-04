% 数据导入
PH = [3, 5, 6, 7, 9];
temperature = [15, 25, 30, 35, 45];
adsorbent_used = [0.2, 0.4, 0.6, 0.8, 1];

% 响应变量矩阵
Removal_rate = [
    92.49,  91.57,  89.44,  92.41,  88.88; % temperature=15 adsorbent_used=0.2 PH=3
    93.12,  92.19,  90.05,  93.04,  89.49; % temperature=25 adsorbent_used=0.2 PH=5
    93.47,  92.54,  90.39,  93.39,  89.82; % temperature=30 adsorbent_used=0.2 PH=6
    94.18,  93.24,  91.07,  94.10,  90.51; % temperature=35 adsorbent_used=0.2 PH=7
    94.03,  93.09,  90.93,  93.95,  90.36; % temperature=45 adsorbent_used=0.2 PH=9

    98.40,  96.93,  95.15,  98.32,  94.56; % temperature=15 adsorbent_used=0.4 PH=3
    98.93,  97.94,  95.67,  98.85,  94.96; % temperature=25 adsorbent_used=0.4 PH=5
    99.31,  98.32,  96.03,  99.23,  95.32; % temperature=30 adsorbent_used=0.4 PH=6
    99.80,  99.63,  96.52,  99.73,  95.93; % temperature=35 adsorbent_used=0.4 PH=7
    99.78,  98.78,  96.49,  99.70,  95.89; % temperature=45 adsorbent_used=0.4 PH=9

    94.39,  93.45,  91.28,  94.31,  90.71; % temperature=15 adsorbent_used=0.6 PH=3
    95.03,  94.08,  92.27,  94.95,  91.32; % temperature=25 adsorbent_used=0.6 PH=5
    95.39,  94.44,  92.24,  95.31,  91.67; % temperature=30 adsorbent_used=0.6 PH=6
    96.10,  95.14,  92.93,  96.02,  92.09; % temperature=35 adsorbent_used=0.6 PH=7
    95.93,  94.97,  92.76,  95.85,  92.19; % temperature=45 adsorbent_used=0.6 PH=9

    97.64,  96.66,  94.42,  97.56,  93.83; % temperature=15 adsorbent_used=0.8 PH=3
    97.95,  97.00,  94.72,  97.87,  94.13; % temperature=25 adsorbent_used=0.8 PH=5
    98.21,  97.23,  94.97,  98.63,  94.38; % temperature=30 adsorbent_used=0.8 PH=6
    98.35,  97.37,  95.10,  98.27,  94.51; % temperature=35 adsorbent_used=0.8 PH=7
    98.15,  97.17,  94.91,  98.07,  94.32; % temperature=45 adsorbent_used=0.8 PH=9

    93.27,  92.34,  90.19,  92.73,  89.63; % temperature=15 adsorbent_used=1 PH=3
    93.47,  92.54,  90.39,  93.39,  89.82; % temperature=25 adsorbent_used=1 PH=5
    93.90,  92.96,  90.80,  93.82,  90.24; % temperature=30 adsorbent_used=1 PH=6
    95.05,  94.10,  91.91,  94.97,  91.34; % temperature=35 adsorbent_used=1 PH=7
    94.67, 	93.72, 	91.55, 	94.59, 	90.98; % temperature=45 adsorbent_used=1 PH=9
];

% 执行多次多项式回归
% 创建包含多次项的模型矩阵
X = [];
for k = 1:length(adsorbent_used)
    X_temp = [];
    for i = 1:length(temperature)
        for j = 1:length(PH)
            X_temp = [X_temp; temperature(i), PH(j)];
        end
    end
    X_temp = [X_temp, repmat(adsorbent_used(k), size(X_temp, 1), 1)];
    X = [X; X_temp];
end

% 调整响应变量为列向量
Y = reshape(Removal_rate', [], 1);

% 执行多次多项式回归
% 创建包含多次项的模型矩阵
X_poly = [X, X(:,1).^2, X(:,2).^2, X(:,3).^2, X(:,1).*X(:,2), X(:,1).*X(:,3), X(:,2).*X(:,3), X(:,1).^3, X(:,2).^3, X(:,3).^3, X(:,1).^4, X(:,2).^4, X(:,3).^4];
mdl = fitlm(X_poly, Y);

% 提取回归系数和截距
coefficients = mdl.Coefficients.Estimate;
intercept = coefficients(1);
slope_temp = coefficients(2);
slope_PH = coefficients(3);
slope_adsorbent = coefficients(4);
slope_temp2 = coefficients(5);
slope_PH2 = coefficients(6);
slope_adsorbent2 = coefficients(7);
slope_temp_PH = coefficients(8);
slope_temp3 = coefficients(9);
slope_PH3 = coefficients(10);
slope_adsorbent3 = coefficients(11);
slope_temp4 = coefficients(12);
slope_PH4 = coefficients(13);
slope_adsorbent4 = coefficients(14);

% 计算回归方程
regression_equation = sprintf('Removal Rate = %.2f + %.2f * Temperature + %.2f * PH + %.2f * Adsorbent + %.2f * Temperature^2 + %.2f * PH^2 + %.2f * Adsorbent^2 + %.2f * Temperature * PH + %.2f * Temperature * Adsorbent + %.2f * PH * Adsorbent + %.2f * Temperature^3 + %.2f * PH^3 + %.2f * Adsorbent^3 + %.2f * Temperature^4 + %.2f * PH^4 + %.2f * Adsorbent^4', ...
    intercept, slope_temp, slope_PH, slope_adsorbent, slope_temp2, slope_PH2, slope_adsorbent2, slope_temp_PH, slope_temp3, slope_PH3, slope_adsorbent3, slope_temp4, slope_PH4, slope_adsorbent4);

% 绘制多个图表
figure;

for k = 1:length(adsorbent_used)
    subplot(2, 3, k); % 使用 subplot 分隔图表区域
    scatter3(X(X(:,4)==adsorbent_used(k), 1), X(X(:,4)==adsorbent_used(k), 2), Y(X(:,4)==adsorbent_used(k)), 'filled');
    hold on;

    % 创建用于绘制回归面的网格
    [xGrid, yGrid] = meshgrid(min(X(:,1)):1:max(X(:,1)), min(X(:,2)):1:max(X(:,2)));
    zGrid = repmat(adsorbent_used(k), size(xGrid)); % 固定当前 adsorbent_used

    % 计算回归面上的预测值
    zFit = intercept + slope_temp * xGrid + slope_PH * yGrid + slope_adsorbent * zGrid + ...
        slope_temp2 * xGrid.^2 + slope_PH2 * yGrid.^2 + slope_adsorbent2 * zGrid.^2 + ...
        slope_temp_PH * xGrid .* yGrid + slope_temp3 * xGrid.^3 + slope_PH3 * yGrid.^3 + ...
        slope_adsorbent3 * zGrid.^3 + slope_temp4 * xGrid.^4 + slope_PH4 * yGrid.^4 + ...
        slope_adsorbent4 * zGrid.^4;

    % 绘制回归面
    mesh(xGrid, yGrid, zFit, 'FaceAlpha', 0.5);
    xlabel('Temperature');
    ylabel('PH');
    zlabel('Removal Rate');
    title(sprintf('Adsorbent Used = %.1f', adsorbent_used(k)));

    % 设置图形属性
    grid on;
    hold off;
end

% 显示回归方程
disp('回归方程:');
disp(regression_equation);

