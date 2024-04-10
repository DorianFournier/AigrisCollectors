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

typedef enum { GO_TO_PLANET, GO_TO_SHIP, GO_TO_BASE } T_mode_direction;

typedef enum { MOVE_CMD, FIRE_CMD, RADAR_CMD } T_command_type;

typedef enum {
  COLLECTOR_SPEED = 1000,
  EXPLORER_SPEED = 2000,
  ATTACKER_SPEED = 3000
} T_ships_speed;

typedef struct {
  uint16_t pos_X;
  uint16_t pos_Y;
} T_point;

typedef struct {
  uint16_t team_ID;
  uint16_t ship_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  uint16_t broken;
} T_ship;

typedef enum {
  FREE,
  COLLECTING_INCOMING,
  COLLECTING,
  COLLECTED,
} T_planet_status; // Advanced status of the planet

typedef struct {
  uint16_t planet_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  int16_t ship_ID;
  uint16_t planet_saved;
  int8_t busy_ship_ID;
  T_planet_status planet_status;
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

extern uint8_t nb_planets;
extern T_game_data game_data[NUMBER_OF_GAME_DATA];

// Main functions for ships
void attacker_manager(uint8_t id);
void explorer_manager(uint8_t id);
void collector_manager(uint8_t id);

// Parsing functions
void parse_planets(const char *server_response, T_game_data *game_data,
                   uint8_t *num_planets);
void parse_ships(const char *server_response, T_game_data *game_data);
void parse_base(const char *server_response, T_game_data *game_data);

// Command functions
char *generate_command(T_command_type command_type, uint8_t ship_id,
                       uint16_t angle, uint16_t speed);
void go_to_planet(T_ship ship, T_planet planet);
void go_to_planet_new(uint8_t ship_id, T_planet planet);
void go_to_base(T_ship ship, T_base base, T_ships_speed ship_speed);
void go_to_base_new(uint8_t ship_id, T_base base, T_ships_speed ship_speed);
void go_to_point(T_ship ship, T_point point);
void follow_ship(T_ship follower_ship, T_ship ship_to_follow);
void follow_ship_new(uint8_t follower_ship_id, T_ship ship_to_follow);
// Trigonometry functions
uint16_t get_distance_between_two_points(T_point starting_point,
                                         T_point ending_point);
uint16_t get_angle_between_two_points(T_point starting_point,
                                      T_point ending_point);

void show_planet(T_planet *planet);
void initialize_game_data(T_game_data *game_data);
void auto_collect_planet(uint8_t ship_id, T_game_data *game_data);
void set_planet_collection_status(int8_t busy_ship_ID, uint8_t planet_num,
                                  T_planet_status planet_status,
                                  T_game_data *game_data);
void update_planet_collection_status_new_2(uint8_t ship_id,
                                           T_game_data *game_data);
void update_planet_collection_status(T_game_data *game_data);

void update_planet_collection_status_2(T_game_data *game_data);
void auto_collect_planet_2(uint8_t ship_id, T_game_data *game_data);
void auto_collect_planet_collector_1(uint8_t ship_id, T_game_data *game_data);
void auto_collect_planet_collector_2(uint8_t ship_id, T_game_data *game_data);

T_point get_ship_position(T_ship ship);
T_point get_planet_position(T_planet planet);
T_point get_base_position(T_base base);
uint8_t get_nearest_planet(uint8_t ship_id, T_game_data *game_data);
uint8_t get_nearest_planet_available(uint8_t ship_id, T_game_data *game_data);
uint16_t check_desired_ship_speed(uint8_t ship_id, uint16_t desired_speed);
uint8_t is_ship_have_a_planet(uint8_t ship_id, T_game_data *game_data);

// Initialiazing functions
void initialize_game_data(T_game_data *game_data);

// Debug functions
void show_planet(T_planet *planet);

#endif // GAME_ENGINE_H