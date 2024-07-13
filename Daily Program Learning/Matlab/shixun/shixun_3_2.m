% 绘图
figure;

% 函数1：sin函数，红色实线
subplot(2, 2, 1);
plot(x, y1, 'r-', 'LineWidth', 1.5);
title('函数1: sin(x)');
xlabel('x');
ylabel('y');
grid on;
legend('sin(x)');

% 函数2：cos函数，蓝色实线
subplot(2, 2, 2);
plot(x, y2, 'b-', 'LineWidth', 1.5);
title('函数2: cos(x)');
xlabel('x');
ylabel('y');
grid on;
legend('cos(x)');

% 函数3：sin^2函数，绿色实线
subplot(2, 2, 3);
plot(x, y3, 'g-', 'LineWidth', 1.5);
title('函数3: sin^2(x)');
xlabel('x');
ylabel('y');
grid on;
legend('sin^2(x)');

% 函数4：指数函数，黑色实线
subplot(2, 2, 4);
plot(x, y4, 'k-', 'LineWidth', 1.5);
title('函数4: exp(-x)');
xlabel('x');
ylabel('y');
grid on;
legend('exp(-x)');


