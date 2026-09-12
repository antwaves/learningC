#include <stdio.h>
#include <math.h>

int main() {
    double radius = 0.0;
    double area = 0.0;
    double surface_area = 0.0;
    double volume = 0.0;

    const double pi = 3.145;

    printf("Enter the radius: ");
    scanf_s("%lf", &radius);

    double radius_squared = radius * radius;
    double four_thirds = 4.0 / 3;
    area = pi * radius_squared;
    surface_area = 4 * pi * radius_squared;
    volume = four_thirds * pi * pow(radius, 3);

    printf("%.3lf\n", area);
    printf("%.3lf\n", surface_area);
    printf("%.3lf\n", volume);

    //1:37:00
    return 0;
}
