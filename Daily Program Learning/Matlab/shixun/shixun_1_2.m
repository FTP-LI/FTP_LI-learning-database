% 生成两个 m 行 n 列的随机矩阵
m = 4; % 可自行调整 m 和 n 的值
n = 4;
B = rand(m, n);
C = rand(m, n);

% 计算两个矩阵的乘法
product = B * C;

% 计算两个矩阵的右除和左除
right_div = B / C;
left_div = B \ C;

% 计算矩阵的幂运算
power_result = B ^ 2; % 例如计算 B 的平方

% 显示结果
disp('矩阵 B：');
disp(B);
disp('矩阵 C：');
disp(C);
disp('乘法结果 B * C：');
disp(product);
disp('右除结果 B / C：');
disp(right_div);
disp('左除结果 B \\ C：');
disp(left_div);
disp('幂运算结果 B ^ 2：');
disp(power_result);
