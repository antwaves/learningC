#include <stdio.h>

int main() {
    int choice = 0;
    float pounds = 0;
    float kilograms = 0;

    printf("lb to kg or kg to lb? (0/1): ");
    scanf_s("%d", &choice);

    if (choice == 0) {
        printf("Amount of pounds: ");
        scanf_s("%f", &pounds);

        kilograms = pounds / 2.0462;
        printf("%.2f pounds is equal to %.2f kilograms", pounds, kilograms);

    } 
    else if (choice == 1) {
        printf("Amount of kilograms: ");
        scanf_s("%f", &kilograms);  

        pounds = kilograms * 2.0462;
        printf("%.2f kilograms is equal to %.2f pounds", kilograms, pounds);
    }

    else {
        printf("fuck off");
    }
    
    //1:37:00
    return 0;
}
