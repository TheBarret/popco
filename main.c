#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"
#include "stats.h"
#include "render.h"
#include "crt_effects.h"

#define WINDOW_TITLE  "PopCo Monitor"
#define CONFIG_PATH   "popco.conf"

static TTF_Font *load_font(const Config *cfg) {
    TTF_Font *font = TTF_OpenFont(cfg->font_path, cfg->font_size);
    if (font) return font;

    // Fall back
    static const char *fallbacks[] = {
        "/usr/share/fonts/truetype/freefont/FreeMono.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf"
    };
    for (int i = 0; i < 2; i++) {
        font = TTF_OpenFont(fallbacks[i], cfg->font_size);
        if (font) {
            printf("Font '%s' not found, falling back to %s\n", cfg->font_path, fallbacks[i]);
            return font;
        }
    }
    return NULL;
}

int main(void) {
    Config cfg;
    config_set_defaults(&cfg);
    if (!config_load(CONFIG_PATH, &cfg)) {
        printf("No %s found, using defaults (and writing one out)\n", CONFIG_PATH);
        config_save(CONFIG_PATH, &cfg);
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF Init Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = load_font(&cfg);
    if (!font) {
        fprintf(stderr, "Could not load any font!\n");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Uint32 windowFlags = SDL_WINDOW_SHOWN;
    if (cfg.resizable)     windowFlags |= SDL_WINDOW_RESIZABLE;
    if (cfg.borderless)    windowFlags |= SDL_WINDOW_BORDERLESS;
    if (cfg.always_on_top) windowFlags |= SDL_WINDOW_ALWAYS_ON_TOP;

    int startX = cfg.pos_x >= 0 ? cfg.pos_x : (int)SDL_WINDOWPOS_CENTERED;
    int startY = cfg.pos_y >= 0 ? cfg.pos_y : (int)SDL_WINDOWPOS_CENTERED;

    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE, startX, startY, cfg.window_width, cfg.window_height, windowFlags);
    if (!window) {
        fprintf(stderr, "Window Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetLogicalSize(renderer, cfg.window_width, cfg.window_height);

    if (cfg.opacity < 1.0f) {
        SDL_SetWindowOpacity(window, cfg.opacity);
    }

    CrtEffects crt = {0};
    crt_effects_create(&crt, renderer, cfg.window_width, cfg.window_height, &cfg);

    SystemStats stats = {0};
    bool running = true;
    SDL_Event event;
    Uint32 lastUpdate = 0;
    Uint32 startTime = SDL_GetTicks();
    srand(startTime);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_r:
                        if (config_load(CONFIG_PATH, &cfg)) {
                            SDL_RenderSetLogicalSize(renderer, cfg.window_width, cfg.window_height);
                            crt_effects_destroy(&crt);
                            crt_effects_create(&crt, renderer, cfg.window_width, cfg.window_height, &cfg);
                            printf("Reloaded %s\n", CONFIG_PATH);
                        }
                        break;
                }
            }
        }

        Uint32 now = SDL_GetTicks();
        if (now - lastUpdate > (Uint32)cfg.refresh_ms) {
            stats_sample(&stats);
            lastUpdate = now;
        }

        SDL_SetRenderTarget(renderer, crt.screenBuffer);
        render_ui(renderer, font, &stats, &cfg, now);

        SDL_SetRenderTarget(renderer, NULL);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        Uint32 elapsed = SDL_GetTicks() - startTime;
        crt_effects_apply(&crt, renderer, &cfg, elapsed);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // Persist window position for next launch.
    if (!cfg.borderless) {
        SDL_GetWindowPosition(window, &cfg.pos_x, &cfg.pos_y);
        config_save(CONFIG_PATH, &cfg);
    }

    crt_effects_destroy(&crt);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
