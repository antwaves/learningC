#include <stdio.h>
#include <string.h>

int main() {
    char item[30] = "";
    int amount = 0;
    float price = 0.0f;
    char currency = '$';


    printf("Enter the item name: ");
    fgets(item, sizeof(item), stdin);
    item[strlen(item) - 1] = '\0';
    
    printf("How much does it cost?: ");
    scanf_s(" %f", &price);

    printf("How many are you buying?: ");
    scanf_s(" %d", &amount);

    float final = amount * price;

    if (amount == 1){
        printf("Buying your %s will cost %c%.2f", item, currency, final);
    } else {
        printf("Buying your %ss will cost %c%.2f", item, currency, final);
    }

    

    return 0;
}
