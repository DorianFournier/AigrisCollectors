#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include <constants.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef enum { GO_TO_PLANET, GO_TO_SHIP, GO_TO_BASE } T_mode_direction;

typedef enum { MOVE_CMD, FIRE_CMD, RADAR_CMD } T_command_type;

typedef struct {
  uint16_t team_ID;
  uint16_t ship_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  uint16_t broken; // same size as an boolean
} T_ship;

typedef struct {
  uint16_t planet_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  int16_t ship_ID;
  uint16_t planet_saved; // same size as an boolean
} T_planet;

typedef struct {
  uint16_t pos_X;
  uint16_t pos_Y;
} T_base;

typedef struct {
  T_planet planets[MAX_PLANETS_NUMBER];
  T_ship ships[SHIPS_NUMBER * NUMBER_OF_TEAM];
  T_base base;
} T_game_data;

typedef struct {
  uint8_t ship_id;
  uint16_t planet_id;
  uint16_t distance;
} T_test;

extern uint8_t nb_planets;
extern uint8_t nb_ships;
extern T_game_data game_data[NUMBER_OF_GAME_DATA];

void ship_manager(uint8_t id);
char *explore(uint8_t ship_id, char *command_buffer);
char *move(uint8_t ship_id, uint16_t angle, uint16_t speed,
           char *command_buffer);

char *generate_command(T_command_type command_type, int ship_id, int angle,
                       int speed, char *command_buffer);

void parse_radar_data(char *answer_buffer);
void parse_planets(const char *server_response, T_game_data *game_data,
                   uint8_t *num_planets);
void parse_ships(const char *server_response, T_game_data *game_data,
                 uint8_t *num_ships);
void parse_base(const char *server_response, T_game_data *game_data);

uint16_t get_distance_between_two_points(T_point starting_point,
                                         T_point ending_point);
uint16_t get_angle_between_two_points(T_point starting_point,
                                      T_point ending_point);

void write_game_data(const char *server_response);

char *create_buffer(int buffer_size);
void free_buffer(char *buffer_ptr);

void show_planet(T_planet *planet);
void initialize_game_data(T_game_data *game_data);
void collect_planet(T_game_data *game_data, char *command_buffer);
T_point get_ship_position(T_ship ship);
T_point get_planet_position(T_planet planet);
T_point get_base_position(T_base base);
void set_direction(T_mode_direction mode, T_ship ship, T_planet planet,
                   T_base base, uint16_t ship_speed, char *command_buffer);

void acquire_game_data_mutex(void);
void release_game_data_mutex(void);

T_test get_nearest_planet_available(T_game_data *game_data);

#endif // GAME_ENGINE_H
