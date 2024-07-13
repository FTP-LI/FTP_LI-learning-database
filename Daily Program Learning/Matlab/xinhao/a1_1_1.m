% 定义参数
A = 1; % 幅度
f = 10; % 频率（Hz）
t = 0:0.001:1; % 时间向量
tau = 0.1; % 延时时间
dt = t(2) - t(1); % 采样间隔

% 定义原始信号
x = A * sin(2 * pi * f * t);

% 尺度变换
k = 2; % 尺度变换因子
x_scaled = A * sin(2 * pi * f * k * t);

% 反褶
x_reflected = fliplr(x);

% 延时
x_delayed = [zeros(1, round(tau/dt)), x(1:end-round(tau/dt))];

% 绘制波形图
figure;
subplot(4,1,1);
plot(t, x);
title('原信号');
xlabel('t(s)');
ylabel('f(t)');
ylim([-1.5 1.5]); % 设置y轴范围，确保信号完整显示

subplot(4,1,2);
plot(t, x_scaled);
title('尺度变换');
xlabel('t(s)');
ylabel('f(t)');
ylim([-1.5 1.5]); % 设置y轴范围，确保信号完整显示

subplot(4,1,3);
plot(t, x_reflected);
title('反褶');
xlabel('t(s)');
ylabel('f(t)');
ylim([-1.5 1.5]); % 设置y轴范围，确保信号完整显示

subplot(4,1,4);
plot(t, x_delayed);
title('延迟');
xlabel('t(s)');
ylabel('f(t)');
ylim([-1.5 1.5]); % 设置y轴范围，确保信号完整显示
