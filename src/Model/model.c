/**
 * @file model.c
 * @brief Contains implementations for game logic.
 */

#include "model.h"

/**
 * @brief Load game configurations into a Universe struct from a file.
 * 
 * @param filename The name of the file to read configurations from.
 * @param universe A pointer to the Universe struct to populate.
 * @return int Returns 0 on success, -1 on failure.
 */
int model_load_file(char* filename, Universe* universe)
{
    FILE* pfile = NULL;
	int x = 0;
    int y = 0;
    int var = 0;
    char key_word[50] = { 0 };

    // Open configuration file
    if(NULL == filename) {
        return -1;
    }

    pfile = fopen(filename, "r");
    if (!pfile) {
        return -1;
    }

    // Read window size
    if (3 != fscanf(pfile, "%s %d %d\n", key_word, &x, &y))
    {
        return -1;
    }
    if(x <=0 || y<= 0) return -1;

    universe->win_dimensions.x = x;
    universe->win_dimensions.y = y;

    // Read start gate
    if (3 != fscanf(pfile,"%s %d %d\n", key_word,&x,&y))
    {
        return -1;
    }
    if(x < 0 || x > universe->win_dimensions.x) return -1;
    if(y < 0 || y > universe->win_dimensions.y) return -1;
    universe->start.position.x = x;
    universe->start.position.y = y;

    // Read finish gate
    if (3 != fscanf(pfile, "%s %d %d\n", key_word, &x, &y))
    {
        return -1;
    }
    if(x < 0 || x > universe->win_dimensions.x) return -1;
    if(y < 0 || y > universe->win_dimensions.y) return -1;
    universe->finish.position.x = x;
    universe->finish.position.y = y;

    // Read the number of solar systems
    if (2 != fscanf(pfile, "%s %d\n", key_word, &var))
    {
        return -1;
    }
    if(var<=0) return -1;
    universe->nb_solar_systems = var;

    // Initialize the array of solar systems
    universe->solar_systems = (Solar_system*)calloc(universe->nb_solar_systems, sizeof(Solar_system));
    if(!universe->solar_systems)
    {
        return -1;
    } 

    // Read solar systems
    for(int i = 0; i < universe->nb_solar_systems; i++)
    {
        // Read star position
        if (3 != fscanf(pfile, "%s %d %d\n", key_word, &x, &y))
        {
            return -1;
        }

		// Check sun position
		if(x < 0 || x > universe->win_dimensions.x) return -1;
		if(y < 0 || y > universe->win_dimensions.y) return -1;

        universe->solar_systems[i].sun.position.x = x;
        universe->solar_systems[i].sun.position.y = y;
    
        // Read star radius
        if (2 != fscanf(pfile, "%s %d\n", key_word, &var))
        {
            return -1;
        }
        if(var<=0) return -1;
        
		if(universe->solar_systems[i].sun.position.x - var < 0
        || universe->solar_systems[i].sun.position.x + var > universe->win_dimensions.x)
            return -1;

		if(universe->solar_systems[i].sun.position.y - var < 0
        || universe->solar_systems[i].sun.position.y + var > universe->win_dimensions.y)
            return -1;

        universe->solar_systems[i].sun.property = var;
        universe->nb_stellar_bodies++;

        // Read the number of planets
        if (2 != fscanf(pfile, "%s %d\n", key_word, &var))
        {
            return -1;
        }
        if(var<=0) return -1;
        universe->solar_systems[i].nb_planets = var;

        // Initialize the array of planets
       
        universe->solar_systems[i].planets=(Planet*)calloc(universe->solar_systems[i].nb_planets,sizeof(Planet));
        if(! universe->solar_systems[i].planets)
        {
            return -1;
        }

        // Read planets
        for(int j = 0; j < universe->solar_systems[i].nb_planets; j++)
        {
            // Read planet radius
            if (2 != fscanf(pfile,"%s %d\n", key_word, &var)
            ||  0 != memcmp(key_word, "PLANET_RADIUS", 14))
            {
            return -1;
            }
            if(var<=0) return -1;
            universe->solar_systems[i].planets[j].property=var;

            // Read planet orbit
            if (2 != fscanf(pfile,"%s %d\n", key_word, &var)
            ||  0 != memcmp(key_word, "PLANET_ORBIT", 13))
            {
                return -1;
            }

			if(universe->solar_systems[i].sun.position.x - abs(var) < 0
            || universe->solar_systems[i].sun.position.x + abs(var) > universe->win_dimensions.x)
                return -1;

		    if(universe->solar_systems[i].sun.position.y - abs(var) < 0
            || universe->solar_systems[i].sun.position.y + abs(var) > universe->win_dimensions.y)
                return -1;

            universe->solar_systems[i].planets[j].orbit_radius = var;
            
            universe->solar_systems[i].planets[j].position.x = universe->solar_systems[i].sun.position.x;
            universe->solar_systems[i].planets[j].position.y = universe->solar_systems[i].sun.position.x - abs(universe->solar_systems[i].planets[j].orbit_radius);
            universe->solar_systems[i].planets[j].position.y = universe->win_dimensions.y - universe->solar_systems[i].planets[j].position.y;

            universe->solar_systems[i].planets[j].position.x= universe->solar_systems[i].sun.position.x;

            universe->nb_stellar_bodies++;
        }
        

    }

    fclose(pfile);

    return 0;
}

/**
 * @brief Creates a new Spaceship instance and initializes it.
 * 
 * @param pos_x The starting x position of the spaceship.
 * @param pos_y The starting y position of the spaceship.
 * @param universe A pointer to the Universe the spaceship belongs to.
 * @return Spaceship* Returns a pointer to the newly created spaceship.
 */
Spaceship* model_spaceship_new(int pos_x, int pos_y, Universe* universe)
{
	Spaceship* spaceship = (Spaceship*)calloc(1, sizeof(Spaceship));
	assert(spaceship);

	spaceship->mass = 2;

	spaceship->dimensions.x = 10;
	spaceship->dimensions.y = 10;
	
    spaceship->position.x = pos_x;
	spaceship->position.y = pos_y;
	
    spaceship->velocity = 70;
    
    spaceship->gravity_vectors = (V2*)calloc(universe->nb_stellar_bodies, sizeof(V2));

	return spaceship;
}

/**
 * @brief Initializes a new Game instance.
 * 
 * @param filename The name of the file to read configurations from.
 * @return Game* Returns a pointer to the newly created Game instance.
 */
Game* model_game_new(char *filename)
{
	Game* game = (Game*)calloc(1, sizeof(Game));
	assert(game);
	
	game->universe = (Universe*)calloc(1, sizeof(Universe));
	assert(game->universe);
	
	if(model_load_file(filename, game->universe) == -1) return 0;

	game->spaceship = model_spaceship_new(game->universe->start.position.x, game->universe->start.position.y, game->universe);
    random_spaceship_angle(game->spaceship);
	assert(game->spaceship);

	// Default planets position
	for (int i = 0; i < game->universe->nb_solar_systems; i++)
	{
		for (int j = 0; j < game->universe->solar_systems[i].nb_planets; j++)
		{
			game->universe->solar_systems[i].planets[j].position.x = game->universe->solar_systems[i].sun.position.x;
			game->universe->solar_systems[i].planets[j].position.y = game->universe->solar_systems[i].sun.position.y- abs(game->universe->solar_systems[i].planets[j].orbit_radius);
		}
	}

	game->state = GAME_STATE_IDLE;

	return game;
}

/**
 * @brief Checks for collision between a spaceship and a planet.
 * 
 * @param spaceship A pointer to the Spaceship instance.
 * @param planet_x The x position of the planet.
 * @param planet_y The y position of the planet.
 * @param planet_radius The radius of the planet.
 * @return bool Returns true if a collision is detected, false otherwise.
 */
bool spaceship_planet_collision(Spaceship *spaceship, int planet_x, int planet_y, int planet_radius) {
    if (spaceship->position.x < planet_x - planet_radius
	||  spaceship->position.x > planet_x + planet_radius) {
        return false;
    }

    if (spaceship->position.y < planet_y - planet_radius
	||  spaceship->position.y > planet_y + planet_radius) {
        return false;
    }

    return true;
}

/**
 * @brief Updates the state of the game based on spaceship's position.
 * 
 * @param game A pointer to the Game instance.
 */
void Game_UpdateState(Game* game)
{
    Solar_system* solar_systems = NULL;
    int dx       = 0;
	int dy       = 0;
	int distance = 0;

	dx = game->spaceship->position.x - game->universe->finish.position.x;
	dy = game->spaceship->position.y - game->universe->finish.position.y;

	distance = sqrt(pow(dx, 2) + pow(dy, 2));
	if (distance <= 5) {
		game->state = GAME_STATE_OVER;
    }

	for (int i = 0; i < game->universe->nb_solar_systems; i++)
	{
        solar_systems = &game->universe->solar_systems[i];
        
        if (spaceship_planet_collision(game->spaceship,
        solar_systems->sun.position.x,
        solar_systems->sun.position.y,
        solar_systems->sun.property))
        {
            game->state = GAME_STATE_OVER;
        }

		for (int j = 0; j < game->universe->solar_systems[i].nb_planets; j++)
		{
            if (spaceship_planet_collision(game->spaceship,
            solar_systems->planets[j].position.x,
            solar_systems->planets[j].position.y,
            solar_systems->planets[j].property))
            {
                game->state = GAME_STATE_OVER;
            }
		}
	}
}

/**
 * @brief Generates a random initial angle for the spaceship.
 * 
 * @param spaceship A pointer to the Spaceship instance.
 */
void random_spaceship_angle(Spaceship* spaceship)
{	
    spaceship->angle = rand() % 181;
    spaceship->angle = spaceship->angle * M_PI / 180; 

	spaceship->velocity_vector.x = spaceship->velocity * cos(spaceship->angle);
	spaceship->velocity_vector.y = -spaceship->velocity * sin(spaceship->angle);
}

/**
 * @brief Frees the memory allocated for a Game instance.
 * 
 * @param game A pointer to the Game instance to free.
 */
void model_free_game(Game* game)
{
	model_free_universe(game->universe);
	free(game->spaceship);
	free(game);
}

/**
 * @brief Frees the memory allocated for a Universe instance.
 * 
 * @param universe A pointer to the Universe instance to free.
 */
void model_free_universe(Universe* universe)
{
	free(universe->solar_systems);
	free(universe);
}

/**
 * @brief Calculates the square of the distance between two points.
 * 
 * @param x1, y1 Coordinates of the first point.
 * @param x2, y2 Coordinates of the second point.
 * @return double Returns the square of the distance.
 */
double model_square_of_distance(int x1, int y1, int x2, int y2)
{
	return (double)((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

/**
 * @brief Calculates the magnitude of the gravity vector between two masses.
 * 
 * @param x1, y1 Coordinates and mass (m1) of the first object.
 * @param x2, y2 Coordinates and mass (m2) of the second object.
 * @return double Returns the magnitude of the gravity vector.
 */
double model_gravity_vector_magnitude(int x1, int y1, int m1, int x2, int y2, int m2)
{
	double G = 40000;
	
	return (double)(G * m1 * m2) / model_square_of_distance(x1, y1, x2, y2);
}

/**
 * @brief Rotates a 2D vector by an angle phi.
 * 
 * @param vector A pointer to the vector to rotate.
 * @param phi The angle to rotate the vector by.
 */
void model_rotation_of_phi_angle(V2_double* vector, double phi)
{
    double x = vector->x; 
    double y = vector->y;
    vector->x = x * cos(phi) - y * sin(phi); 
    vector->y = x * sin(phi) + y * cos(phi);
}