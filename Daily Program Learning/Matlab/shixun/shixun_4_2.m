% 生成周期矩形波形信号
Fs = 1000;          % 采样率（每秒采样点数）
T = 1;              % 信号周期（秒）
t = 0:1/Fs:T-1/Fs;  % 时间向量
f = 5;              % 信号频率（Hz）
x = square(2*pi*f*t); % 周期矩形波形信号

% 计算FFT并绘制单边频谱
L = length(x);              % 信号长度
Y = fft(x)/L;               % FFT并归一化
frequencies = Fs*(0:(L/2))/L; % 计算频率向量

% 绘制单边频谱的幅度
figure;
subplot(2,1,1);
plot(frequencies, 2*abs(Y(1:L/2+1)));
title('单边频谱的幅度');
xlabel('频率 (Hz)');
ylabel('幅度');

% 绘制单边频谱的相位
subplot(2,1,2);
plot(frequencies, angle(Y(1:L/2+1)));
title('单边频谱的相位');
xlabel('频率 (Hz)');
ylabel('相位 (弧度)');

% 绘制双边频谱
figure;
subplot(2,1,1);
plot(-Fs/2:Fs/L:Fs/2-Fs/L, fftshift(abs(Y)));
title('双边频谱的幅度');
xlabel('频率 (Hz)');
ylabel('幅度');

subplot(2,1,2);
plot(-Fs/2:Fs/L:Fs/2-Fs/L, fftshift(angle(Y)));
title('双边频谱的相位');
xlabel('频率 (Hz)');
ylabel('相位 (弧度)');




