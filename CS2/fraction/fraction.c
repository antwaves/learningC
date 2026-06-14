#include <stdio.h>

typedef struct {
    int num;
    int den;
} Fraction;

enum input_type {
    FRACTION, INT
};


Fraction i_fraction(int num, int den) {
    int a = num; 
    int b = den;

    while (b != 0) {
        int tmp = b;
        b = a % b;
        a = tmp;
    }
    Fraction f = {num / a, den / a};

    if (f.num > 0 && f.den < 0) {
        f.num *= -1;
        f.den *= -1;
    }
    return f;
}


char* repr(Fraction* self, char* buffer) {
    snprintf(buffer, 128, "%d/%d", self->num, self->den);
    return buffer;
}


Fraction inverse(Fraction self) {
    Fraction f = {self.den, self.num};
    return f;
}


Fraction mul(void* self, enum input_type self_type, void* other, enum input_type other_type) {
    if (self_type == FRACTION) {
        if (other_type == FRACTION) {
            Fraction f = {((Fraction*)self)->num * ((Fraction*)other)->num, ((Fraction*)self)->den * ((Fraction*)other)->den};
            return i_fraction(f.num, f.den);
        }

        Fraction f = {((Fraction*)self)->num * *((int*)other), ((Fraction*)self)->den};
        return i_fraction(f.num, f.den);
    
    }   
    if (other_type == FRACTION) {
        return mul(other, other_type, self, self_type);
    }

    Fraction f = {*((int*)self) * *((int*)other), 1};
    return f;
}


Fraction div(void* self, enum input_type self_type, void* other, enum input_type other_type) {    
    if (self_type == FRACTION) {
        if (other_type == FRACTION) {
            Fraction f = {((Fraction*)other)->den, ((Fraction*)other)->num};
            return mul(self, self_type, &f, other_type);
        }


        Fraction f = {1, *((int*)other)};
        return mul(self, self_type, &f, FRACTION);
    
    }   

    if (other_type == FRACTION) {
        Fraction f = {1, *((int*)self)};
        return mul(&f, FRACTION, other, other_type);
    }

    Fraction f = {*((int*)self) / *((int*)other), 1};
    return f;
}


Fraction sub(void* self, enum input_type self_type, void* other, enum input_type other_type) {
    if (self_type == FRACTION) {
        if (other_type == FRACTION) {
            Fraction f1 = *((Fraction*)self);
            Fraction f2 = *((Fraction*)other);

    
            f1 = mul(&f1, FRACTION, &f2.den, INT);
            f2 = mul(&f2, FRACTION, &f1.den, INT);

            Fraction f = {f1.num - f2.num, f1.den * f2.den};
            return i_fraction(f.num, f.den);
        }

        Fraction f = {*((int*)other), 1};
        return sub(self, self_type, &f, FRACTION);
    
    }   

    if (other_type == FRACTION) {
        return sub(other, other_type, self, self_type);
    }

    Fraction f = {*((int*)self) - *((int*)other), 1};
    return f;
}


Fraction add(void* self, enum input_type self_type, void* other, enum input_type other_type) {
    if (self_type == FRACTION) {
        if (other_type == FRACTION) {
            Fraction f1 = *((Fraction*)self);
            Fraction f2 = *((Fraction*)other);
    
            f1 = mul(&f1, FRACTION, &f2.den, INT);
            f2 = mul(&f2, FRACTION, &f1.den, INT);

            Fraction f = {f1.num + f2.num, f1.den * f2.den};
            return i_fraction(f.num, f.den);
        }

        Fraction f = {*((int*)other), 1};
        return add(self, self_type, &f, FRACTION);
    
    }   

    if (other_type == FRACTION) {
        return add(other, other_type, self, self_type);
    }

    Fraction f = {*((int*)self) + *((int*)other), 1};
    return f;
}


int main() {
    Fraction t = i_fraction(5, 20);

    char buffer[128];
    Fraction f = inverse(t);
    printf("Test methods\na = %s\na.inverse() =", repr(&t, buffer));
    printf(" %s\n", repr(&f, buffer));

    f = i_fraction(2, 3);
    int x = 6;
    Fraction r = mul(&t, FRACTION, &f, FRACTION);
    Fraction r2 = mul(&t, FRACTION, &x, INT);
    printf("\nTest multiplication:\na * 2/3 = %s\na * 6 =", repr(&r, buffer));
    printf(" %s\n", repr(&r2, buffer));

    r = div(&t, FRACTION, &f, FRACTION);
    r2 = div(&t, FRACTION, &x, INT);
    printf("\nTest division:\na / 2/3 = %s\na / 6 =", repr(&r, buffer));
    printf(" %s\n", repr(&r2, buffer));

    r = add(&t, FRACTION, &f, FRACTION);
    r2 = add(&t, FRACTION, &x, INT);
    printf("\nTest addition:\na + 2/3 = %s\na + 6 =", repr(&r, buffer));
    printf(" %s\n", repr(&r2, buffer));

    r = sub(&t, FRACTION, &f, FRACTION);
    r2 = sub(&t, FRACTION, &x, INT);
    printf("\nTest subtraction:\na - 2/3 = %s\na - 6 =", repr(&r, buffer));
    printf(" %s\n", repr(&r2, buffer));
}