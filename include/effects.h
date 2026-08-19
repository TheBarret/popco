#ifndef EFFECTS_H
#define EFFECTS_H

#include <SDL2/SDL.h>
#include "config.h"

typedef struct {
    SDL_Texture *screenBuffer;
    SDL_Texture *scanlineOverlay;
    SDL_Texture *vignetteTexture;
    int width, height; // logical resolution these were baked for
} CrtEffects;

// Creates the offscreen render targets sized to (width, height),
// which should be the SDL logical resolution (see SDL_RenderSetLogicalSize),
// NOT the raw window pixel size
void crt_effects_create(CrtEffects *crt, SDL_Renderer *renderer, int width, int height,
                         const Config *cfg);

void crt_effects_destroy(CrtEffects *crt);

// Composites crt->screenBuffer (which the caller must have already rendered the UI into)
// onto the current render target with glow, scanlines, vignette, flicker,
// roll bar and chromatic aberration, each individually toggleable via cfg.
void crt_effects_apply(CrtEffects *crt, SDL_Renderer *renderer, const Config *cfg, Uint32 elapsed_ms);

#endif
