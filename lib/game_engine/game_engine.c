#include "game_engine.h"
#include "OS_engine.h"
bool is_comptetion_started_for_all = false;
uint8_t nb_planets = 0;
uint8_t nb_ships = 0;
bool game_state = true; // TODO : RENAME THIS

T_game_data game_data[NUMBER_OF_GAME_DATA];
T_planet planets[MAX_PLANETS_NUMBER] = {
    {1, 1, 1, 1, 1}, {2, 2, 2, 2, 2}, {3, 3, 3, 3, 3}, {4, 4, 4, 4, 4},
    {5, 5, 5, 5, 5}, {6, 6, 6, 6, 6}, {7, 7, 7, 7, 7}, {8, 8, 8, 8, 8}};

char *explore(uint8_t ship_id, char *command_buffer) {
  command_buffer = generate_command(RADAR_CMD, ship_id, 0, 0, command_buffer);
  return command_buffer;
}

char *move(uint8_t ship_id, uint16_t angle, uint16_t speed,
           char *command_buffer) {
  command_buffer =
      generate_command(MOVE_CMD, ship_id, angle, speed, command_buffer);
  return command_buffer;
}

char *generate_command(T_command_type command_type, int ship_id, int angle,
                       int speed, char *command_buffer) {
  switch (command_type) {
  case MOVE_CMD:
    snprintf(command_buffer, BUFFER_SIZE, "MOVE %d %d %d\n", ship_id, angle,
             speed);
    break;
  case FIRE_CMD:
    snprintf(command_buffer, BUFFER_SIZE, "FIRE %d %d\n", ship_id, angle);
    break;
  case RADAR_CMD:
    snprintf(command_buffer, BUFFER_SIZE, "RADAR %d\n", ship_id);
    break;
  }

  return command_buffer;
}

void ship_manager(uint8_t id) {
  static char answer_buffer[RX_COMMAND_BUFFER_SIZE] = {0};
  char command_buffer[BUFFER_SIZE] = {0};

  while (1) {
    static int _angle = 90;
    static int _speed = 0;

#ifndef DEMO
    if (id == 6) {
      generate_command(RADAR_CMD, id, _angle, _speed, command_buffer);
      send_command(command_buffer, answer_buffer);
      parse_radar_data(answer_buffer);
    } else {
      if (game_data->planets[0].ship_ID != -1) {
        set_direction(GO_TO_BASE, game_data->ships[8], game_data->planets[0],
                      game_data->base, 1000, command_buffer);
        send_command(command_buffer, answer_buffer);
      } else {
        set_direction(GO_TO_PLANET, game_data->ships[8], game_data->planets[0],
                      game_data->base, 1000, command_buffer);
        send_command(command_buffer, answer_buffer);
      }
    }
    memset(command_buffer, 0, sizeof(command_buffer));
    memset(answer_buffer, 0, sizeof(answer_buffer));
#endif

#ifdef DEMO
    if (_angle >= 359) {
      _angle = 0;
    }

    if (_speed >= 999) {
      _speed = 0;
    }

    _angle += 10;
    _speed += 10;

    if (id == 1 || id == 5) {
      generate_command(FIRE_CMD, id, _angle, _speed, command_buffer);
      send_command(command_buffer, answer_buffer);
    } else {
      generate_command(MOVE_CMD, id, _angle, _speed, command_buffer);
      send_command(command_buffer, answer_buffer);
    }
#endif
  }
}

uint16_t get_distance_between_two_points(T_point starting_point,
                                         T_point ending_point) {
  return sqrt(pow(ending_point.pos_X - starting_point.pos_X, 2) +
              pow(ending_point.pos_Y - starting_point.pos_Y, 2));
}

uint16_t get_angle_between_two_points(T_point starting_point,
                                      T_point ending_point) {
  double angle_radian = atan2(ending_point.pos_Y - starting_point.pos_Y,
                              ending_point.pos_X - starting_point.pos_X);
  int16_t angle_degree = (int16_t)(angle_radian * (180.0 / M_PI));

  if (angle_degree < 0)
    angle_degree += 360;

  return (uint16_t)angle_degree;
}

char *create_buffer(int buffer_size) {
  char *buffer = (char *)malloc(buffer_size * sizeof(char));

  if (buffer == NULL) {
    while (1) {
    }
  }

  return buffer;
}

void free_buffer(char *buffer_ptr) {
  if (buffer_ptr != NULL) {
    free(buffer_ptr);
  } else {
    while (1)
      ;
  }
}

T_point get_ship_position(T_ship ship) {
  T_point ship_pos = {ship.pos_X, ship.pos_Y};
  return ship_pos;
}

T_point get_planet_position(T_planet planet) {
  T_point planet_pos = {planet.pos_X, planet.pos_Y};
  return planet_pos;
}

T_point get_base_position(T_base base) {
  T_point base_pos = {base.pos_X, base.pos_Y};
  return base_pos;
}

void acquire_game_data_mutex(void) { os_acquire_game_data_mutex(); }

void release_game_data_mutex(void) { os_release_game_data_mutex(); }

// set_direction(GO_TO_PLANET, game_data.ship)
void set_direction(T_mode_direction mode, T_ship ship, T_planet planet,
                   T_base base, uint16_t ship_speed, char *command_buffer) {

  T_point ship_pos = get_ship_position(ship);
  T_point planet_pos = get_planet_position(planet);
  T_point base_pos = get_base_position(base);

  switch (mode) {
  case GO_TO_PLANET:
    generate_command(MOVE_CMD, ship.ship_ID,
                     get_angle_between_two_points(ship_pos, planet_pos),
                     ship_speed, command_buffer);
    break;

  case GO_TO_SHIP:
    /* code */
    break;

  case GO_TO_BASE:
    generate_command(MOVE_CMD, ship.ship_ID,
                     get_angle_between_two_points(ship_pos, base_pos),
                     ship_speed, command_buffer);
    break;

  default:
    break;
  }
}

void collect_planet(T_game_data *game_data, char *command_buffer) {
  T_point ship_point = {game_data->ships[8].pos_X, game_data->ships[8].pos_Y};
  T_point planet_point = {game_data->planets[0].pos_X,
                          game_data->planets[0].pos_Y};

  uint16_t angle = get_angle_between_two_points(ship_point, planet_point);
  generate_command(MOVE_CMD, game_data->ships[8].ship_ID, angle, 1000,
                   command_buffer);
}

void parse_radar_data_mutex(char *answer_buffer) {
  acquire_game_data_mutex();
  parse_planets(answer_buffer, game_data, &nb_planets);
  parse_ships(answer_buffer, game_data, &nb_ships);
  parse_base(answer_buffer, game_data);
  release_game_data_mutex();
}

void parse_radar_data(char *answer_buffer) {
  parse_planets(answer_buffer, game_data, &nb_planets);
  parse_ships(answer_buffer, game_data, &nb_ships);
  parse_base(answer_buffer, game_data);
}

void parse_planets(const char *server_response, T_game_data *game_data,
                   uint8_t *num_planets) {
  *num_planets = 0;
  const char *str = server_response;
  const char *delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);

  while (delimiter != NULL) {
    if (*str == SERVER_RESPONSE_PLANET_DELIMITER) {
      if (*num_planets >= MAX_PLANETS_NUMBER) {
        while (1) {
          puts("num_planets > MAX_PLANETS_NUMBE\n");
        }
      }
      sscanf(str, "P %hu %hu %hu %hu %hu",
             &game_data->planets[*num_planets].planet_ID,
             &game_data->planets[*num_planets].pos_X,
             &game_data->planets[*num_planets].pos_Y,
             &game_data->planets[*num_planets].ship_ID,
             &game_data->planets[*num_planets].planet_saved);
      (*num_planets)++;
    }

    str = delimiter + 1;
    delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);
  }
}

void parse_ships(const char *server_response, T_game_data *game_data,
                 uint8_t *num_ships) {
  *num_ships = 0;
  const char *str = server_response;
  const char *delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);

  while (delimiter != NULL) {
    if (*str == SERVER_RESPONSE_SHIP_DELIMITER) {
      if (*num_ships >= 36) {
        break;
      }
      sscanf(str, "S %hu %hu %hu %hu %hu",
             &game_data->ships[*num_ships].team_ID,
             &game_data->ships[*num_ships].ship_ID,
             &game_data->ships[*num_ships].pos_X,
             &game_data->ships[*num_ships].pos_Y,
             &game_data->ships[*num_ships].broken);
      (*num_ships)++;
    }

    str = delimiter + 1;
    delimiter = strchr(str, SERVER_RESPONSE_DELIMITER[0]);
  }
}

void parse_base(const char *server_response, T_game_data *game_data) {
  const char *str_token;
  char *save_ptr;
  char server_response_copy[strlen(server_response) + 1];
  strcpy(server_response_copy, server_response);

  str_token =
      strtok_r(server_response_copy, SERVER_RESPONSE_DELIMITER, &save_ptr);
  while (str_token != NULL) {
    if (str_token[0] == 'B') {
      sscanf(str_token, "B %hu %hu", &game_data->base.pos_X,
             &game_data->base.pos_Y);
    }

    str_token = strtok_r(NULL, SERVER_RESPONSE_DELIMITER, &save_ptr);
  }
}

void show_planet(T_planet *planet) {
  char buffer[RX_COMMAND_BUFFER_SIZE];

  for (uint8_t id = 0; id < nb_planets; id++) {
    sprintf(
        buffer,
        "planet_ID: %u, pos_X: %u, pos_Y: %u, ship_ID: %d, planet_saved: %u",
        planet[id].planet_ID, planet[id].pos_X, planet[id].pos_Y,
        planet[id].ship_ID, planet[id].planet_saved);

    puts(buffer);
  }
}

void initialize_game_data(T_game_data *game_data) {
  for (int i = 0; i < 8; i++) {
    game_data->planets[i].planet_ID = 0;
    game_data->planets[i].pos_X = 0;
    game_data->planets[i].pos_Y = 0;
    game_data->planets[i].ship_ID = -1;
    game_data->planets[i].planet_saved = 0;
  }

  for (int i = 0; i < 9 * 4; i++) {
    game_data->ships[i].team_ID = 0;
    game_data->ships[i].ship_ID = 0;
    game_data->ships[i].pos_X = 0;
    game_data->ships[i].pos_Y = 0;
    game_data->ships[i].broken = 0;
  }

  game_data->base.pos_X = 0;
  game_data->base.pos_Y = 0;
}

T_test get_nearest_planet_available(T_game_data *game_data) {
  T_test best_data = {0, 0, 20000};
  uint16_t distance = 0;

  for (uint8_t ship_num = 8; ship_num <= SHIPS_NUMBER; ship_num++) {
    for (uint8_t planet_num = 0; planet_num < MAX_PLANETS_NUMBER;
         planet_num++) {
      distance = get_distance_between_two_points(
          get_ship_position(game_data->ships[ship_num]),
          get_planet_position(game_data->planets[planet_num]));

      // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n", distance,
      //        ship_num, planet_num);
      if (distance < best_data.distance) {
        // printf("Distance / ship_id -> planet_id : %d / %d -> %d\n", distance,
        //        ship_num, planet_num);
        best_data.distance = distance;
        best_data.ship_id = ship_num;
        best_data.planet_id = planet_num;
      }
    }
  }

  return best_data;
}