% Clear workspace and set random seed for reproducibility
clear all; close all; clc;
rng(2024);

% Set figure properties for academic styling
set(0, 'DefaultTextFontName', 'Times New Roman');
set(0, 'DefaultAxesFontName', 'Times New Roman');

% Simulation parameters
n = 1000; % Sample size for robust analysis
title_fontsize = 16;    % Reduced for academic style
label_fontsize = 14;    % Reduced for academic style
legend_fontsize = 12;   % Reduced for academic style

% Income distribution parameters
mu_range = [2, 3, 4];        % Different possible values for mu
sigma_range = [6.5, 3.5, 5.5]; % Different possible values for sigma

% Initialize matrices to store Gini coefficients
num_mu = length(mu_range);
num_sigma = length(sigma_range);
gini_matrix_1 = zeros(num_mu, num_sigma);
gini_matrix_2 = zeros(num_mu, num_sigma);
gini_matrix_3 = zeros(num_mu, num_sigma);

% Arrays to store all combinations for scatter plot
gini_values_1 = [];
gini_values_2 = [];
gini_values_3 = [];

% Calculate Gini coefficients for different parameter combinations
for i = 1:num_mu
    for j = 1:num_sigma
        % Generate income data with constraints
        income1 = max(min(lognrnd(mu_range(i), sigma_range(j), [1, n]) * 10000, 500000), 5000);
        income2 = max(min(lognrnd(mu_range(i), sigma_range(j), [1, n]) * 10000, 300000), 5000);
        income3 = max(min(lognrnd(mu_range(i), sigma_range(j), [1, n]) * 10000, 200000), 5000);
        
        % Calculate and store Gini coefficients
        gini_matrix_1(i,j) = calculateGiniCoefficient(income1);
        gini_matrix_2(i,j) = calculateGiniCoefficient(income2);
        gini_matrix_3(i,j) = calculateGiniCoefficient(income3);
        
        % Store values for scatter plot
        gini_values_1 = [gini_values_1; gini_matrix_1(i,j)];
        gini_values_2 = [gini_values_2; gini_matrix_2(i,j)];
        gini_values_3 = [gini_values_3; gini_matrix_3(i,j)];
    end
end

% Create figure with subplots
figure('Position', [100, 100, 1200, 500], 'Color', 'white');

% Subplot 1: 3D scatter plot
subplot(1, 2, 1);
scatter3(gini_values_1, gini_values_2, gini_values_3, 100, gini_values_1, 'filled');
colormap(parula);  % Changed to parula for a more academic look
cb = colorbar;
ylabel(cb, 'Gini Coefficient', 'FontName', 'Times New Roman', 'FontSize', label_fontsize-2);
xlabel('Time Period 1', 'FontName', 'Times New Roman', 'FontSize', label_fontsize);
ylabel('Time Period 2', 'FontName', 'Times New Roman', 'FontSize', label_fontsize);
zlabel('Time Period 3', 'FontName', 'Times New Roman', 'FontSize', label_fontsize);
title('\bf{Temporal Evolution of Gini Coefficients}', 'FontName', 'Times New Roman', 'FontSize', title_fontsize);
grid on;
box on;
view(45, 30);
set(gca, 'FontName', 'Times New Roman', 'FontSize', label_fontsize-2);

% Subplot 2: Heatmap
subplot(1, 2, 2);
imagesc(sigma_range, mu_range, gini_matrix_1);
colormap(parula);
cb = colorbar;
ylabel(cb, 'Gini Coefficient', 'FontName', 'Times New Roman', 'FontSize', label_fontsize-2);
xlabel('\sigma Values', 'FontName', 'Times New Roman', 'FontSize', label_fontsize);
ylabel('\mu Values', 'FontName', 'Times New Roman', 'FontSize', label_fontsize);
title('\bf{Gini Coefficient Distribution (t=1)}', 'FontName', 'Times New Roman', 'FontSize', title_fontsize);
set(gca, 'YDir', 'normal', 'FontName', 'Times New Roman', 'FontSize', label_fontsize-2);
box on;

% Adjust overall figure appearance
set(gcf, 'PaperUnits', 'inches');
set(gcf, 'PaperPosition', [0 0 12 5]);

% Save the figure in high resolution
print('sensitivity_analysis_gini', '-dpng', '-r300');

% Gini coefficient calculation function
function gini = calculateGiniCoefficient(income)
    sorted_income = sort(income);
    n = length(income);
    cumulative_income = cumsum(sorted_income);
    total_income = sum(sorted_income);
    
    % Calculate Gini coefficient using the area under Lorenz curve method
    cumulative_population = (1:n) / n;
    cumulative_income_share = cumulative_income / total_income;
    gini = 1 - 2 * trapz(cumulative_population, cumulative_income_share);
end