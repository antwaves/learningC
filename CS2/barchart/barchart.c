#include <stdio.h>

int main() {
    int nums[] = {1, 4, 2, 7, 3, 1};

    int height = nums[0];
    for (int i = 0; i < sizeof(nums) / sizeof(int); i++) {
        if (nums[i] > height) {
            height = nums[i];
        }
    }

    for (int i = height; i > 0; i--) {
        for (int j = 0; j < height; j++) {
            if (nums[j] >= i) {
                printf("X");
            }
            else {
                printf(" ");
            }

        }

        printf("\n");
    }
}