% 参数设置
A = 1;        % 矩形波振幅
T = 2*pi;     % 矩形波周期
num_harmonics = 10;  % 考虑的谐波次数

% 时间变量
t = linspace(0, T, 1000);

% 原始矩形波信号定义
rect_wave = A * square(t);

% 傅里叶级数计算
N = num_harmonics;  % 考虑的最高谐波次数
fs = N / T;  % 计算采样频率
Y = fft(rect_wave);  % 进行傅里叶变换
L = length(rect_wave);  % 信号的长度
P2 = abs(Y/L);  % 计算双边频谱
P1 = P2(1:L/2+1);  f = fs*(0:(L/2))/L;

% 绘制原始矩形波形
figure;
plot(t, rect_wave, 'b-', 'LineWidth', 1.5);
title('原始周期矩形波信号');
xlabel('时间 t');
ylabel('幅度');
ylim([-1.2 1.2]);
grid on;

% 计算傅里叶级数系数
a0 = P1(1);
ak = 2*P1(2:end);  bk = ak;

% 合成波形初始化
synthesized_wave = a0 * ones(size(t))/2;

% 合成波形综合
for k = 1:num_harmonics
    synthesized_wave = synthesized_wave + ak(k) * cos(k * t) + bk(k) * sin(k * t);
end

% 绘制合成波形与原始矩形波形对比
figure;
plot(t, rect_wave, 'b-', 'LineWidth', 1.5);  % 原始矩形波形
hold on;
plot(t, synthesized_wave, 'r--', 'LineWidth', 1.5);  % 合成波形
hold off;
title(sprintf('合成波形与原始矩形波形对比 (包含 %d 个谐波)', num_harmonics));
xlabel('时间 t');
ylabel('幅度');
legend('原始矩形波形', '合成波形');
ylim([-1.2 1.2]);
grid on;

