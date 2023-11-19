
% 定义已知常量部分
format long g;
V_cars = [120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200];
distances = [1000,1062.5,1125,1187.5,1250,1312.5,1375,1437.5,1500,1562.5,1625,1687.5,1750,1812.5,1875,1937.5,2000];
rou_man = 1097.7;
rou_air = 1.225;
C = 101218;
P0 = 101325;
g = 9.8;
m=65;
high = 1000;
low = 1200;

% 创建四维数组
F = zeros(length(V_cars), length(distances));
P = zeros(length(V_cars), length(distances));

h_bar = -0.04548 * (power(m,2))+12.06765*m+1053.5580+500;

V_volume_bar = 0.004*m/(2*rou_man) + 0.0046;

% 计算F和P的值
for i = 1:length(V_cars)
    for j = 1:length(distances)
        V_car = V_cars(i);
        d = distances(j);
        
        temp1 = 0.072 / (power(V_car,4));
        V_air = (power(1.2319,temp1)) * (0.4575 * (power((d / 1000),2)) - 3.5496 * (d / 1000) + 9.1545);
        P(i, j) = C - 0.5 * rou_air * (power(V_air,2)) - rou_air * g * (h_bar / 2000);
        temp2 = sqrt(V_volume_bar * (h_bar / 1000));
        temp3 = sqrt(4 / pi);
        S_bar = temp2 * 0.5 * (temp3 + 1);
        F(i, j) =70-(P0 - P(i, j)) *S_bar;
        fprintf('h_bar为: %s\n', - 0.5 * rou_air * (power(V_air,2)) - rou_air * g * (h_bar / 2000));
    end
end

% 绘制四维图像

[X, Y] = meshgrid(V_cars, distances);
figure;
%subplot(2, 2, 1);
surf(X, Y, F);
title('作用力、速度关系');
xlabel('列车速度');
ylabel('距离');
zlabel('作用力');
colorbar;

figure;
%subplot(2, 2, 2);
surf(X, Y, P);
title('压强、速度、距离四维图');
xlabel('列车速度');
ylabel('距离');
zlabel('压强');
colorbar;