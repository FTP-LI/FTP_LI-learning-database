% 生成一个 m 行 n 列的随机矩阵
m = 4; % 可自行调整 m 和 n 的值
n = 4;
A = rand(m, n);

% 计算矩阵的元素最大值、最小值、均值和方差
max_value = max(A(:));
min_value = min(A(:));
mean_value = mean(A(:));
variance = var(A(:));

% 计算矩阵的转置
A_transpose = A';

% 计算矩阵的逆（如果可逆的话）
if det(A) ~= 0
    A_inv = inv(A);
else
    disp('矩阵不可逆');
end

% 计算矩阵的行列式和特征值
determinant = det(A);
eigenvalues = eig(A);

% 显示结果
disp('随机生成的矩阵 A:');
disp(A);

disp('元素最大值:');
disp(max_value);

disp('元素最小值:');
disp(min_value);

disp('均值:');
disp(mean_value);

disp('方差:');
disp(variance);

disp('转置矩阵 A_transpose:');
disp(A_transpose);

if det(A) ~= 0
    disp('逆矩阵 A_inv:');
    disp(A_inv);
else
    disp('矩阵 A 不可逆，无法计算逆矩阵');
end

disp('行列式 determinant:');
disp(determinant);

disp('特征值 eigenvalues:');
disp(eigenvalues);
