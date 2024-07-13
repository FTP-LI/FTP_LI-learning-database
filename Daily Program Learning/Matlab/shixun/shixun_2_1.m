% 参数设置
A_sin = 2;       % 正弦波振幅
f_sin = 2;       % 正弦波频率（周期为 1/f_sin）
phi_sin = pi/2;  % 正弦波相位偏移

% 时间变量
t = linspace(0, 2, 1000);  % 从0到2的时间范围，取1000个点

% 正弦波函数
sin_wave = A_sin * sin(2 * pi * f_sin * t + phi_sin);

% 绘图
figure;
plot(t, sin_wave, 'b', 'LineWidth', 1.5);
title('正弦波函数');
xlabel('时间 t');
ylabel('幅度');
grid on;
