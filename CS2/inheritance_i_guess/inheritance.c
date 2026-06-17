#include <stdio.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <windows.h>

#include "../../random/random.c"


typedef struct {
    char* name;
    int health;
    int power;
} Character;


struct Warrior {
    int armor;
    Character stats;
    char* show_special;
    void (*repr)(void*, int);
    void (*special)(void*);
};


struct Wizard{
    int mana;
    Character stats;
    char* show_special;
    void (*repr)(void*, int);
    void (*special)(void*);
};


void take_damage(Character* c, int damage) {
    c->health -= damage;
    printf("%s takes %d damage\n", c->name, damage);
}


void attack(Character* self, Character* other) {
    int damage = self->power * randint(1, 3);
    printf("%s attacks %s\n", self->name, other->name);
    take_damage(other, damage);
}


bool alive(Character* c) {
    return c->health > 0;
}


void c_repr(Character* self, int start_health) {
    wchar_t *filled_bar = L"█";
    wchar_t *empty_bar = L"-";

    int bar_length =  20;
    float percent_health = (float)self->health / start_health;
    int filled = percent_health * bar_length;
    percent_health = percent_health  * 100;
    int empty = bar_length - filled;

    wchar_t temp_buffer[128];
    int b_index = 0;
    for (int i = 0; i < filled; i++) {
        temp_buffer[b_index++] = *filled_bar;
    }

    for (int i = 0; i < empty; i++) {
        temp_buffer[b_index++] = *empty_bar;
    }

    char* color;
    if (percent_health > 50) {
        char* c = "\033[92m";
        color = c;
    } 
    else if (percent_health > 25) {
        char* c = "\033[93m";
        color = c;
    } 
    else {
        char* c = "\033[91m";
        color = c;
    }
    char* reset = "\033[0m";
    temp_buffer[b_index] = 0;

    printf("%s %s", self->name, color);
    wprintf(L"|%ls|", temp_buffer);
    printf("%s\n", reset);
}


void defend(void* warrior) {
    struct Warrior* w = (struct Warrior*)warrior;
    int defense = w->armor * randint(1, 3);
    printf("Warrior defends for %d\n", defense);
    w->stats.health += defense;
}


void show_special(char* c) {
    printf("%s", c);
}


void wi_repr(void* wizard, int start_health) {
    struct Wizard *w = (struct Wizard*)wizard;
    c_repr(&(w->stats), start_health);
    printf("Wizard has %d mana\n", w->mana);
}


void wa_repr(void* warrior, int start_health) {
    struct Warrior *w = (struct Warrior*)warrior;
    c_repr(&(w->stats), start_health);
}


struct Warrior wa_init(char* name, int health, int power, int armor) {
    struct Warrior wa  = {armor, {name, health, power}, "2 - Defend (restore HP)", wa_repr, defend};
    return wa;
}


struct Wizard wi_int(char* name, int health, int power, int mana) {
    struct Wizard wi = {mana, {name, health, power}, "2 - Meditate (restore mana)", wi_repr};
    return wi;
}


int main() {
    setlocale(LC_CTYPE, ""); 

    struct Warrior w = wa_init("test", 10, 20, 5);
    Character c = {"hey", 5, 2};

    struct Wizard wi = wi_int("guh", 20, 20, 20);

    wi.repr(&wi, 20);
    attack(&c, &(w.stats));

    printf("\n\n");
    w.repr(&w, 10);
    printf("\n\n");

    defend(&w);
    show_special(w.show_special);

}