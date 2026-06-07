#include <stdio.h>
#include <stdbool.h>

int main() {
    char op = '\0';
    double num_one = 0.0;
    double num_two = 0.0;
    double result = 0.0;

    printf("Enter num one: ");
    scanf_s("%lf", &num_one);

    printf("Enter the operator: ");
    scanf_s(" %c", &op);

    printf("Enter num two: ");
    scanf_s(" %lf", &num_two);

    switch (op) {
        case '+':
            result = num_one + num_two;
            break;
        case '-':
            result = num_one - num_two;
            break;
        case '*':
            result = num_one * num_two;
            break;
        case '/':
            if (num_two == 0) {
                printf("Divide by zero error\n");
                op = '\0';
                break;
            }
            result = num_one / num_two;
            break;
        default:
            printf("transition.,., now.,.,.");
    }

    if (op != '\0'){
        printf("Your result is %lf", result);
    }
    
    return 0;
}
