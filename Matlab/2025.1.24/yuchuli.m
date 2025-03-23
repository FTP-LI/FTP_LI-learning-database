% 假设的决策变量范围
tourist_range = [100000, 300000];  % 游客数量范围：10万到30万
road_repair_range = [5000000, 20000000];  % 道路维修费范围：500万到2000万美元
power_cost_range = [3000000, 15000000];  % 电力供应成本范围：300万到1500万美元
water_supply_range = [5000000, 20000000];  % 饮用水供应范围：500万到2000万加仑
waste_cost_range = [2000000, 10000000];  % 废物处理成本范围：200万到1000万美元
carbon_footprint_range = [500000, 2000000];  % 碳足迹范围：50万到200万吨

% 粒子群初始化
num_particles = 50;
num_dimensions = 6;  % 6个决策变量
max_iter = 50;  % 最大迭代次数

% 约束范围
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
function [f1, f2] = objective_function(positions)
    tourist = positions(:,1);
    road_repair = positions(:,2);
    power_cost = positions(:,3);
    water_supply = positions(:,4);
    waste_cost = positions(:,5);
    carbon_footprint = positions(:,6);

    % 目标函数1：基础设施压力（加权和）
    f1 = tourist + road_repair + power_cost;

    % 目标函数2：资源消耗（加权和）
    f2 = water_supply + waste_cost + carbon_footprint;
end

% 粒子群更新
w = 0.5;  % 惯性权重
phi1 = 2; % 个体学习因子
phi2 = 2; % 社会学习因子

% 初始化全局和个体最优解
pbest = positions;  % 每个粒子的最佳位置
gbest = positions(1,:);  % 全局最佳位置
pbest_fitness = inf(num_particles, 1);
gbest_fitness = inf;

% 迭代过程
for iter = 1:max_iter
    for i = 1:num_particles
        [f1, f2] = objective_function(positions(i,:));
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

% 输出最优解
disp('最优解的决策变量和目标值：');
disp(gbest);
[f1_opt, f2_opt] = objective_function(gbest);
disp(['最优解目标1: ', num2str(f1_opt)]);
disp(['最优解目标2: ', num2str(f2_opt)]);

% 可视化结果
% 1. Pareto前沿
f1_values = pbest_fitness;
f2_values = pbest_fitness;

figure;
scatter(f1_values, f2_values, 'filled');
xlabel('Objective 1 (Infrastructure Pressure)');
ylabel('Objective 2 (Resource Consumption)');
title('Pareto Front');
grid on;

% 2. 目标函数收敛曲线
iterations = 1:max_iter;
figure;
plot(iterations, f1_values, 'b-', 'LineWidth', 2, 'DisplayName', 'Objective 1');
hold on;
plot(iterations, f2_values, 'r-', 'LineWidth', 2, 'DisplayName', 'Objective 2');
xlabel('Iterations');
ylabel('Objective Value');
legend('show');
title('Objective Function Convergence');
grid on;

% 3. 决策变量分布（箱形图）
decision_variables = positions;
figure;
boxplot(decision_variables, 'Labels', {'Tourist', 'RoadRepair', 'PowerCost', 'WaterSupply', 'WasteCost', 'CarbonFootprint'});
xlabel('Decision Variable Index');
ylabel('Decision Variable Value');
title('Decision Variable Distribution in Pareto Optimal Set');
grid on;

% 4. 目标函数3D可视化
figure;
scatter3(f1_values, f2_values, f1_values + f2_values, 80, 'filled'); % 示例为目标1、目标2和其和
xlabel('Objective 1');
ylabel('Objective 2');
zlabel('Objective 1 + Objective 2');
title('3D Objective Function Space');
grid on;
