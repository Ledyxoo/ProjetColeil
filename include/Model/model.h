#ifndef MODEL_H
#define MODEL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>

/**
 * @brief Represents a 2D vector with integer coordinates.
 * 
 * @attr x: The x-coordinate (int)
 * @attr y: The y-coordinate (int)
 */
typedef struct {
    int x;
    int y;
} V2;

/**
 * @brief Represents a 2D vector with double precision coordinates.
 * 
 * @attr x: The x-coordinate (double)
 * @attr y: The y-coordinate (double)
 */
typedef struct {
    double x;
    double y;
} V2_double;

/**
 * @brief Models a planet within a solar system.
 * 
 * @attr position: Position in 2D space (V2)
 * @attr property: Combined measure of radius, mass, and orbital period (int)
 * @attr angle: Current angle in orbit (double)
 * @attr orbit_radius: Radius of the orbit (int)
 */
typedef struct {
    V2 position;
    int property;
    double angle;
    int orbit_radius;
} Planet;

/**
 * @brief Models a star within a solar system.
 * 
 * @attr position: Position in 2D space (V2)
 * @attr property: Combined measure of radius and mass (int)
 */
typedef struct {
    V2 position;
    int property;
} Star;

/**
 * @brief Models an entire solar system.
 * 
 * @attr sun: The central star (Star)
 * @attr nb_planets: Number of planets (int)
 * @attr planets: Array of planets (Planet*)
 */
typedef struct {
    Star sun;
    int nb_planets;
    Planet* planets;
} Solar_system;

/**
 * @brief Models a spaceship.
 * 
 * @attr position: Position in 2D space (V2)
 * @attr dimensions: Width and height (V2)
 * @attr velocity: Speed of movement (double)
 * @attr velocity_vector: Velocity as a 2D vector (V2_double)
 * @attr angle: Orientation angle (double)
 * @attr mass: Mass of the spaceship (int)
 * @attr gravity_vectors: Array of vectors representing gravitational forces (V2*)
 */
typedef struct {
    V2 position;
    V2 dimensions;
    double velocity;
    V2_double velocity_vector;
    double angle;
    int mass;
    V2* gravity_vectors;
} Spaceship;

/**
 * @brief Represents a starting/finishing gate.
 * 
 * @attr position: Position in 2D space (V2)
 * @attr dimensions: Width and height (V2)
 */
typedef struct {
    V2 position;
    V2 dimensions;
} Gate;

/**
 * @brief Models the entire universe.
 * 
 * @attr nb_solar_systems: Number of solar systems (int)
 * @attr nb_stellar_bodies: Total number of stellar bodies (int)
 * @attr solar_systems: Array of solar systems (Solar_system*)
 * @attr start: The start gate (Gate)
 * @attr finish: The finish gate (Gate)
 * @attr win_dimensions: Dimensions of the window (V2)
 */
typedef struct {
    int nb_solar_systems;
    int nb_stellar_bodies;
    Solar_system* solar_systems;
    Gate start;
    Gate finish;
    V2 win_dimensions;
} Universe;

/**
 * @brief Enum representing the current game state.
 * 
 * @attr GAME_STATE_IDLE: Game is idle
 * @attr GAME_STATE_IN_PROGRESS: Game is in progress
 * @attr GAME_STATE_OVER: Game is over
 */
typedef enum {
    GAME_STATE_IDLE = 0,
    GAME_STATE_IN_PROGRESS,
    GAME_STATE_OVER
} GameState;

/**
 * @brief Models the entire game.
 * 
 * @attr universe: The game universe (Universe*)
 * @attr spaceship: The player's spaceship (Spaceship*)
 * @attr state: Current game state (GameState)
 * @attr score: Player's score (int)
 * @attr first_launch: Indicates if it's the first launch of the game (bool)
 * @attr gravity_vector: Combined gravitational force acting on the spaceship (V2)
 */
typedef struct {
    Universe* universe;
    Spaceship* spaceship;
    GameState state;
    int score;
    bool first_launch;
    V2 gravity_vector;
} Game;


void model_free_game(Game* game);
void model_free_universe(Universe* universe);
int model_load_file(char *filename, Universe *universe);
void Game_UpdateState(Game* self);
void random_spaceship_angle(Spaceship* spaceship);

Game* model_game_new(char *filename);
Spaceship* Spaceship_New(int pos_x, int pos_y);
Spaceship* model_spaceship_new(int pos_x, int pos_y, Universe* universe);

// Gravity functions
double model_square_of_distance(int x1,int y1, int x2,int y2);
double model_gravity_vector_magnitude(int x1,int y1, int m1, int x2,int y2, int m2);
void model_rotation_of_phi_angle(V2_double* vector, double phi);

#endif