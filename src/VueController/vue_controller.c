/**
 * @file vue_controller.c
 * @brief Implementation of game rendering and update logic.
 */

#include "vue_controller.h"

SDL_Window*   window     = NULL;
SDL_Renderer* renderer   = NULL;

#define FRAME_TICKS (1000/60)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int  last_frame_tick      = 0;
bool v_pressed            = false;
bool SDLK_LEFT_pressed    = false;
bool SDLK_RIGHT_pressed   = false;
int  FPS                  = 0; 

/**
 * @brief Initialize the SDL window and renderer.
 * @param game A pointer to the Game structure.
 * @return true if initialization is successful, otherwise false.
 */
int init_window(Game* game)
{
    window = SDL_CreateWindow(
        "FPS: % d\n",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        game->universe->win_dimensions.x,
        game->universe->win_dimensions.y,
        SDL_WINDOW_SHOWN);

    if (!window)
    {
        printf("Window creation failed\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        printf("Renderer creation failed\n");
        return false;
    }

    return true;
}

/**
 * @brief Initial setup of game elements like spaceship, start, and finish positions.
 * @param game A pointer to the Game structure.
 */
void setup(Game* game)
{
    game->spaceship->dimensions.x= 10;
    game->spaceship->dimensions.y = 10;
    game->spaceship->position.x = game->universe->start.position.x;
    game->spaceship->position.y = game->universe->start.position.y;
    game->universe->start.dimensions.x = 10;
    game->universe->start.dimensions.y = 10;
    game->universe->finish.dimensions.x= 10;
    game->universe->finish.dimensions.y = 10;
    game->score = 0;
}

/**
 * @brief Process user inputs from the keyboard.
 * @param game A pointer to the Game structure.
 */
void process_input(Game* game)
{
    SDL_Event e;
    SDL_PollEvent(&e);

    switch (e.type)
    {
        case SDL_QUIT:
            game->state = GAME_STATE_OVER;
        break;

        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    game->state = GAME_STATE_OVER;
                break;

                case SDLK_SPACE:
                    if (game->state == GAME_STATE_IDLE)
                    {
                        game->state = GAME_STATE_IN_PROGRESS;
                        game->first_launch = true;
                    }
                break;

                case SDLK_LEFT:
                    if (game->state == GAME_STATE_IN_PROGRESS) // LEFT
                    {
                        game->spaceship->position.x -= 5;
                        SDLK_LEFT_pressed = true;
                        
                        game->score++;
                    }
                break;

                case SDLK_RIGHT:
                    if (game->state == GAME_STATE_IN_PROGRESS) // RIGHT
                    {
                        game->spaceship->position.x += 5; // supprime moi
                        SDLK_RIGHT_pressed = true;
                        
                        game->score++;
                    }
                break;

                case SDLK_v:
                    if (game->state == GAME_STATE_IN_PROGRESS) {
                        v_pressed = v_pressed == false ? true : false;
                    }
                break;

                default:
                break;
            }
        break;

        default:
        break;
    }
}

/**
 * @brief Update the positions of the planets in the solar systems.
 * @param planet A pointer to the Planet structure.
 * @param sun A pointer to the Star structure.
 * @param time The current time.
 */
void update_planets(Planet* planet, Star* sun, double time)
{
	double orbital_angular_velocity = 0;

	orbital_angular_velocity = 2 * (double)M_PI / planet->property;
	orbital_angular_velocity = (planet->orbit_radius >= 0) ? orbital_angular_velocity : -orbital_angular_velocity;

	planet->position.x = sun->position.x + abs(planet->orbit_radius) * cos(orbital_angular_velocity * time - (double)M_PI/2);
	planet->position.y = sun->position.y + abs(planet->orbit_radius) * sin(orbital_angular_velocity * time - (double)M_PI/2);
}

/**
 * @brief Update the game logic including planets, spaceship, and gravity.
 * @param game A pointer to the Game structure.
 */
void update(Game* game)
{
    int tick_curr     = SDL_GetTicks();
    int tick_delta    = (tick_curr - last_frame_tick);
    double time       = 0;
    double time_delta = 0;

    if (tick_delta <= FRAME_TICKS)
    {
        SDL_Delay(FRAME_TICKS - tick_delta);
    }

    tick_curr  = SDL_GetTicks();
    tick_delta = (tick_curr - last_frame_tick);
    time       = SDL_GetTicks() / 1000.0;
    time_delta = tick_delta / 1000.0;
    FPS        = 1/time_delta;

    for (int i = 0; i < game->universe->nb_solar_systems; i++)
    {
        for (int j = 0; j < game->universe->solar_systems[i].nb_planets; j++)
        {
            update_planets(&(game->universe->solar_systems[i].planets[j]), &(game->universe->solar_systems[i].sun), time);
        }
    }

    // Gravity logic

    V2_double acceleration = { 0 };
    V2 spaceship_position = { game->spaceship->position.x, game->spaceship->position.y };
    int index = 0;
    
    V2_double gravity = { 0 };
    double gravity_magnitude = 0;
    double vector_magnitude = 0;
    V2_double gravity_sum = { 0 };

	for (int i = 0; i < game->universe->nb_solar_systems; i++)
	{
        V2 sun_pos = { game->universe->solar_systems[i].sun.position.x, game->universe->solar_systems[i].sun.position.y };
		int sun_mass = game->universe->solar_systems[i].sun.property;
        
        gravity.x = sun_pos.x - spaceship_position.x;
        gravity.y = sun_pos.y - spaceship_position.y;
       
        gravity_magnitude = model_gravity_vector_magnitude(spaceship_position.x, spaceship_position.y, game->spaceship->mass, sun_pos.x, sun_pos.y, sun_mass);
        vector_magnitude = sqrt(model_square_of_distance(spaceship_position.x, spaceship_position.y, sun_pos.x, sun_pos.y));
        
        gravity.x = ((double)gravity.x / vector_magnitude) * gravity_magnitude;
        gravity.y = ((double)gravity.y / vector_magnitude) * gravity_magnitude;

		gravity_sum.x += gravity.x;
		gravity_sum.y += gravity.y;

        game->spaceship->gravity_vectors[index].x = (int)gravity.x;
        game->spaceship->gravity_vectors[index].y = (int)gravity.y;
        index++;
        
		
		for (int j = 0; j < game->universe->solar_systems[i].nb_planets; j++)
		{	
			V2 planet_pos = { game->universe->solar_systems[i].planets[j].position.x, game->universe->solar_systems[i].planets[j].position.y };
			int planet_mass = game->universe->solar_systems[i].planets[j].property; 

            gravity.x = planet_pos.x - spaceship_position.x;
            gravity.y = planet_pos.y - spaceship_position.y;
           
			gravity_magnitude = model_gravity_vector_magnitude(spaceship_position.x, spaceship_position.y, game->spaceship->mass, planet_pos.x, planet_pos.y, planet_mass);
			vector_magnitude = sqrt(model_square_of_distance(spaceship_position.x, spaceship_position.y, planet_pos.x, planet_pos.y));
            
            gravity.x = ((double)gravity.x / vector_magnitude) * gravity_magnitude;
            gravity.y = ((double)gravity.y / vector_magnitude) * gravity_magnitude;

		    gravity_sum.x += gravity.x;
		    gravity_sum.y += gravity.y;

            game->spaceship->gravity_vectors[index].x = (int)gravity.x;
            game->spaceship->gravity_vectors[index].y = (int)gravity.y;
            index++;

		}

	}

    game->gravity_vector.x = (int)gravity_sum.x;
    game->gravity_vector.y = (int)gravity_sum.y;

    
    if (game->state == GAME_STATE_IN_PROGRESS) {

     if  (game->first_launch==true)
        {
            game->spaceship->position.x = game->spaceship->position.x + game->spaceship->velocity_vector.x *time_delta; 
            game->spaceship->position.y = game->spaceship->position.y - game->spaceship->velocity_vector.y *time_delta;

            game->first_launch=false;
        }

        acceleration.x = gravity_sum.x / game->spaceship->mass;
        acceleration.y = gravity_sum.y / game->spaceship->mass;
        
        game->spaceship->position.x = game->spaceship->position.x + game->spaceship->velocity_vector.x * time_delta +
        0.5 * acceleration.x * time_delta * time_delta;

	    game->spaceship->position.y = game->spaceship->position.y + game->spaceship->velocity_vector.y * time_delta +
        0.5 * acceleration.y * time_delta * time_delta;


        V2_double new_velocity = { 0 };

        new_velocity.x = game->spaceship->velocity_vector.x + acceleration.x * time_delta;
        new_velocity.y = game->spaceship->velocity_vector.y + acceleration.y * time_delta;

        double new_velocity_magnitude = sqrt(new_velocity.x * new_velocity.x + new_velocity.y * new_velocity.y);
        if (new_velocity_magnitude < 0.1) {
            new_velocity_magnitude = 0.1;
        }

        if(new_velocity_magnitude < (game->spaceship->velocity / 2)) 
        {
            game->spaceship->velocity_vector.x = (game->spaceship->velocity / 2) * (new_velocity.x / new_velocity_magnitude);
            game->spaceship->velocity_vector.y = (game->spaceship->velocity / 2) * (new_velocity.y / new_velocity_magnitude);
        }
        else if (new_velocity_magnitude > (2 * (game->spaceship->velocity)))
        {
            game->spaceship->velocity_vector.x = 2 * (game->spaceship->velocity) * (new_velocity.x / new_velocity_magnitude);
            game->spaceship->velocity_vector.y = 2 * (game->spaceship->velocity) * (new_velocity.y / new_velocity_magnitude);
        }
        else
        {
            game->spaceship->velocity_vector.x = new_velocity.x;
            game->spaceship->velocity_vector.y = new_velocity.y;
        }
    }
     
    // End gravity logic

    if (game->spaceship->position.x > game->universe->win_dimensions.x - 20) game->spaceship->position.x = 10;
    if (game->spaceship->position.x < 10) game->spaceship->position.x = game->universe->win_dimensions.x - 20;
    if (game->spaceship->position.y > game->universe->win_dimensions.y - 20) game->spaceship->position.y = 10;
    if (game->spaceship->position.y < 10) game->spaceship->position.y = game->universe->win_dimensions.y - 20;

    last_frame_tick = SDL_GetTicks();
}

/**
 * @brief Render the game elements on the window.
 * @param game A pointer to the Game structure.
 */
void render(Game* game)
{   
    char window_title[50] = { 0 };

    // Set black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Update window title window with FPS and score 
    sprintf(window_title, "ProjetC_oleil | FPS : %d | Score : %d", FPS, game->score);
    SDL_SetWindowTitle(window, window_title);

    // Rectangle start gate
    rectangleRGBA(renderer, game->universe->start.position.x, game->universe->start.position.y, game->universe->start.position.x + game->universe->start.dimensions.x, game->universe->start.position.y + game->universe->start.dimensions.y, 255, 255, 255, 255);
    
    // Rectangle finish gate
    rectangleRGBA(renderer, game->universe->finish.position.x, game->universe->finish.position.y, game->universe->finish.position.x + game->universe->finish.dimensions.y, game->universe->finish.position.y + game->universe->finish.dimensions.x, 255, 255, 255, 255);
    
    // Rectangle window
    rectangleRGBA(renderer, 10, 10, game->universe->win_dimensions.x -10, game->universe->win_dimensions.y-10, 255, 255, 255, 255);

    // Rectangle spaceship
    boxRGBA(renderer, game->spaceship->position.x, game->spaceship->position.y, game->spaceship->position.x + game->spaceship->dimensions.x, game->spaceship->position.y + game->spaceship->dimensions.y, 255, 0, 0, 255);

    for (int i = 0; i < game->universe->nb_solar_systems; i++)
    {
        filledCircleRGBA(renderer, game->universe->solar_systems[i].sun.position.x, game->universe->solar_systems[i].sun.position.y, game->universe->solar_systems[i].sun.property, 255, 215, 0, 255);
        
        for (int j = 0; j < game->universe->solar_systems[i].nb_planets; j++)
        {
            // Draw planets
            filledCircleRGBA(renderer, game->universe->solar_systems[i].planets[j].position.x, game->universe->solar_systems[i].planets[j].position.y, game->universe->solar_systems[i].planets[j].property, 0, 10, 255, 255);

            // Draw orbits
            circleRGBA(renderer, game->universe->solar_systems[i].sun.position.x, game->universe->solar_systems[i].sun.position.y, abs(game->universe->solar_systems[i].planets[j].orbit_radius), 255, 255, 255, 50);
        }
    }

    // Set magenta color for vectors
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);

    // Display all vectors being currently applied to the spaceship
    if(v_pressed == true){
        for(int i = 0;i < game->universe->nb_stellar_bodies; i++)
        {
            SDL_RenderDrawLine(renderer,  game->spaceship->position.x + 5, game->spaceship->position.y + 5, 
            game->spaceship->position.x + 10*game->spaceship->gravity_vectors[i].x, 
            game->spaceship->position.y + 10*game->spaceship->gravity_vectors[i].y);
        }
    }
    
    SDL_RenderPresent(renderer);
}

/**
 * @brief Destroy the SDL window and renderer and quit SDL.
 */
void destroy_window()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
