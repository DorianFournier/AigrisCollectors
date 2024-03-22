#include "game_engine.h"

bool is_comptetion_started = false;
T_planet *planets = NULL;
T_ship_test *ship = NULL;

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

void set_ship_type(T_ship *ship) {
  if (ship->id >= 1 && ship->id <= 5) {
    ship->ship_type = ATTACKERS_SHIP;
  } else if (ship->id > 5 && ship->id <= 7) {
    ship->ship_type = EXPLORER_SHIP;
  } else if (ship->id >= 8 && ship->id <= 9) {
    ship->ship_type = COLLECTOR_SHIP;
  } else {
    ship->ship_type = UNKNOWN_SHIP;
  }
}

char *test_ship(uint8_t id, char *command_buffer) {
  static int _angle = 90;
  static int _speed = 0;

  if (is_comptetion_started == true) {
    if (_angle >= 359) {
      _angle = 0;
    }

    if (_speed >= 999) {
      _speed = 0;
    }

    _angle += 10;
    _speed += 10;

    command_buffer =
        generate_command(MOVE_CMD, id, _angle, _speed, command_buffer);
    // generate_command(FIRE_CMD, 1, _angle, _speed, command_buffer);

    return command_buffer;
  } else {
    return NULL;
    /* do nothing */
  }
}

uint16_t get_opposite_side(T_point starting_point, T_point ending_point) {
  return ending_point.pos_X - starting_point.pos_X;
}

uint16_t get_adjacent_side(T_point starting_point, T_point ending_point) {
  return ending_point.pos_Y - starting_point.pos_Y;
}

uint16_t get_distance(uint16_t adjacent_side, uint16_t opposite_side) {
  return sqrt(pow(adjacent_side, 2) + pow(opposite_side, 2));
}

uint16_t get_angle(uint16_t opposite_side, uint16_t distance) {
  double result =
      asin((double)opposite_side / (double)distance) * (180.0 / M_PI);
  return result;
}

uint16_t test(T_point starting_point, T_point ending_point) {
  uint16_t opposite_side = get_opposite_side(starting_point, ending_point);
  uint16_t adjacent_side = get_adjacent_side(starting_point, ending_point);
  uint16_t distance_between_point = get_distance(adjacent_side, opposite_side);
  uint16_t angle = get_angle(opposite_side, distance_between_point);

  return angle;
}

uint16_t angle_entre_points(uint16_t x1, uint16_t y1, uint16_t x2,
                            uint16_t y2) {
  uint16_t diff_x = x2 - x1;
  uint16_t diff_y = y2 - y1;
  uint16_t angle_radians = atan2(diff_y, diff_x);
  uint16_t angle_degrees = angle_radians * (180.0 / M_PI);

  if (angle_degrees < 0) {
    angle_degrees += 360.0;
  }

  return angle_degrees;
}

T_planet_test parse_planet(const char *chaine) {
  T_planet_test planet;
  char type;
  sscanf(chaine, "%c %hd %hd", &type, &planet.pos_X, &planet.pos_Y);
  return planet;
}

T_ship_test parse_ship(const char *chaine) {
  T_ship_test ship;
  char type;
  sscanf(chaine, "%c %hhd %hhd %hd %hd %hhd", &type, &ship.team, &ship.ship_id,
         &ship.abscisse, &ship.ordinate, &ship.status);
  return ship;
}

char *create_buffer(int buffer_size) {
  static int counter = 0;
  counter++;
  char *buffer = (char *)malloc(buffer_size * sizeof(char));

  if (buffer == NULL) {
    while (1) {
    }
  }

  return buffer;
}

int free_buffer(char *buffer_ptr) {
  if (buffer_ptr != NULL) {
    free(buffer_ptr);

    return 0;
  }
  return -1;
}