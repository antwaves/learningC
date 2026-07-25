#include "planet.h"
#include <stdint.h>
#include <math.h>


struct Planet planet_init(int x, int y, float vx, float vy, uint32_t mass) {
    struct Planet p = {
        .circle = {
            .radius = 2 * pow((float)mass, 0.3), 
            .x = (float)x, .y = (float)y, 
            .color = {255, 156, 222}
        },
        .start_pos = {x, y},
        .velocity = {vx, vy},
        .acceleration = {0.0f, 0.0f},
        .mass = mass
    };
    // update radius to allow it to be more precise
    return p;
}


void update_planet_acceleration(struct Planet* self, struct Planet* planets, int planet_count) {
    self->acceleration[0] = 0;
    self->acceleration[1] = 0;

    for (int i = 0 ; i < planet_count; i++) {
        if (self != &planets[i]) {
            struct Circle other_circ = planets[i].circle;
            float dx = other_circ.x - self->circle.x;
            float dy = other_circ.y - self->circle.y;
            double r = sqrt(dx * dx + dy * dy);

            //printf("%f", other_circ.x);
            self->acceleration[0] += dx * planets[i].mass / pow(r, 3);
            self->acceleration[1] += dy * planets[i].mass / pow(r, 3);
        }
    }

    //printf("AAAAAAAAAAAAAAAAAAAAAAAaa %f AAAAAAAAa", self->acceleration[0]);
}


void move(struct Planet* self) {
    self->velocity[0] += self->acceleration[0];
    self->velocity[1] += self->acceleration[1];
    self->circle.x += self->velocity[0];
    self->circle.y += self->velocity[1];
}
