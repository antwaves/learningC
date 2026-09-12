#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int elms = 0;
    printf("Enter the num of prices: ");
    scanf_s(" %d", &elms);

    float *prices = malloc(sizeof(float) * elms);

    if (prices == NULL) {
        printf("AAAAAAaa");
        return 1;
    }

    for (int i = 0; i < elms; i++) {
        printf("Enter price #%d: ", i + 1);
        scanf_s("%f", &prices[i]);
    }

    int new_prices = 0;
    printf("How many elms?: ");
    scanf_s(" %d", &new_prices);

    float *temp = realloc(prices, sizeof(float) * new_prices); // copies and frees old memory
   
    if (temp == NULL) {
        printf("AAAAAA");
    } else {
        prices = temp;
        temp = NULL;
        
        for (int i = elms; i < new_prices; i++) {
            printf("Enter price #%d: ", i + 1);
            scanf_s("%f", &prices[i]);
        }
        
        for (int i = 0; i <  new_prices; i++) {
            printf("$%0.2f ", prices[i]);
        }
    }
   

    free(prices);
    prices = NULL;

    return 0;

    //6:44:04
}
