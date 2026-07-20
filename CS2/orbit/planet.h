#include <cglm/cglm.h>
#include <stdint.h>
#include "circle.h"

#ifndef PLANET
#define PLANET
struct Planet {
    struct Circle circle;
    vec2 start_pos;
    vec2 velocity;
    vec2 acceleration;
    uint32_t mass;
};

struct PlanetArr {
    struct Planet* planets;
    uint32_t num_planets;
};

struct Planet planet_init(int x, int y, float vx, float vy, uint32_t mass);
#endif
