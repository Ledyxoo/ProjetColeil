/**
 * @file main.c
 * @brief Compile using: gcc -Wall -O2 -o ProjetC_oleil.exe main.c ./model/model.c ./vue_controller/vue_controller.c -lm -lSDL2 -lSDL2_gfx
 */
#include "Model/model.h"
#include "VueController/vue_controller.h"

int main(int argc, char* argv[])
{
    Game* game = 0;
    bool random_spaceship_initialized = false;
    const char* config_path = "Assets/config.txt";

    // Si un argument est fourni, l'utiliser comme chemin de config
    if (argc > 1) {
        config_path = argv[1];
    }

    srand(0);

    game = model_game_new((char*)config_path);
    if (game == NULL) {
        fprintf(stderr, "Error: Failed to load game from config file: %s\n", config_path);
        return -1;
    }
    
    SDL_Init(SDL_INIT_VIDEO);
    init_window(game);
    setup(game);

    while (game->state != GAME_STATE_OVER)
    {
        process_input(game);
        update(game);
        render(game);

        Game_UpdateState(game);
        
        
        if (game->state == GAME_STATE_IN_PROGRESS && !random_spaceship_initialized)
        {
            random_spaceship_initialized = true;
        }
        
    }
    
    model_free_game(game);
    destroy_window();

    return 0;
}
