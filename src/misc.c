#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>
#include "../include/def.h"
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_mouse.h>

void ToggleFullscreen(SDL_Window* Window){
    Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP; //SDL_WINDOW_FULLSCREEN;
    int IsFullscreen = SDL_GetWindowFlags(Window) & FullscreenFlag;
    SDL_SetWindowResizable(Window, IsFullscreen ? 0 : 1);
    SDL_SetWindowFullscreen(Window, IsFullscreen ? 0 : FullscreenFlag); // SDL_FULLSCREEN_DESKTOP FOR FAKE FULLSCREEN?«
    !IsFullscreen ? SDL_SetWindowBordered(Window, 0) : (SDL_SetWindowBordered(Window, 1), SDL_SetWindowSize(Window, WINDOW_WIDTH, WINDOW_HEIGHT));
    SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowCursor(1);
}

char * substr(char *destination, const char *source, int from, int len){
    while (len > 0){
        *destination = *(source + from);
        destination++;
        source++;
        len--;
    }
    *destination = '\0';
    return destination;
}
 