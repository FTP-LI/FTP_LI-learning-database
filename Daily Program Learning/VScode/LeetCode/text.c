#include <stdio.h>
#include <math.h>

int main() {
    float principal = 1000.0;//本金
    float interestRate = 0.025;//利率
    
    // 第一种方式：一次存5年期
    float m1 = principal * pow(1 + interestRate, 5);
    printf("一次存5年期的存款利息为：%f\n", m1 - principal);

    // 第二种方式：先存2年期，再存3年期
    float m2 = principal * pow(1 + interestRate, 2);
    float m2_total = m2 * pow(1 + interestRate, 3);
    printf("先存2年期，再存3年期的存款利息为：%f\n", m2_total - principal);

    // 第三种方式：先存3年期，再存2年期
    float m3 = principal * pow(1 + interestRate, 3);
    float m3_total = m3 * pow(1 + interestRate, 2);
    printf("先存3年期，再存2年期的存款利息为：%f\n", m3_total - principal);

    // 第四种方式：连续存1年期，共存5次
    float m4 = principal;
    for (int i = 0; i < 5; i++) {
        m4 = m4 * pow(1 + interestRate, 1);
    }
    printf("连续存1年期，共存5次的存款利息为：%f\n", m4 - principal);

    // 第五种方式：活期存款，每季度结算一次
    float quarterlyInterest = 0.0;
    for (int i = 0; i < 5; i++) {
        quarterlyInterest += principal * interestRate * 0.25;
        principal += quarterlyInterest;
    }
    printf("活期存款每季度结算一次的存款利息为：%f\n", quarterlyInterest);

    return 0;
}