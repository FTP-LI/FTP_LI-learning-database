% Main script for fuzzy optimization
clear;
clc;
format long;

% Fixed decision variables
x1 = 1; x2 = 1; x3 = 1; x4 = 1;

% Example data (replace with actual values)
a1 = 200000; b1 = 300000; c1 = 400000; % Daily
a2 = 30; b2 = 50; c2 = 80; % USD
m1 = 5000; n1 = 15000; p1 = 25000; q1 = 35000; % Daily
m2 = 300000; n2 = 350000; p2 = 400000; q2 = 450000;

% Global variables to store iteration data
global ITERATION_DATA WEIGHT_DATA CURRENT_VALUES;
ITERATION_DATA = [];
WEIGHT_DATA = [];
CURRENT_VALUES = struct('funcValue', [], 'weights', []);

% Initial weight coefficients
alpha0 = [0.25, 0.25, 0.25, 0.25];

% Objective function handle
obj_fun = @(alpha) -objective_function(alpha(1), a1, b1, c1, x1, ...
    alpha(2), a2, b2, c2, x2, ...
    alpha(3), m1, n1, p1, q1, x3, ...
    alpha(4), m2, n2, p2, q2, x4);

% Constraints
A = []; b = [];
Aeq = [1, 1, 1, 1]; % Sum of weights = 1
beq = 1;
lb = [0.2, 0.2, 0.2, 0.2]; % Lower bound for weights
ub = [1, 1, 1, 1]; % Upper bound for weights

% Optimization options
options = optimoptions('fmincon', ...
    'Display', 'iter', ...
    'OutputFcn', @iteration_tracker, ...
    'PlotFcns', {@custom_plot});

% Optimization
[alpha_opt, Z_opt] = fmincon(obj_fun, alpha0, A, b, Aeq, beq, lb, ub, [], options);

% Output results
fprintf('Optimal weights: %.4f, %.4f, %.4f, %.4f\n', alpha_opt(1), alpha_opt(2), alpha_opt(3), alpha_opt(4));
fprintf('Maximum Z: %.4f\n', -Z_opt);

% Objective function
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

% Iteration tracking function
function stop = iteration_tracker(x, optimValues, state)
    global ITERATION_DATA WEIGHT_DATA CURRENT_VALUES;
    stop = false;
    
    switch state
        case 'iter'
            % Store iteration number and absolute function value
            current_iter = numel(ITERATION_DATA) + 1;
            current_fval = abs(optimValues.fval);
            
            ITERATION_DATA = [ITERATION_DATA; current_iter, current_fval];
            WEIGHT_DATA = [WEIGHT_DATA; current_iter, x];
            
            % Store current values for title
            CURRENT_VALUES.funcValue = current_fval;
            CURRENT_VALUES.weights = x;
    end
end

% Custom plotting function
function stop = custom_plot(~, optimValues, state)
    global ITERATION_DATA WEIGHT_DATA CURRENT_VALUES;
    stop = false;
    
    % Ensure we have data to plot
    if ~isempty(ITERATION_DATA)
        % Create a single figure with multiple subplots
        figure('Position', [100, 100, 1200, 800], ...
               'Name', 'Optimization Analysis', ...
               'NumberTitle', 'off');
        
        % Prepare title strings with current function value and weights
        if ~isempty(CURRENT_VALUES.funcValue)
            func_val_str = sprintf('f(x) = %.4f', CURRENT_VALUES.funcValue);
            weights_str = sprintf('α = [%.3f, %.3f, %.3f, %.3f]', ...
                CURRENT_VALUES.weights(1), CURRENT_VALUES.weights(2), ...
                CURRENT_VALUES.weights(3), CURRENT_VALUES.weights(4));
            full_title = {func_val_str, weights_str};
        else
            full_title = 'Objective Function Value';
        end
        
        % Subplot 1: Objective Function Value
        subplot(2, 2, 1);
        plot(ITERATION_DATA(:,1), ITERATION_DATA(:,2), '-o', ...
            'LineWidth', 2, ...
            'MarkerSize', 8, ...
            'MarkerFaceColor', [0 0.4470 0.7410], ...
            'MarkerEdgeColor', 'black', ...
            'Color', [0 0.4470 0.7410]);
        title(full_title, 'FontWeight', 'bold', 'FontSize', 10, 'FontName', 'Times New Roman');
        xlabel('Iteration', 'FontName', 'Times New Roman');
        ylabel('Function Value', 'FontName', 'Times New Roman');
        grid on;
        
        % Subplot 2: Weight Evolution for Each Parameter
        subplot(2, 2, 2);
        if ~isempty(WEIGHT_DATA)
            plot(WEIGHT_DATA(:,1), WEIGHT_DATA(:,2:end), '-o', ...
                'LineWidth', 2, ...
                'MarkerSize', 6);
            title('Weight Coefficients', 'FontWeight', 'bold', 'FontName', 'Times New Roman');
            xlabel('Iteration', 'FontName', 'Times New Roman');
            ylabel('Weight Value', 'FontName', 'Times New Roman');
            legend('α1', 'α2', 'α3', 'α4', 'Location', 'best', 'FontName', 'Times New Roman');
            grid on;
        end
        
        % Subplot 3: Convergence Progress
        subplot(2, 2, 3);
        if length(ITERATION_DATA(:,2)) > 1
            plot(ITERATION_DATA(2:end,1), diff(ITERATION_DATA(:,2)), '-s', ...
                'LineWidth', 2, ...
                'MarkerSize', 8, ...
                'Color', [0.8500 0.3250 0.0980]);
            title('Convergence Progress', 'FontWeight', 'bold', 'FontName', 'Times New Roman');
            xlabel('Iteration', 'FontName', 'Times New Roman');
            ylabel('Change in Function Value', 'FontName', 'Times New Roman');
            grid on;
        end
        
        % Subplot 4: Cumulative Distribution
        subplot(2, 2, 4);
        if ~isempty(WEIGHT_DATA)
            bar(mean(WEIGHT_DATA(:,2:end)), 'FaceColor', [0.3010 0.7450 0.9330]);
            title('Average Weights', 'FontWeight', 'bold', 'FontName', 'Times New Roman');
            xlabel('Weight Index', 'FontName', 'Times New Roman');
            ylabel('Average Value', 'FontName', 'Times New Roman');
            set(gca, 'XTickLabel', {'α1', 'α2', 'α3', 'α4'}, 'FontName', 'Times New Roman');
        end
        
        % Adjust overall figure styling
        set(findall(gcf, '-property', 'FontName'), 'FontName', 'Times New Roman');
        set(findall(gcf, '-property', 'FontSize'), 'FontSize', 10);
        
        % Main figure title
        annotation('textbox', [0.3, 0.95, 0.4, 0.05], ...
            'String', 'Optimization Process Analysis', ...
            'HorizontalAlignment', 'center', ...
            'FontSize', 14, ...
            'FontWeight', 'bold', ...
            'EdgeColor', 'none', 'FontName', 'Times New Roman');
    end
end
