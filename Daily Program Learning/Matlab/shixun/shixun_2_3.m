% 参数设置
A_tri = 0.5;      % 三角波振幅
f_tri = 4;        % 三角波频率（周期为 1/f_tri）
phi_tri = pi/8;    % 矩形波相位偏移

% 三角波函数
tri_wave = A_tri * sawtooth(2 * pi * f_tri * t+phi_tri, 0.5);

% 绘图
figure;
plot(t, tri_wave, 'g', 'LineWidth', 1.5);
title('三角波函数');
xlabel('时间 t');
ylabel('幅度');
ylim([-1.1, 1.1]);  % 调整y轴范围以显示完整的三角波形
grid on;
