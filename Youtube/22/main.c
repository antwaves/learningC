#include <stdio.h>
#include <stdbool.h>

struct Car {
    char model[25];
    int year;
    int price;
};

void p_car(struct Car c);

int main() {
    struct Car cars[] = {{"Mustang", 2025, 32000},  
                        {"Corvette", 2026, 68000},
                        {"Toyota", 2024, 29000}};
    
    for (int i = 0; i < sizeof(cars) / sizeof(struct Car); i++) {
        p_car(cars[i]);
    }

}


void p_car(struct Car c) {
    printf("%s %d $%d\n", c.model, c.year, c.price);
}
