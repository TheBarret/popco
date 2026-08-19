#include "render.h"
#include <stdio.h>

#define BASE_W 640.0f
#define BASE_H 480.0f

void draw_text(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_progress_bar(SDL_Renderer *renderer, int x, int y, int width, int height,
                        float percentage, SDL_Color color) {
    // Background
    SDL_SetRenderDrawColor(renderer, 20, 15, 0, 255);
    SDL_Rect bg = {x, y, width, height};
    SDL_RenderFillRect(renderer, &bg);

    // Outer border
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 200);
    SDL_RenderDrawRect(renderer, &bg);

    // Inner border
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 100);
    SDL_Rect innerBorder = {x + 1, y + 1, width - 2, height - 2};
    SDL_RenderDrawRect(renderer, &innerBorder);

    int fillWidth = (int)(width * (percentage / 100.0f));
    if (fillWidth > 4) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 200);
        SDL_Rect fill = {x + 2, y + 2, fillWidth - 4, height - 4};
        SDL_RenderFillRect(renderer, &fill);

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 100);
        SDL_Rect glow = {x + 4, y + 4, fillWidth - 8, height - 8};
        SDL_RenderFillRect(renderer, &glow);
    }
}

void render_ui(SDL_Renderer *renderer, TTF_Font *font, const SystemStats *stats,
               const Config *cfg, Uint32 now_ms) {
    float sx = cfg->window_width  / BASE_W;
    float sy = cfg->window_height / BASE_H;
#define X(px) ((int)((px) * sx))
#define Y(px) ((int)((px) * sy))
#define W(px) ((int)((px) * sx))
#define H(px) ((int)((px) * sy))

    SDL_Color amber  = {(Uint8)cfg->amber_r, (Uint8)cfg->amber_g, (Uint8)cfg->amber_b, 255};
    SDL_Color green  = {100, 255, 100, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color red    = {255, 100, 100, 255};

    SDL_SetRenderDrawColor(renderer, 8, 5, 0, 255);
    SDL_RenderClear(renderer);

    draw_text(renderer, font, "= = = PopCo Industries Monitor = = =", X(20), Y(10), amber);
    draw_text(renderer, font, "Vault-Tec Certified", X(20), Y(30), amber);
    draw_text(renderer, font, stats->hostname, X(450), Y(30), amber);

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "Uptime: %ldh %ldm", stats->uptime / 3600, (stats->uptime % 3600) / 60);
    draw_text(renderer, font, buffer, X(20), Y(60), amber);

    SDL_SetRenderDrawColor(renderer, amber.r, amber.g, amber.b, 100);
    SDL_RenderDrawLine(renderer, X(20), Y(75), X(620), Y(75));

    // CPU
    draw_text(renderer, font, ">> CPU PROCESSOR UNIT", X(30), Y(95), amber);
    draw_text(renderer, font, "Status:", X(30), Y(115), amber);
    snprintf(buffer, sizeof(buffer), "%5.1f%%", stats->cpu_usage);
    draw_text(renderer, font, buffer, X(120), Y(115), stats->cpu_usage > 80 ? red : amber);
    draw_progress_bar(renderer, X(30), Y(130), W(400), H(20), stats->cpu_usage,
                       stats->cpu_usage > 80 ? red : (stats->cpu_usage > 50 ? yellow : green));

    // Memory
    draw_text(renderer, font, ">> MEMORY ALLOCATION", X(30), Y(175), amber);
    draw_text(renderer, font, "Status:", X(30), Y(195), amber);
    snprintf(buffer, sizeof(buffer), "%5.1f%%", stats->memory_usage);
    draw_text(renderer, font, buffer, X(120), Y(195), stats->memory_usage > 80 ? red : amber);
    draw_progress_bar(renderer, X(30), Y(210), W(400), H(20), stats->memory_usage,
                       stats->memory_usage > 80 ? red : (stats->memory_usage > 50 ? yellow : green));

    // Disk
    draw_text(renderer, font, ">> STORAGE ARRAY", X(30), Y(255), amber);
    draw_text(renderer, font, "Status:", X(30), Y(275), amber);
    snprintf(buffer, sizeof(buffer), "%5.1f%%", stats->disk_usage);
    draw_text(renderer, font, buffer, X(120), Y(275), stats->disk_usage > 80 ? red : amber);
    draw_progress_bar(renderer, X(30), Y(290), W(400), H(20), stats->disk_usage,
                       stats->disk_usage > 80 ? red : (stats->disk_usage > 50 ? yellow : green));

    // Status log
    //draw_text(renderer, font, ">> SYSTEM STATUS", X(30), Y(335), amber);
    //draw_text(renderer, font, "[ OK ] All systems operational", X(30), Y(355), green);
    //draw_text(renderer, font, "[ OK ] Vault-Tec systems nominal", X(30), Y(375), green);

    // Rotating flavor text
    static const char *messages[] = {
        "Linux. Linux never changes.",
        "Please stand by...",
        "Vault-Tec: Preparing for the future.",
        "PopCo: Building a better tomorrow."
    };
    int msgIndex = (now_ms / 5000) % 4;
    draw_text(renderer, font, messages[msgIndex], X(30), Y(405), amber);

    snprintf(buffer, sizeof(buffer), "v2.0.%03d", (int)(now_ms / 10000) % 1000);
    draw_text(renderer, font, buffer, X(540), Y(460), amber);

#undef X
#undef Y
#undef W
#undef H
}
