#include "effects.h"
#include <math.h>
#include <stdlib.h>

void crt_effects_create(CrtEffects *crt, SDL_Renderer *renderer, int width, int height,
                         const Config *cfg) {
    crt->width = width;
    crt->height = height;

    crt->screenBuffer = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

    crt->scanlineOverlay = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(crt->scanlineOverlay, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, crt->scanlineOverlay);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    for (int y = 0; y < height; y += 4) {
        SDL_RenderDrawLine(renderer, 0, y, width, y);
        SDL_RenderDrawLine(renderer, 0, y + 1, width, y + 1);
    }

    SDL_SetRenderDrawColor(renderer, cfg->amber_r, cfg->amber_g, cfg->amber_b, 8);
    for (int y = 2; y < height; y += 4) {
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }

    SDL_SetRenderTarget(renderer, NULL);

    crt->vignetteTexture = SDL_CreateTexture(
        renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    SDL_SetTextureBlendMode(crt->vignetteTexture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderTarget(renderer, crt->vignetteTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int edge = (int)(20.0f * (width / 640.0f));
    if (edge < 4) edge = 4;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
    SDL_Rect r = {0, 0, width, edge};
    SDL_RenderFillRect(renderer, &r);
    r = (SDL_Rect){0, height - edge, width, edge};
    SDL_RenderFillRect(renderer, &r);
    r = (SDL_Rect){0, 0, edge, height};
    SDL_RenderFillRect(renderer, &r);
    r = (SDL_Rect){width - edge, 0, edge, height};
    SDL_RenderFillRect(renderer, &r);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 40);
    SDL_Rect inner = {edge, edge, width - 2 * edge, height - 2 * edge};
    SDL_RenderDrawRect(renderer, &inner);

    SDL_SetRenderTarget(renderer, NULL);
}

void crt_effects_destroy(CrtEffects *crt) {
    if (crt->screenBuffer) SDL_DestroyTexture(crt->screenBuffer);
    if (crt->scanlineOverlay) SDL_DestroyTexture(crt->scanlineOverlay);
    if (crt->vignetteTexture) SDL_DestroyTexture(crt->vignetteTexture);
    crt->screenBuffer = crt->scanlineOverlay = crt->vignetteTexture = NULL;
}

void crt_effects_apply(CrtEffects *crt, SDL_Renderer *renderer, const Config *cfg, Uint32 elapsed_ms) {
    int w = crt->width, h = crt->height;

    // Phosphor glow / bloom
    if (cfg->enable_glow) {
        SDL_SetTextureBlendMode(crt->screenBuffer, SDL_BLENDMODE_ADD);

        SDL_SetTextureAlphaMod(crt->screenBuffer, 35);
        SDL_Rect glowRect = {-6, -6, w + 12, h + 12};
        SDL_RenderCopy(renderer, crt->screenBuffer, NULL, &glowRect);

        SDL_SetTextureAlphaMod(crt->screenBuffer, 20);
        SDL_Rect glowRect2 = {-3, -3, w + 6, h + 6};
        SDL_RenderCopy(renderer, crt->screenBuffer, NULL, &glowRect2);
    }

    // Main screen, with optional flicker
    int flicker = 255;
    if (cfg->enable_flicker) {
        flicker = 230 + (rand() % 25);
        float wave = sinf(elapsed_ms * 0.001f) * 8.0f;
        flicker += (int)wave;
        flicker = flicker > 255 ? 255 : (flicker < 200 ? 200 : flicker);
    }
    SDL_SetTextureBlendMode(crt->screenBuffer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(crt->screenBuffer, (Uint8)flicker);
    SDL_RenderCopy(renderer, crt->screenBuffer, NULL, NULL);

    // Scanlines
    if (cfg->enable_scanlines) {
        SDL_RenderCopy(renderer, crt->scanlineOverlay, NULL, NULL);
    }

    // Vignette
    if (cfg->enable_vignette) {
        SDL_RenderCopy(renderer, crt->vignetteTexture, NULL, NULL);
    }

    // Moving refresh / roll bar
    if (cfg->enable_flicker) {
        int rollY = (elapsed_ms / 8) % h;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(renderer, cfg->amber_r, cfg->amber_g, cfg->amber_b, 12);
        SDL_Rect rollRect = {0, rollY, w, 20};
        SDL_RenderFillRect(renderer, &rollRect);

        SDL_SetRenderDrawColor(renderer, cfg->amber_r, cfg->amber_g, cfg->amber_b, 25);
        SDL_Rect rollCenter = {0, rollY + 8, w, 4};
        SDL_RenderFillRect(renderer, &rollCenter);
    }

    // Chromatic aberration on edges
    if (cfg->enable_chromatic_aberration) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        SDL_SetRenderDrawColor(renderer, cfg->amber_r, 0, 0, 5);
        SDL_Rect ab = {2, 0, 10, h};
        SDL_RenderFillRect(renderer, &ab);
        SDL_SetRenderDrawColor(renderer, 0, cfg->amber_g, cfg->amber_b, 5);
        ab = (SDL_Rect){w - 12, 0, 10, h};
        SDL_RenderFillRect(renderer, &ab);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}
