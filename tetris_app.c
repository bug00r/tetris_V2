#include "tetris_app.h"

/* START Private Section */

static bool __tetris_app_init() {
	
	/* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
		return false;
	}
	
	/* init font sys */
	if (TTF_Init() < 0) {
		printf("Font opening error : %s\n",TTF_GetError()); 
		return false;
	}
	
	/* init sound sys */
	if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) { 
		printf("Mix_Init: Failed to init required ogg support!\nMix_Init: %s\n", Mix_GetError());
		return false;
	}

	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
		printf("IMG_Init: Failed to init required jpg and png support!\nIMG_Init: %s\n", IMG_GetError());
		return false;
	}
	
	return true;
}

void __tetris_cleanup_sdl() {
	TTF_Quit();
	Mix_CloseAudio();
	IMG_Quit();
    SDL_Quit();
}

/* EO Private Section */

/* START Public Section */

bool tetris_app_init(tetris_app_t *app, int argc, char **argv) {
	bool init_successfully = true;
	if ( app != NULL ) {
		srand(time(NULL));
		app->state.done = false;
		app->argc = argc;
		app->argv = argv;
		
		init_successfully &= __tetris_app_init();
		init_successfully &= tetris_game_init(&app->game);
		init_successfully &= tetris_input_init(&app->input);
		init_successfully &= tetris_gfx_init(&app->ui.gfx);
		init_successfully &= tetris_ui_init(&app->ui, &app->game);
		init_successfully &= init_tetris_sfx(&app->sfx);
		
	}
	return init_successfully;
}

void
loop(tetris_app_t * tetris_app)
{

    SDL_Event e;
	if (SDL_PollEvent(&e)) {
		
		tetris_game_t *game = &tetris_app->game;
		tetris_input_t *input = &tetris_app->input;
		tetris_ui_t *ui = &tetris_app->ui;
		tetris_sfx_t *sfx = &tetris_app->sfx;
		
		tetris_input_update(input, &e);

		tetris_game_update(game, input);
		
		tetris_ui_update(ui, game);
		
		tetris_sfx_update(sfx, game);
		
		/* here we should add frame sheduler to 60fps */
		SDL_UpdateWindowSurface(tetris_app->ui.window);
		
		/* finish game */
		if (game->state.quit) {
            tetris_app->state.done = true;
            return;
        }	
					
    }
	

	
}

void tetris_app_run(tetris_app_t *app) {
	
	while (app->state.done == false) {
        loop(app);
		SDL_Delay(1); /* wait 1ms to reduce cpu usage. Here we should use another kind of logic with animations */
    }
}


void tetris_app_cleanup(tetris_app_t *app) {
	tetris_game_cleanup(&app->game);
	__tetris_cleanup_sdl();
}

/* EO Public Section */