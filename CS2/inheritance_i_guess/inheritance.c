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
    void (*repr)(void*, int);
    bool (*alive)(void*);
} Character;


typedef struct {
    Character stats;

    void (*repr)(void*, int);
    void (*special)(void*);
    void (*show_special)();
    void (*attack)(void*, Character*);
    bool (*alive)(void*);
} Entity;


typedef struct {
    int armor;
    Entity e;
} Warrior;


typedef struct {
    int mana;
    Entity e;
} Wizard;


void take_damage(Character* c, int damage) {
    c->health -= damage;
    printf("%s takes %d damage\n", c->name, damage);
}


void attack(Character* self, Character* other) {
    int damage = self->power * randint(1, 3);
    printf("%s attacks %s\n", self->name, other->name);
    take_damage(other, damage);
}


void wa_attack(void* warrior, Character* other) {
    Warrior* self = (Warrior*)warrior;
    attack(&self->e.stats, other);
}


void wi_attack(void* wizard, Character* other) {
    Wizard* self = (Wizard*)wizard;
    float mult = 1 + ((float)self->mana / 30);
    if (self->mana < 5) {
        printf("Cannot attack! Not enough mana!");
    }

    int damage = (float)(self->e.stats.power * randint(1, 3)) * mult;
    printf("%s attacks %s\n", self->e.stats.name, other->name);
    take_damage(other, damage);
    self->mana -= 5;
    printf("%s has %d mana left\n", self->e.stats.name, self->mana);
}


bool alive(Character* c) {
    return c->health > 0;
}


bool c_alive(void* self) {
    Character *c = (Character*)self;
    return alive(c);
}


bool wa_alive(void* self) {
    Entity *w = (Entity*)self;
    return alive(&w->stats);
}


bool wi_alive(void* self) {
    Entity *w = (Entity*)self;
    return alive(&w->stats);
}


void defend(void* warrior) {
    Warrior* w = (Warrior*)warrior;
    int defense = w->armor * randint(1, 3);
    printf("Warrior defends for %d\n", defense);
    w->e.stats.health += defense;
}


void meditate(void* wizard) {
    Wizard *self = (Wizard*)wizard;
    int max_mana = 30;
    if (self->mana >= max_mana) {
        printf("Already at max mana\n");
        return;
    }

    self->mana = min(self->mana + 10, max_mana);
    printf("%s restores mana. Now at %d mana\n", self->e.stats.name, self->mana);
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


void c_handle_repr(void* c, int max_health){
    Character* self = (Character*)c;
    c_repr(self, max_health); 
    printf("\n");
}


void wi_repr(void* wizard, int start_health) {
    Wizard *w = (Wizard*)wizard;
    c_repr(&(w->e.stats), start_health);
    printf("The wizard %s has %d mana\n\n", w->e.stats.name, w->mana);
}


void wa_repr(void* warrior, int start_health) {
    Warrior *w = (Warrior*)warrior;
    c_repr(&(w->e.stats), start_health);
    printf("\n");
}


void wa_show_special() {
    printf("2 - Defend (restore HP)");
}


void wi_show_special() {
    printf("2 - Meditate (restore mana)");
}


Warrior wa_init(char* name, int health, int power, int armor) {
    Warrior wa  = {armor, {{name, health, power, NULL}, 
                           wa_repr, defend, wa_show_special, 
                           wa_attack, wa_alive}};
    return wa;
}


Wizard wi_int(char* name, int health, int power, int mana) {
    Wizard wi = {mana, {{name, health, power, NULL}, 
                 wi_repr, meditate, wi_show_special,
                 wi_attack, wi_alive}};

    return wi;
}

Character c_init(char* name, int health, int power) {
    Character c = {name, health, power, c_handle_repr, c_alive};
    return c;
}


int main() {
    setlocale(LC_CTYPE, ""); 

    printf("Choose your hero...\n");
    printf("1. Yuri Warrior\n");
    printf("2. Yuri Wizard\n");

    void* hero_holder;
    int max_health = 100;
    int choice = 0;
    Entity* hero = NULL;
    printf("> ");
    scanf_s("%d", &choice);
    if (choice == 1) {
        Warrior wa = wa_init("Yuri Warrior", max_health, 12, 8);
        hero_holder = &wa;
        hero = &(wa.e);
    }
    else {
        Wizard wi = wi_int("Yuri Wizard", max_health, 10, 30);
        hero_holder = &wi;
        hero = &(wi.e);
    }

    Character e = c_init("Troll", max_health, 9);
    Character* enemy = &e;
    bool fled = false;

    while (hero->alive(hero) && enemy->alive(enemy)) {
        printf("\n");
        hero->repr(hero_holder, max_health);
        enemy->repr(enemy, max_health);
        printf("\nYour turn\n1 - attack\n");
        hero->show_special();
        printf("\n3 - do nothing\n4 - quit\n\n");

        printf("> ");
        scanf_s("%d", &choice);
        switch (choice){
            case 1:
                hero->attack(hero_holder, enemy);
                break;
            case 2:
                hero->special(hero_holder);
                break;
            case 3:
                printf("Hero does nothing\n");
                break;
            case 4:
                printf("You fled\n");
                fled = true;
                break;
            default:
                printf("Invalid options");
        }

        if (fled) {
            break;
        }

        if (enemy->alive(enemy)) {
            attack(enemy, &hero->stats);
        }
        printf("\n\n");
    }

    hero->repr(hero_holder, max_health);
    enemy->repr(enemy, max_health);

    if (!enemy->alive(enemy)) {
        printf("You win! Yuri reigns!");
    }
    else {
        printf("You're so dead. Get better");
    }
}   