% Main script
clear;
clc;
format long;

% Fixed decision variables
x1 = 1; x2 = 1; x3 = 1; x4 = 1;

% Example data
a1 = 200000; b1 = 300000; c1 = 400000; % Daily
a2 = 30; b2 = 50; c2 = 80; % USD
m1 = 5000; n1 = 15000; p1 = 25000; q1 = 35000; % Daily
m2 = 300000; n2 = 350000; p2 = 400000; q2 = 450000;

% Global variables for iteration tracking
global ITERATION_DATA WEIGHT_DATA CURRENT_VALUES;
ITERATION_DATA = [];
WEIGHT_DATA = [];
CURRENT_VALUES = struct('funcValue', [], 'weights', []);

% Initial weights
alpha0 = [0.25, 0.25, 0.25, 0.25];

% Create objective function
obj_fun = @(alpha) -objective_function(alpha(1), a1, b1, c1, x1, ...
    alpha(2), a2, b2, c2, x2, ...
    alpha(3), m1, n1, p1, q1, x3, ...
    alpha(4), m2, n2, p2, q2, x4);

% Optimization constraints
A = []; b = [];
Aeq = [1, 1, 1, 1];
beq = 1;
lb = [0.2, 0.2, 0.2, 0.2];
ub = [1, 1, 1, 1];

% Optimization options
options = optimoptions('fmincon', ...
    'Display', 'off', ...
    'OutputFcn', @iteration_tracker, ...
    'PlotFcns', []);

% Define sensitivity analysis parameters
alpha_range = [0.1, 0.5, 0.9];

% Perform sensitivity analysis and visualization
[sensitivity_results, analysis_data] = sensitivity_analysis(alpha_range, obj_fun, alpha0, ...
    A, b, Aeq, beq, lb, ub, options);

% Create academic visualization
create_academic_plots(analysis_data.alpha1_vals, analysis_data.alpha2_vals, ...
    analysis_data.func_values, alpha_range);

% Display results
disp('Sensitivity Analysis Results:');
disp(sensitivity_results);

function Z = objective_function(alpha1, a1, b1, c1, x1, ...
    alpha2, a2, b2, c2, x2, ...
    alpha3, m1, n1, p1, q1, x3, ...
    alpha4, m2, n2, p2, q2, x4)
    
    % Calculate expected values for triangular fuzzy numbers
    A1 = (a1 + b1 + c1) / 3;
    A2 = (a2 + b2 + c2) / 3;
    
    % Calculate expected values for trapezoidal fuzzy numbers
    B1 = (m1 + n1 + p1 + q1) / 4;
    B2 = (m2 + n2 + p2 + q2) / 4;
    
    % Calculate objective function
    Z = alpha1 * A1 * x1 + alpha2 * A2 * x2 + alpha3 * B1 * x3 + alpha4 * B2 * x4;
end

function [results, analysis_data] = sensitivity_analysis(alpha_range, obj_fun, alpha0, ...
    A, b, Aeq, beq, lb, ub, options)
    
    % Initialize results structure
    results = struct('alpha', [], 'func_value', [], 'weights', []);
    
    % Calculate total number of combinations
    num_combinations = length(alpha_range)^4;
    
    % Initialize arrays for storing results
    alpha1_vals = zeros(num_combinations, 1);
    alpha2_vals = zeros(num_combinations, 1);
    alpha3_vals = zeros(num_combinations, 1);
    alpha4_vals = zeros(num_combinations, 1);
    func_values = zeros(num_combinations, 1);
    
    % Counter for storing results
    counter = 1;
    
    % Perform sensitivity analysis
    for i = 1:length(alpha_range)
        for j = 1:length(alpha_range)
            for k = 1:length(alpha_range)
                for l = 1:length(alpha_range)
                    % Set current alpha combination
                    alpha_current = [alpha_range(i), alpha_range(j), alpha_range(k), alpha_range(l)];
                    
                    % Perform optimization
                    [alpha_opt, Z_opt] = fmincon(obj_fun, alpha0, A, b, Aeq, beq, lb, ub, [], options);
                    
                    % Store results
                    alpha1_vals(counter) = alpha_range(i);
                    alpha2_vals(counter) = alpha_range(j);
                    alpha3_vals(counter) = alpha_range(k);
                    alpha4_vals(counter) = alpha_range(l);
                    func_values(counter) = -Z_opt;
                    
                    counter = counter + 1;
                end
            end
        end
    end
    
    % Store results
    results.alpha = [alpha1_vals, alpha2_vals, alpha3_vals, alpha4_vals];
    results.func_value = func_values;
    results.weights = [alpha1_vals, alpha2_vals, alpha3_vals, alpha4_vals];
    
    % Package analysis data for visualization
    analysis_data = struct('alpha1_vals', alpha1_vals, ...
        'alpha2_vals', alpha2_vals, ...
        'func_values', func_values);
end

function create_academic_plots(alpha1_vals, alpha2_vals, func_values, alpha_range)
    % Create figure with specific size for academic papers
    fig = figure('Position', [100, 100, 1200, 500]);
    
    % Set default font properties
    set(fig, 'DefaultTextFontname', 'Times New Roman');
    set(fig, 'DefaultAxesFontname', 'Times New Roman');
    
    % Scatter plot
    subplot(1, 2, 1);
    scatter3(alpha1_vals, alpha2_vals, func_values, 50, func_values, 'filled');
    
    % Style scatter plot
    ax1 = gca;
    set(ax1, 'FontSize', 11);
    set(ax1, 'LineWidth', 1.2);
    set(ax1, 'Box', 'on');
    set(ax1, 'TickDir', 'out');
    
    xlabel('\alpha_{1}', 'FontSize', 12, 'Interpreter', 'tex');
    ylabel('\alpha_{2}', 'FontSize', 12, 'Interpreter', 'tex');
    zlabel('Objective Function Value', 'FontSize', 12);
    title('Parameter Space Exploration', 'FontSize', 13, 'FontWeight', 'bold');
    
    c1 = colorbar;
    c1.Label.String = 'Function Value';
    c1.Label.FontSize = 11;
    colormap(ax1, parula);
    
    % Create and plot heatmap
    subplot(1, 2, 2);
    heatmap_size = length(alpha_range);
    heatmap_data = zeros(heatmap_size, heatmap_size);
    count_matrix = zeros(heatmap_size, heatmap_size);
    
    for idx = 1:length(func_values)
        i = find(alpha_range == alpha1_vals(idx));
        j = find(alpha_range == alpha2_vals(idx));
        heatmap_data(i, j) = heatmap_data(i, j) + func_values(idx);
        count_matrix(i, j) = count_matrix(i, j) + 1;
    end
    
    heatmap_data = heatmap_data ./ count_matrix;
    
    ax2 = gca;
    imagesc(alpha_range, alpha_range, heatmap_data);
    set(ax2, 'FontSize', 11);
    set(ax2, 'LineWidth', 1.2);
    set(ax2, 'YDir', 'normal');
    
    xlabel('\alpha_{1}', 'FontSize', 12, 'Interpreter', 'tex');
    ylabel('\alpha_{2}', 'FontSize', 12, 'Interpreter', 'tex');
    title('Parameter Interaction Heat Map', 'FontSize', 13, 'FontWeight', 'bold');
    
    c2 = colorbar;
    c2.Label.String = 'Average Function Value';
    c2.Label.FontSize = 11;
    colormap(ax2, jet);
    
    % Save high-resolution outputs
    print(fig, 'sensitivity_analysis_academic', '-dpng', '-r300');
    saveas(fig, 'sensitivity_analysis_academic.fig');
end

function stop = iteration_tracker(x, optimValues, state)
    global ITERATION_DATA WEIGHT_DATA CURRENT_VALUES;
    stop = false;
    
    switch state
        case 'iter'
            current_iter = numel(ITERATION_DATA) + 1;
            current_fval = abs(optimValues.fval);
            
            ITERATION_DATA = [ITERATION_DATA; current_iter, current_fval];
            WEIGHT_DATA = [WEIGHT_DATA; current_iter, x];
            
            CURRENT_VALUES.funcValue = current_fval;
            CURRENT_VALUES.weights = x;
    end
end