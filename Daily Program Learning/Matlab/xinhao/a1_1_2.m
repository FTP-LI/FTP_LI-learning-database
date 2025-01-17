% 参数定义
A = 1; % 脉冲幅度
A_A = 2; %幅度更改
f0 = 1; % 正弦波频率
T = 5; % 脉冲宽度
Fs = 100; % 采样频率
t = -2*T:1/Fs:2*T; % 时间向量

% 生成信号
x = A * sin(2*pi*f0*t) .* rectpuls(t/T, T);%正弦波信号

% 计算频谱
X = fft(x);%频谱
f = (-Fs/2:Fs/length(t):Fs/2-Fs/length(t));
X_mag = abs(X)/length(t);

%幅度更改
X_A = A_A * sin(2*pi*f0*t) .* rectpuls(t/T, T);
X_FP = fft(X_A);%频谱
X_mag_P = abs(X_FP)/length(t);

%宽度更改
x_K = A * sin(4*pi*f0*t) .* rectpuls(t/T, T);
X_LP = fft(x_K);%频谱
X_mag_K = abs(X_LP)/length(t);

%宽度更改
x_K_A = A * sin(1*pi*f0*t) .* rectpuls(t/T, T);
X_LP_A = fft(x_K_A);%频谱
X_mag_K_A = abs(X_LP_A)/length(t);

% 绘制时域信号
figure;
subplot(5,1,1);
plot(t, x);
title('时域信号');
xlabel('t(s)');
ylabel('f(t)');

% 绘制频谱图
subplot(5,1,2);
plot(f, X_mag);
title('频域');
xlabel('频率 (Hz)');
ylabel('级数');

%幅度更改
subplot(5,1,3);
plot(f, X_mag_P);
title('幅度增大');
xlabel('频率 (Hz)');
ylabel('级数');

%宽度更改
subplot(5,1,4);
plot(f, X_mag_K);
title('频率增大');
xlabel('频率 (Hz)');
ylabel('级数');

%宽度更改
subplot(5,1,5);
plot(f, X_mag_K_A);
title('频率减小');
xlabel('频率 (Hz)');
ylabel('级数');