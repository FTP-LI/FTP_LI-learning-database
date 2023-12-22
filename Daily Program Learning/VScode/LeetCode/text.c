#include <stdio.h>
#include <math.h>

int main() {
    float base = 1000.0;//本金
    float rate = 0.025;//利率
    
    // 第一种方式：一次存5年期
    float m1 = base * pow(1 + rate, 5);
    printf("一次存5年期的存款利息为：%f\n", m1 - base);

    // 第二种方式：先存2年期，再存3年期
    float m2 = base * pow(1 + rate, 2);
    float m2_total = m2 * pow(1 + rate, 3);
    printf("先存2年期，再存3年期的存款利息为：%f\n", m2_total - base);

    // 第三种方式：先存3年期，再存2年期
    float m3 = base * pow(1 + rate, 3);
    float m3_total = m3 * pow(1 + rate, 2);
    printf("先存3年期，再存2年期的存款利息为：%f\n", m3_total - base);

    // 第四种方式：连续存1年期，共存5次
    float m4 = base;
    for (int i = 0; i < 5; i++) {
        m4 = m4 * pow(1 + rate, 1);
    }
    printf("连续存1年期，共存5次的存款利息为：%f\n", m4 - base);

    // 第五种方式：活期存款，每季度结算一次
    float inter = 0.0;
    for (int i = 0; i < 5; i++) {
        inter += base * rate * 0.25;
        base += inter;
    }
    printf("活期存款每季度结算一次的存款利息为：%f\n", inter);

    return 0;
}