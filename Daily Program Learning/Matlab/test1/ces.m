% 定义参数t的范围
t = linspace(-10, 10, 100); % 从-10到10，生成100个点

% 计算交线上的点
x = t;
y = 2*t - 2;
z = t;

% 计算交线上的点
i = t;
j = 2*t + 12;
k = (3/2)*t + 2;

% 绘制直线
figure;
plot3(x, y,z, 'r-', 'LineWidth', 2);
hold on;
plot3(i, j, k, 'b--', 'LineWidth', 2);
hold off;
xlabel('X-axis');
ylabel('Y-axis');
zlabel('Z-axis');
title('3D Plot of the Intersection Line of Two Planes');
grid on;