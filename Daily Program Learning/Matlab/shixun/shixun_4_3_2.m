% 参数设定
Fs = 1000;       % 采样率
T = 1;           % 信号周期（秒）
Width = 0.5;     % 三角波脉冲宽度（0到1之间）

% 生成三角脉冲信号
t = 0:1/Fs:T-1/Fs; % 时间向量
f = 5;             % 三角波频率（Hz）
x = sawtooth(2*pi*f*t, Width); % 生成三角脉冲信号

% 计算FFT并归一化
L = length(x);              % 信号长度
Y = fft(x)/L;               % FFT并归一化
frequencies = Fs*(0:(L/2))/L; % 计算频率向量

% 绘制单边幅度频谱
figure;
subplot(2,1,1);
plot(frequencies, 2*abs(Y(1:L/2+1)));
title('周期三角脉冲信号的单边幅度频谱');
xlabel('频率 (Hz)');
ylabel('幅度');

% 绘制双边幅度频谱
subplot(2,1,2);
plot(-Fs/2:Fs/L:Fs/2-Fs/L, fftshift(abs(Y)));
title('周期三角脉冲信号的双边幅度频谱');
xlabel('频率 (Hz)');
ylabel('幅度');
