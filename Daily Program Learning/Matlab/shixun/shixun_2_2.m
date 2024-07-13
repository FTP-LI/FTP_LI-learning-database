% 参数设置
A_rect = 3;         % 矩形波幅度    
f_rect = 1;         % 矩形波频率（周期为 1/f_rect）
phi_rect = pi/4;    % 矩形波相位偏移

% 矩形波函数
rect_wave = A_rect * sign(sin(2 * pi * f_rect * t + phi_rect));

% 绘图
figure;
plot(t, rect_wave, 'r', 'LineWidth', 1.5);
title('矩形波函数');
xlabel('时间 t');
ylabel('幅度');
ylim([-1.5, 1.5]);  % 调整y轴范围以显示完整的矩形波形
grid on;
