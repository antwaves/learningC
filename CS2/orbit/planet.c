#include "planet.h"
#include <stdint.h>
#include <math.h>


struct Planet planet_init(int x, int y, float vx, float vy, uint32_t mass) {
    struct Planet p = {
        .circle = {
            .radius = 2 * pow(mass, 0.3), 
            . x = x, .y = y, 
            .color = {255, 156, 222}
        },
        .velocity = {vx, vy},
        .acceleration = {0.0f, 0.0f},
        .mass = mass
    };

    return p;
}


void update_planet_acceleration(struct Planet* self, struct Planet* planets, int planet_count) {
    self->acceleration[0] = 0;
    self->acceleration[1] = 0;

    for (int i = 0 ; i < planet_count; i++) {
        if (self != &planets[planet_count]) {
            struct Circle other_circ = planets[planet_count].circle;
            int dx = other_circ.x - self->circle.x;
            int dy = other_circ.y - self->circle.y;
            double r = sqrt(dx * dx + dy * dy);
            self->acceleration[0] = dx * planets[planet_count].mass / pow(r, 3);
            self->acceleration[1] = dy * planets[planet_count].mass / pow(r, 3);
        }
    }
}


void move(struct Planet* self) {
    self->velocity[0] += self->acceleration[0];
    self->velocity[1] += self->acceleration[1];
    self->circle.x += self->velocity[0];
    self->circle.y += self->velocity[1];
}
