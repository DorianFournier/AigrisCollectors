#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/constants.h"

typedef struct {
  uint16_t pos_X;
  uint16_t pos_Y;
} T_point;

typedef enum {
  UNKNOWN_SHIP,
  ATTACKERS_SHIP,
  EXPLORER_SHIP,
  COLLECTOR_SHIP
} T_ship_type;

typedef enum { MOVE_CMD, FIRE_CMD, RADAR_CMD } T_command_type;

typedef struct {
  int id;
  int abscisse;
  int ordinate;
  int angle;
  int speed;
  bool status;
  T_ship_type ship_type;

} T_ship;

typedef struct {
  uint8_t team;
  uint8_t ship_id;
  uint16_t abscisse;
  uint16_t ordinate;
  uint8_t status;
} T_ship_test;

typedef struct {
  uint8_t planet_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  uint8_t ship_ID;
  uint8_t saved;
} T_planet;

typedef struct {
  uint16_t pos_X;
  uint16_t pos_Y;
} T_planet_test;

extern T_planet *planets;
extern bool is_comptetion_started;
extern uint8_t nb_planets;
extern T_ship_test *ship;
extern uint8_t nb_ship;

char *test_ship(uint8_t id, char *command_buffer);
char *explore(uint8_t ship_id, char *command_buffer);
char *move(uint8_t ship_id, uint16_t angle, uint16_t speed,
           char *command_buffer);

void set_ship_type(T_ship *ship);
char *generate_command(T_command_type command_type, int ship_id, int angle,
                       int speed, char *command_buffer);

uint16_t angle_entre_points(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

T_planet_test parse_planet(const char *chaine);
T_ship_test parse_ship(const char *chaine);

uint16_t get_opposite_side(T_point starting_point, T_point ending_point);
uint16_t get_adjacent_side(T_point starting_point, T_point ending_point);
uint16_t get_distance(uint16_t adjacent_side, uint16_t opposite_side);
uint16_t get_angle(uint16_t opposite_side, uint16_t distance);
uint16_t test(T_point starting_point, T_point ending_point);

char *create_buffer(int buffer_size);
int free_buffer(char *buffer_ptr);

#endif // GAME_ENGINE_H
