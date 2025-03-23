% 假设的Pareto前沿数据
pareto_front = rand(50, 2);  % 假设50个解的目标1和目标2值

% 假设的目标函数收敛曲线数据
iterations = 1:50;  % 假设有50代迭代
objective_1_values = rand(1, 50);  % 假设目标1值
objective_2_values = rand(1, 50);  % 假设目标2值

% 假设的决策变量分布数据
decision_variables = rand(50, 6);  % 假设有6个决策变量

% 假设目标函数的3D数据
f1_values = rand(1, 50);  % 假设目标1值
f2_values = rand(1, 50);  % 假设目标2值
f3_values = rand(1, 50);  % 假设目标3值

% 粒子群优化（PSO）初始化
num_particles = 50;
num_dimensions = 6;  % 6个决策变量
max_iter = 50;  % 最大迭代次数

% 约束范围
tourist_range = [100000, 300000];  % 游客数量范围
road_repair_range = [5000000, 20000000];  % 道路维修费范围
power_cost_range = [3000000, 15000000];  % 电力供应成本范围
water_supply_range = [5000000, 20000000];  % 饮用水供应范围
waste_cost_range = [2000000, 10000000];  % 废物处理成本范围
carbon_footprint_range = [500000, 2000000];  % 碳足迹范围

ranges = [
    tourist_range;
    road_repair_range;
    power_cost_range;
    water_supply_range;
    waste_cost_range;
    carbon_footprint_range
];

% 粒子初始化
positions = rand(num_particles, num_dimensions) .* (ranges(:,2)' - ranges(:,1)') + ranges(:,1)';
velocities = rand(num_particles, num_dimensions) * 0.1;

% 粒子群适应度计算
w = 0.5;  % 惯性权重
phi1 = 2; % 个体学习因子
phi2 = 2; % 社会学习因子

% 初始化全局和个体最优解
pbest = positions;  % 每个粒子的最佳位置
gbest = positions(1,:);  % 全局最佳位置
pbest_fitness = inf(num_particles, 1);
gbest_fitness = inf;

% 目标函数计算
for iter = 1:max_iter
    for i = 1:num_particles
        [f1, f2] = objective_function(positions(i,:));  % 计算目标函数值
        fitness = f1 + f2;  % 适应度是两个目标函数的加权和
        if fitness < pbest_fitness(i)
            pbest(i,:) = positions(i,:);  % 更新个体最优解
            pbest_fitness(i) = fitness;
        end
        if fitness < gbest_fitness
            gbest = positions(i,:);  % 更新全局最优解
            gbest_fitness = fitness;
        end
    end
    
    % 更新粒子速度和位置
    for i = 1:num_particles
        velocities(i,:) = w * velocities(i,:) + phi1 * rand * (pbest(i,:) - positions(i,:)) + phi2 * rand * (gbest - positions(i,:));
        positions(i,:) = positions(i,:) + velocities(i,:);

        % 保证粒子在约束范围内
        positions(i,:) = max(positions(i,:), ranges(:,1)');
        positions(i,:) = min(positions(i,:), ranges(:,2)');
    end
end

% 执行灵敏度分析
% 假设我们对第一个决策变量（tourist）进行灵敏度分析
tourist_range_sensitivity = linspace(tourist_range(1), tourist_range(2), 50);  % 变化范围
objective_1_sensitivity = zeros(length(tourist_range_sensitivity), 1);
objective_2_sensitivity = zeros(length(tourist_range_sensitivity), 1);

for i = 1:length(tourist_range_sensitivity)
    % 固定其他决策变量，在灵敏度分析中只改变tourist
    positions_sensitivity = gbest;
    positions_sensitivity(1) = tourist_range_sensitivity(i);  % 只改变tourist
    
    [f1, f2] = objective_function(positions_sensitivity);  % 计算目标函数
    objective_1_sensitivity(i) = f1;
    objective_2_sensitivity(i) = f2;
end

% 1. 创建一个新的图形窗口
figure;

% 2. 使用subplot绘制第一个图（散点图）
subplot(1, 2, 1);  % 1行2列的第一个图
scatter(objective_1_sensitivity, objective_2_sensitivity, 'filled');
xlabel('Objective 1 (Infrastructure Pressure)', 'FontSize', 14);
ylabel('Objective 2 (Resource Consumption)', 'FontSize', 14);
title('Sensitivity Analysis Scatter Plot', 'FontSize', 16, 'FontWeight', 'bold');
grid on;

% 3. 使用subplot绘制第二个图（热力图）
subplot(1, 2, 2);  % 1行2列的第二个图
% 对tourist值变化的目标函数1和目标函数2的热力图
[X, Y] = meshgrid(tourist_range_sensitivity, [1, 2]); % X为tourist，Y为目标函数类型
Z = [objective_1_sensitivity'; objective_2_sensitivity'];  % Z为目标函数值

imagesc(tourist_range_sensitivity, [1, 2], Z);  % 绘制热力图
set(gca, 'YTick', [1, 2], 'YTickLabel', {'Objective 1', 'Objective 2'});
xlabel('Tourist Quantity', 'FontSize', 14);
ylabel('Objective Function', 'FontSize', 14);
title('Sensitivity Analysis Heatmap', 'FontSize', 16, 'FontWeight', 'bold');
colorbar; % 添加颜色条


% 目标函数定义放在文件的最后
function [f1, f2] = objective_function(positions)
    tourist = positions(1);
    road_repair = positions(2);
    power_cost = positions(3);
    water_supply = positions(4);
    waste_cost = positions(5);
    carbon_footprint = positions(6);

    % 目标函数1：基础设施压力（加权和）
    f1 = tourist + road_repair + power_cost;

    % 目标函数2：资源消耗（加权和）
    f2 = water_supply + waste_cost + carbon_footprint;
end
