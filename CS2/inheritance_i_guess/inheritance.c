#include <stdio.h>
#include <stdbool.h>
#include "../../random/random.c"


typedef struct {
    char* name;
    int health;
    int power;
} Character;


typedef struct {
    int armor;
    Character stats;
    char* show_special;
} Warrior;


typedef struct {
    int mana;
    Character stats;
} Wizard;


void take_damage(Character* c, int damage) {
    c->health -= damage;
    printf("%s takes %d damage", c->name, damage);
}


void attack(Character* self, Character* other) {
    int damage = self->power * randint(1, 2);
    printf("%s attacks %s", self->name, other->name);
    take_damage(other, damage);
}


bool alive(Character* c) {
    return c->health > 0;
}


void defend(Warrior* w) {
    int defense = w->armor * randint(1, 2);
    printf("Warrior defends for %d", defense);
    w->stats.health += defense;
}


void show_special(char* c) {
    printf("%s", c);
}


Warrior wa_init(char* name, int health, int power, int armor) {
    Warrior wa  = {armor, {name, health, power}, "2 - Defend (restore HP)"};
    return wa;
}


Wizard wi_int(char* name, int health, int power, int mana) {
    Wizard wi = {mana, {name, health, power}};
    return wi;
}


int main() {
    Warrior w = wa_init("test", 10, 20, 5);

    take_damage(&w.stats, 1);
    defend(&w);
    show_special(w.show_special);

}