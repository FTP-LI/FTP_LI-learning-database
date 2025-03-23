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

% 输出最优解
disp('最优解的决策变量和目标值：');
disp(gbest);
[f1_opt, f2_opt] = objective_function(gbest);
disp(['最优解目标1: ', num2str(f1_opt)]);
disp(['最优解目标2: ', num2str(f2_opt)]);

% 1. 输出约束条件表示
disp('约束条件表示：');
disp(['游客数量：', num2str(tourist_range(1)), ' 到 ', num2str(tourist_range(2)), ' 每年']);
disp(['道路维修费：', num2str(road_repair_range(1)), ' 到 ', num2str(road_repair_range(2)), ' 每年']);
disp(['电力供应成本：', num2str(power_cost_range(1)), ' 到 ', num2str(power_cost_range(2)), ' 每年']);
disp(['饮用水供应数量：', num2str(water_supply_range(1)), ' 到 ', num2str(water_supply_range(2)), ' 加仑']);
disp(['废物处理成本：', num2str(waste_cost_range(1)), ' 到 ', num2str(waste_cost_range(2)), ' 每年']);
disp(['碳足迹：', num2str(carbon_footprint_range(1)), ' 到 ', num2str(carbon_footprint_range(2)), ' 吨']);

% 2. 输出位置范围公式（示例）
disp('位置范围公式：');
disp('每个决策变量的值必须在相应的范围内，如游客数量在[100000, 300000]之间。');

% 3. 输出7个粒子的目标值表格
disp('7个粒子的目标值表格（每个粒子的目标1和目标2的值）：');
goal_table = array2table(pbest(1:7,:), ...
    'VariableNames', {'Tourist', 'RoadRepair', 'PowerCost', 'WaterSupply', 'WasteCost', 'CarbonFootprint'});
disp(goal_table);

% 4. 输出资源消耗最少的基础设施维修方案
disp('资源消耗最少的基础设施建设方案（决策变量和对应目标值）：');
optimal_solution_table = array2table(gbest, ...
    'VariableNames', {'Tourist', 'RoadRepair', 'PowerCost', 'WaterSupply', 'WasteCost', 'CarbonFootprint'});
disp(optimal_solution_table);

% 创建一个2x2的子图布局
figure('Position', [100, 100, 1200, 800]);  % 设置图像大小和位置

% 设置全局字体
set(gca, 'FontName', 'Times New Roman', 'FontSize', 12);

% 新配色方案：
light_blue = [0.6, 0.8, 1];  % 浅蓝色
light_orange = [1, 0.8, 0.4];  % 浅橙色
dark_green = [0.2, 0.6, 0.2];  % 深绿色

% 第一个图：Pareto前沿散点图
subplot(2, 2, 1);
scatter(pareto_front(:, 1), pareto_front(:, 2), 50, 'filled', 'MarkerFaceColor', light_blue); % 使用浅蓝色
xlabel('Objective 1 (Infrastructure Pressure)', 'FontSize', 14);
ylabel('Objective 2 (Resource Consumption)', 'FontSize', 14);
title('Pareto Front', 'FontSize', 16, 'FontWeight', 'bold'); % 加粗标题
grid on; % 添加网格

% 第二个图：目标函数收敛曲线
subplot(2, 2, 2);
hold on;
plot(iterations, objective_1_values, 'Color', light_orange, 'LineWidth', 2, 'DisplayName', 'Objective 1'); % 使用浅橙色
plot(iterations, objective_2_values, 'Color', light_blue, 'LineWidth', 2, 'DisplayName', 'Objective 2'); % 使用浅蓝色
xlabel('Iterations', 'FontSize', 14);
ylabel('Objective Value', 'FontSize', 14);
legend('show', 'FontSize', 12, 'Location', 'best');
title('Objective Function Convergence', 'FontSize', 16, 'FontWeight', 'bold'); % 加粗标题
hold off;
grid on;

% 第三个图：决策变量分布图（箱形图）
subplot(2, 2, 3);
boxplot(decision_variables, 'Labels', {'Var 1', 'Var 2', 'Var 3', 'Var 4', 'Var 5', 'Var 6'}, 'Whisker', 1.5);
xlabel('Decision Variable Index', 'FontSize', 14);
ylabel('Decision Variable Value', 'FontSize', 14);
title('Decision Variable Distribution in Pareto Optimal Set', 'FontSize', 16, 'FontWeight', 'bold'); % 加粗标题
grid on;

% 第四个图：目标函数3D可视化
subplot(2, 2, 4);
scatter3(f1_values, f2_values, f3_values, 50, 'filled', 'MarkerFaceColor', dark_green); % 使用深绿色
xlabel('Objective 1', 'FontSize', 14);
ylabel('Objective 2', 'FontSize', 14);
zlabel('Objective 3', 'FontSize', 14);
title('3D Objective Function Space', 'FontSize', 16, 'FontWeight', 'bold'); % 加粗标题
grid on;

% 保存图像
saveas(gcf, 'optimization_results_new_color_scheme_with_bold_titles.png');  % 保存为高质量图片

% 目标函数计算
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
