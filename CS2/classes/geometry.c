#include <stdio.h>
#include <math.h>


typedef struct {
    unsigned int width;
    unsigned int height;
    char repr[64];
} Rectangle;


char* r_repr(Rectangle* self) {
    snprintf(self->repr, 64, "Rectangle(%d, %d)", self->width, self->height);
    return self->repr;
}

int r_area(Rectangle* self) {
    return self->width * self->height;
}


int r_perimeter(Rectangle* self) {
    return 2 * (self->width + self->height);
}


typedef struct {
    unsigned int length;
    char repr[64];
} Square;


char* s_repr(Square* self) {
    snprintf(self->repr, 64, "Square(%d)", self->length);
    return self->repr;
}

int s_area(Square* self) {
    return self->length * self->length;
}


int s_perimeter(Square* self) {
    return 4 * self->length;
}


typedef struct {
    int x;
    int y;
    char repr[64];
} Point;


char* p_repr(Point* self) {
    snprintf(self->repr, 64, "Point(%d, %d)", self->x, self->y);
    return self->repr;
}


double distance_to(Point* self, Point* other) {
    return sqrt(pow((self->x - other->x), 2) + pow((self->y - other->y), 2));
}


typedef struct {
    Point p1;
    Point p2;
    char repr[128];
} Line;


char* l_repr(Line* self) {
    snprintf(self->repr, 64, "Line at %s and %s", p_repr(&self->p1), p_repr(&self->p2));
    return self->repr;
}


double length(Line* self) {
    return distance_to(&self->p1, &self->p2);
}


double slope(Line* self) {
    return ((double)(self->p1.y - self->p2.y) / (double)(self->p1.x - self->p2.x));
}


int main() {
    Rectangle r = {3, 4};
    printf("%d\n", r.width);
    printf("%d\n", r.height);
    printf("%s\n", r_repr(&r));
    printf("%d\n", r_area(&r));
    printf("%d\n", r_perimeter(&r));


    Square s = {10};
    printf("\n%d\n", s.length);
    printf("%s\n", s_repr(&s));
    printf("%d\n", s_area(&s));
    printf("%d\n", s_perimeter(&s));

    Point p = {1, 2};   
    printf("\n%d\n", p.x);
    printf("%d\n", p.y);

    Point o = {0, 0};
    printf("%f\n", distance_to(&p, &o));

    Line l = {{0, 0}, {3, 4}};
    printf("\n%s\n", l_repr(&l));
    printf("%s\n", p_repr(&l.p1));
    printf("%s\n", p_repr(&l.p2));
    printf("%f\n", length(&l));
    printf("%f\n", slope(&l));
}