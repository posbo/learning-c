#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ex19.h"

int Player_init(void *self)
{
    Player *player = self;

    player->hit_points = 100;
    player->xp = 0;
    player->damage = 8;
    player->damage_multiplier = 0.3;

    printf("%s entered the dungeon", player->_(description));

    return 1;
}

Object PlayerProto = {
    .init = Player_init
};

int Monster_attack(void *self, int damage)
{
    Monster *monster = self;

    printf("You attack %s for %dhp of damage\n", monster->_(description), damage);

    monster->hit_points -= damage;

    if(monster->hit_points > 0) {
        printf("It is still alive and has %dhp remaining\n.",
                monster->hit_points);
        return 0;
    } else {
        printf("It is dead! it drops %d gold coins\n",
                monster->loot);
        return 1;
    }
}

int Monster_init(void *self)
{
    Monster *monster = self;
    monster->hit_points = 10;
    monster->loot = 100;
    return 1;
}

Object MonsterProto = {
    .init = Monster_init,
    .attack = Monster_attack
};

void *Room_move(void *self, Direction direction)
{
    Room *room = self;
    Room *next = NULL;

    if(direction == NORTH && room->north) {
        printf("You go north, into:\n");
        next = room->north;
    } else if (direction == SOUTH && room->south) {
        printf("You go south, into:\n");
        next = room->south;
    } else if (direction == EAST && room->east) {
        printf("You go east, into:\n");
        next = room->east;
    } else if (direction == WEST && room->west) {
        printf("You go west, into:\n");
        next = room->west;
    } else {
        printf("You can't go in that direction.");
        next = NULL;
    }

    if(next) {
        next->_(describe)(next);
    }

    return next;
}

int Room_attack(void *self, int damage)
{
    Room *room = self;
    Monster *monster = room->bad_guy;

    if(monster) {
        monster->_(attack)(monster, damage);
        return 1;
    } else {
        printf("You flail in the air at nothing.\n");
        return 0;
    }
}

Object RoomProto = {
    .move = Room_move,
    .attack = Room_attack
};

void *Map_move(void *self, Direction direction)
{
    Map *map = self;
    Room *location = map->location;
    Room *next = NULL;

    next = location->_(move)(location, direction);

    if(next) {
        map->location = next;
    }

    return next;
}

int Map_attack(void *self, int damage)
{
    Map* map = self;
    Room *location = map->location;

    return location->_(attack)(location, damage);
}

int Map_init(void *self)
{
    Map *map = self;

    // make some rooms
    Room *hall = NEW(Room, "The great hall");
    Room *throne = NEW(Room, "The throne room");
    Room *arena = NEW(Room, "The arena, with the minotaur");
    Room *kitchen = NEW(Room, "Kitchen, you have the knife now");

    // add bad guy
    arena->bad_guy = NEW(Monster, "The evil minotaur");

    // setup the map rooms
    hall->north = throne;

    throne->west = arena;
    throne->east = kitchen;
    throne->south = hall;

    arena->east = throne;
    kitchen->west = throne;

    // start in hall
    map->start = hall;
    map->location = hall;

    return 1;
}

Object MapProto = {
    .init = Map_init,
    .move = Map_move,
    .attack = Map_attack
};

int process_input(Map *game, Player *player)
{
    printf("\n> ");

    char ch = getchar();
    getchar();

    int max_damage = player->damage + (player->damage * player->damage_multiplier);
    int damage = rand() % max_damage;

    switch(ch) {
        case 113:
            printf("Giving up?\n");
            return 0;
            break;
        case 'n':
            game->_(move)(game, NORTH);
            break;
        case 's':
            game->_(move)(game, SOUTH);
            break;
        case 'e':
            game->_(move)(game, EAST);
            break;
        case 'w':
            game->_(move)(game, WEST);
            break;
        case 'a':
            game->_(attack)(game, damage);
            break;
        case 'l':
            printf("You can go:\n");
            if(game->location->north) printf("NORTH\n");
            if(game->location->south) printf("SOUTH\n");
            if(game->location->east) printf("EAST\n");
            if(game->location->west) printf("WEST\n");
            break;
        default:
            printf("What?: %d\n", ch);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    // simple way to setup the randomness
    srand(time(NULL));

    // create map
    Map *game = NEW(Map, "The hall of the minotaur");
    //create player
    Player *player = NEW(Player, argv[1]);

    game->location->_(describe)(game->location);

    while(process_input(game, player)) {
    }

    return 0;
}
