#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#include <constants.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { GO_TO_PLANET, GO_TO_SHIP, GO_TO_BASE } T_mode_direction;

typedef enum { MOVE_CMD, FIRE_CMD, RADAR_CMD } T_command_type;

typedef enum {
  TOP = 0,
  TOP_RIGHT = 315,
  RIGHT = 270,
  BOTTOM_RIGHT = 225,
  BOTTOM = 180,
  BOTTOM_LEFT = 135,
  LEFT = 90,
  TOP_LEFT = 45
} T_follower_ship_direction;

typedef enum {
  COLLECTOR_SPEED = 1000,
  EXPLORER_SPEED = 2000,
  ATTACKER_SPEED = 3000
} T_ships_speed;

typedef enum { UNKNOWN_SHIP, COLLECTOR, EXPLORER, ATTACKER } T_ship_type;

typedef struct {
  uint16_t pos_X;
  uint16_t pos_Y;
} T_point;

typedef enum {
  // OK
  READY,
  GOING_TO_PLANET,
  COLLECTING,
  COLLECTED,
  // ERRORS STATE
  COLLECTING_WRONG_PLANET,
  PLANET_STOLEN,
  BROKEN,
  COOLDOWN,
  UNKNOWN,
} T_ship_FSM; // Advanced status of the planet

typedef enum { DESTROYED, OUT_OF_RANGE, MISSED } T_fire_result;

typedef struct {
  uint16_t team_ID;
  uint16_t ship_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  uint16_t broken;
  T_ship_FSM FSM;
  int8_t target_planet_ID;
  uint16_t angle;
} T_ship;

typedef struct {
  uint16_t planet_ID;
  uint16_t pos_X;
  uint16_t pos_Y;
  int16_t ship_ID;
  uint16_t planet_saved;
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
void reset_undetected_enemy_ships(uint8_t num_ships, T_game_data *game_data);

// Command functions
char *generate_command(T_command_type command_type, uint8_t ship_id,
                       uint16_t angle, uint16_t speed);
void go_to_planet(uint8_t ship_id, uint8_t planet_id);
void go_to_base(uint8_t ship_id, T_base base, T_ships_speed ship_speed);
void go_to_point(uint8_t ship_id, T_point point);

// follow ship functions
void follow_ship(uint8_t follower_ship_id, uint8_t ship_to_follow_id,
                 uint16_t follower_ship_speed,
                 T_follower_ship_direction relative_position);
void ship_following_collector(uint8_t ship_id, uint8_t collector_id,
                              T_follower_ship_direction relative_position);
T_point polar_to_cartesian_coordinates(uint8_t ship_id, uint16_t distance,
                                       uint16_t angle, T_game_data *game_data);

// Trigonometry functions
uint16_t get_distance_between_two_points(T_point starting_point,
                                         T_point ending_point);
uint16_t get_angle_between_two_points(T_point starting_point,
                                      T_point ending_point);
uint16_t get_angle_for_follower_ship(uint8_t ship_id, uint16_t angle,
                                     T_game_data *game_data);

void show_planet(T_planet *planet);
void initialize_game_data(T_game_data *game_data);
void auto_collect_planet(uint8_t ship_id, T_game_data *game_data);
void set_ship(uint8_t ship_id, T_game_data *game_data);

int8_t get_planet_ID_from_ship(uint8_t ship_id, T_game_data *game_data);
T_point get_ship_position(T_ship ship);
T_point get_planet_position(T_planet planet);
T_point get_base_position(T_base base);
uint8_t get_nearest_planet(uint8_t ship_id, T_game_data *game_data);
uint16_t check_desired_ship_speed(uint8_t ship_id, uint16_t desired_speed);

bool can_ship_be_READY(uint8_t ship_id, T_game_data *game_data);

// Initialiazing functions
void initialize_game_data(T_game_data *game_data);

// Debug functions
void show_planet(T_planet *planet);

// Fire functions
T_fire_result fire_on_enemy_ship(uint8_t attacker_id, uint8_t enemy_ship_id,
                                 T_game_data *game_data);

// FSM Functions
T_ship_FSM get_ship_FSM(const uint8_t ship_id, const T_game_data *game_data);
void set_ship_FSM(uint8_t ship_id, T_ship_FSM FSM_state,
                  T_game_data *game_data);
void set_ship_target_planet_ID(uint8_t ship_id, int8_t target_planet_id,
                               T_game_data *game_data);

bool can_ship_be_READY(uint8_t ship_id, T_game_data *game_data);
bool can_ship_be_GOING_TO_PLANET(uint8_t ship_id, int8_t desired_target_ID,
                                 T_game_data *game_data);
bool can_ship_be_COLLECTING(uint8_t ship_id, T_game_data *game_data);
bool can_ship_be_COLLECTED(uint8_t ship_id, T_game_data *game_data);
bool can_ship_be_COLLECTING_WRONG_PLANET(uint8_t ship_id,
                                         T_game_data *game_data);
bool can_ship_be_PLANET_STOLEN(uint8_t ship_id, T_game_data *game_data);
bool can_ship_be_BROKEN(uint8_t ship_id, T_game_data *game_data);

int8_t find_nearest_enemy_ship_id(uint8_t collector_id, T_game_data *game_data);
void os_firing_timer_callback(uint8_t ship_id, T_game_data *game_data);

#endif // GAME_ENGINE_H