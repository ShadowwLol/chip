#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../include/def.h"
#include "../include/logs.h"
#include <SDL2/SDL_mixer.h>
#include <dirent.h>


#if __WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
#else
int main(void){
#endif
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO) != 0)
    {
        log(ERR, "Failed initializing SDL : %s", SDL_GetError());
        SDL_Quit();
    }else{
        log(SUCCESS, "Successfully initialized SDL");
    }

    SDL_Window* win = SDL_CreateWindow(WINDOW_TITLE,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!win){
        log(ERR, "Failed creating window : %s", SDL_GetError());
        SDL_Quit();
    }else{
        log(SUCCESS, "Successfully created window");
    }

    // create a renderer, which sets up the graphics hardware
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (!rend){
        log(ERR, "Failed creating renderer : %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
    }else{
        log(SUCCESS, "Successfully created renderer");
    }

    if(SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND) != 0){
        log(ERR, "Failed configuring renderer blend mode : %s", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }else{
        log(SUCCESS, "Successfully configured renderer blend mode");
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) != 0){
        log(ERR, "Failed initializing SDL_MIXER : %s", Mix_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
    }else{
        log(SUCCESS, "Successfully initialized SDL_MIXER");
    }

    int current_display = SDL_GetWindowDisplayIndex(win);
    SDL_DisplayMode current_dp;
    if (0 != SDL_GetCurrentDisplayMode(current_display, &current_dp)){
        log(ERR, "Failed getting display mode for video display #%d : %s", current_display, SDL_GetError());
    }else{log(INFO, "Display #%d: [%dx%dpx] @ %dhz", current_display, current_dp.w, current_dp.h, current_dp.refresh_rate);}

    SDL_SetWindowMinimumSize(win, (current_dp.w/2), (current_dp.h/2));
    SDL_SetWindowMaximumSize(win, current_dp.w, current_dp.h);
    SDL_RenderSetLogicalSize(rend, WINDOW_WIDTH, WINDOW_HEIGHT);

    log(INFO, "Target frame time : [%.2fms]", 1000.0f/FPS);

    SDL_RaiseWindow(win);


    // UI
    int close_requested = 0;

    // load the image into memory
    /* SDL_Texture * texture = IMG_LoadTexture(rend, "resources/image.png"); */

    /* POS */
    Mix_Music * music = NULL;
    Vector2 music_pos = {WINDOW_WIDTH/2, WINDOW_HEIGHT/2};
    SDL_Rect background = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    /* * * */

    // Fonts
    FC_Font* font = FC_CreateFont();
    FC_LoadFont(font, rend, "resources/bee.ttf", 20, FC_MakeColor(0,255,160,255), TTF_STYLE_NORMAL);

    /* Find music files  */
    char P[1024] = "/home/shadoww/Music/"; // Your own music path
    int n=0, i=0;\
	DIR *d;\
	struct dirent *dir;\
	d = opendir(P);\
	while((dir = readdir(d)) != NULL) {\
		if ( !strncmp(dir->d_name, ".", 1) || !strncmp(dir->d_name, "..", 2) )\
		{\
		} else {\
			n++;\
		}\
	}\
	rewinddir(d);\
	if (n < 1){ return EXIT_FAILURE; }
	char *files[n];
	while((dir = readdir(d)) != NULL) {
		if ( !strncmp(dir->d_name, ".", 1) || !strncmp(dir->d_name, "..", 2) )
		{}
		else {
			files[i]= dir->d_name;\
			i++;
		}
	}
	rewinddir(d);
	
    /* * * * * * * * * * */

    // Fps timing
    clock_t startm, stopm;

    double beforeTime = (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
    float current_fps = -0.0;
    float time_since_last_fps = 0.0;

    // animation loop
    while (!close_requested)
    {
        //Start fps timer
        if ( (startm = clock()) == -1){err("Failed starting fps timer.");}

        // process events
        SDL_Event event;
        
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    close_requested = 1;
                    break;
                
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_ESCAPE:
                            close_requested = 1;
                            break;

                        case SDL_SCANCODE_F11:
                            ToggleFullscreen(win);
                            FC_LoadFont(font, rend, "resources/bee.ttf", 20, FC_MakeColor(0,255,160,255), TTF_STYLE_NORMAL);
                            break;
                        
                        case SDL_SCANCODE_SPACE:
                            if (Mix_PausedMusic() == 0){
                                Mix_PauseMusic();
                            }else{
                                Mix_ResumeMusic();
                            }
                            break;
                        
                        default:
                            break;
                    }
                    break;

                case SDL_WINDOWEVENT:
                    switch(event.window.event){
                        case SDL_WINDOWEVENT_RESIZED:
                            FC_LoadFont(font, rend, "resources/bee.ttf", 20, FC_MakeColor(0,255,160,255), TTF_STYLE_NORMAL);
                            break;

                        case SDL_WINDOWEVENT_FOCUS_GAINED:
                            FC_LoadFont(font, rend, "resources/bee.ttf", 20, FC_MakeColor(0,255,160,255), TTF_STYLE_NORMAL);
                            break;
                        
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        /* Fps and delta */
        double currentTime = (double)SDL_GetPerformanceCounter() / SDL_GetPerformanceFrequency();
        float deltaTime = (float)(currentTime - beforeTime);
        beforeTime = currentTime; // Prime beforeTime for the next frame

        time_since_last_fps += deltaTime;
        if (time_since_last_fps >= 0.25F || current_fps <= 0.0F) {
            time_since_last_fps = 0.0F;
            current_fps = 1.0 / deltaTime;
        }
        /* * * * * * * * */

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        /* Drawing */

        // clear the window
        SDL_RenderClear(rend);       
        SDL_SetRenderDrawColor(rend, 20, 20, 200, 255);
        SDL_RenderFillRect(rend, &background);

        for(i=0; i<n; i++){
            render_button(rend, win, font, 20, (music_pos.x), (music_pos.y), 255, 0, files[i]);
            if (ClickedText(rend, win, font, files[i], music_pos.x, music_pos.y) == 1){ // Doesn't work well
                /* Play music */
                char music_path[2048] = "\0";
                strcpy(music_path, P);
                strcat(music_path, files[i]);
                music = Mix_LoadMUS(music_path);
                DEBUG;
                if (!music){
                    log(ERR, "Failed loading {%s} music file", music_path);
                    close_requested = 1;
                }else{
                    log(SUCCESS, "Successfully loaded {%s} music file", music_path);
                }
                if (Mix_PlayingMusic() == 0){
                    log(SUCCESS, "Now Playing : {%s}", music_path);
                    Mix_PlayMusic(music, 0); // custom loops?
                }else{
                    if (Mix_PausedMusic() == 0){
                        Mix_PauseMusic();
                    }else{
                        Mix_ResumeMusic();
                    }
                }
            }
		    //printf("%d. [%s]\n", i+1, files[i]);
            music_pos.y += 40;
	    }
        music_pos.x = (WINDOW_WIDTH/2);
        music_pos.y = (WINDOW_HEIGHT/2);

        /* * * * * */

        // wait 1/60th of a second
        if ( (stopm = clock()) == -1){err("Failed ending fps timer.");}

        SDL_Delay((1000.0/FPS)-(((double)stopm-startm)/CLOCKS_PER_SEC));
        FC_Draw(font, rend, 0, 0, "[%.0fFPS]", current_fps);

        SDL_RenderPresent(rend);
    }

    // clean up resources before exiting
    printf("Quitting..\n");
    if (music != NULL){Mix_FreeMusic(music);}
    FC_FreeFont(font);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    Mix_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}