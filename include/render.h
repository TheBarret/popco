#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "stats.h"
#include "config.h"

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color);

void draw_progress_bar(SDL_Renderer *renderer, int x, int y, int width, int height,
                        float percentage, SDL_Color color);

void render_ui(SDL_Renderer *renderer, TTF_Font *font, const SystemStats *stats,
                const Config *cfg, Uint32 now_ms);

#endif
