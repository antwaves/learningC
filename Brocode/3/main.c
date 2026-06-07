#include <stdio.h>
#include <math.h>

int main() {
    int x = -9;

    //x = sqrt(x);
    //x = pow(x, 2);
    //x = abs(x);

    float y = 3.14;
    //y = round(7);
    //y  = ciel(y);
    //y = floor(y);
    //y = log(y);

    double z = 45;  // in radians
    //z = sin(z); 
    //z = cos(z);
    z = tan(x);

    printf("%d\n", x);
    printf("%f\n", y);
    printf("%f\n", z);

    return 0;
}
