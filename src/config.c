#define _DEFAULT_SOURCE
#include "../include/config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h> // strcasecmp

void config_set_defaults(Config *cfg) {
    cfg->window_width  = 640;
    cfg->window_height = 480;
    cfg->resizable      = true;
    cfg->borderless     = false;
    cfg->always_on_top  = false;
    cfg->pos_x = -1;
    cfg->pos_y = -1;
    cfg->opacity = 1.0f;

    cfg->refresh_ms = 1000;

    strncpy(cfg->font_path, "./fonts/ShareTechMono-Regular.ttf", sizeof(cfg->font_path) - 1);
    cfg->font_path[sizeof(cfg->font_path) - 1] = '\0';
    cfg->font_size = 14;

    cfg->amber_r = 255;
    cfg->amber_g = 176;
    cfg->amber_b = 0;

    cfg->enable_scanlines            = true;
    cfg->enable_vignette             = true;
    cfg->enable_glow                 = true;
    cfg->enable_flicker              = true;
    cfg->enable_chromatic_aberration = true;
}

static char *trim(char *s) {
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

static bool parse_bool(const char *v) {
    return (strcasecmp(v, "true") == 0 || strcasecmp(v, "1") == 0 ||
            strcasecmp(v, "yes") == 0  || strcasecmp(v, "on") == 0);
}

bool config_load(const char *path, Config *cfg) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return false; // caller keeps defaults
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        char *l = trim(line);
        if (l[0] == '\0' || l[0] == '#' || l[0] == ';') continue;

        char *eq = strchr(l, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = trim(l);
        char *val = trim(eq + 1);

        if      (strcmp(key, "window_width") == 0)  cfg->window_width  = atoi(val);
        else if (strcmp(key, "window_height") == 0) cfg->window_height = atoi(val);
        else if (strcmp(key, "resizable") == 0)      cfg->resizable      = parse_bool(val);
        else if (strcmp(key, "borderless") == 0)     cfg->borderless     = parse_bool(val);
        else if (strcmp(key, "always_on_top") == 0)  cfg->always_on_top  = parse_bool(val);
        else if (strcmp(key, "pos_x") == 0)          cfg->pos_x = atoi(val);
        else if (strcmp(key, "pos_y") == 0)          cfg->pos_y = atoi(val);
        else if (strcmp(key, "opacity") == 0)        cfg->opacity = (float)atof(val);
        else if (strcmp(key, "refresh_ms") == 0)     cfg->refresh_ms = atoi(val);
        else if (strcmp(key, "font_path") == 0) {
            strncpy(cfg->font_path, val, sizeof(cfg->font_path) - 1);
            cfg->font_path[sizeof(cfg->font_path) - 1] = '\0';
        }
        else if (strcmp(key, "font_size") == 0)      cfg->font_size = atoi(val);
        else if (strcmp(key, "amber_r") == 0)        cfg->amber_r = atoi(val);
        else if (strcmp(key, "amber_g") == 0)        cfg->amber_g = atoi(val);
        else if (strcmp(key, "amber_b") == 0)        cfg->amber_b = atoi(val);
        else if (strcmp(key, "enable_scanlines") == 0)            cfg->enable_scanlines = parse_bool(val);
        else if (strcmp(key, "enable_vignette") == 0)              cfg->enable_vignette = parse_bool(val);
        else if (strcmp(key, "enable_glow") == 0)                  cfg->enable_glow = parse_bool(val);
        else if (strcmp(key, "enable_flicker") == 0)               cfg->enable_flicker = parse_bool(val);
        else if (strcmp(key, "enable_chromatic_aberration") == 0)  cfg->enable_chromatic_aberration = parse_bool(val);
        // Unknown keys are ignored
    }

    fclose(fp);
    return true;
}

bool config_save(const char *path, const Config *cfg) {
    FILE *fp = fopen(path, "w");
    if (!fp) return false;
    fprintf(fp, "# PopCo Monitor Configuration\n");
    fprintf(fp, "# Window / layout\n");
    fprintf(fp, "window_width=%d\n", cfg->window_width);
    fprintf(fp, "window_height=%d\n", cfg->window_height);
    fprintf(fp, "resizable=%s\n", cfg->resizable ? "true" : "false");
    fprintf(fp, "borderless=%s\n", cfg->borderless ? "true" : "false");
    fprintf(fp, "always_on_top=%s\n", cfg->always_on_top ? "true" : "false");
    fprintf(fp, "pos_x=%d\n", cfg->pos_x);
    fprintf(fp, "pos_y=%d\n", cfg->pos_y);
    fprintf(fp, "opacity=%.2f\n\n", cfg->opacity);

    fprintf(fp, "# Data\n");
    fprintf(fp, "refresh_ms=%d\n\n", cfg->refresh_ms);

    fprintf(fp, "# Theme settingsl\n");
    fprintf(fp, "font_path=%s\n", cfg->font_path);
    fprintf(fp, "font_size=%d\n", cfg->font_size);
    fprintf(fp, "amber_r=%d\n", cfg->amber_r);
    fprintf(fp, "amber_g=%d\n", cfg->amber_g);
    fprintf(fp, "amber_b=%d\n\n", cfg->amber_b);
    fprintf(fp, "enable_scanlines=%s\n", cfg->enable_scanlines ? "true" : "false");
    fprintf(fp, "enable_vignette=%s\n", cfg->enable_vignette ? "true" : "false");
    fprintf(fp, "enable_glow=%s\n", cfg->enable_glow ? "true" : "false");
    fprintf(fp, "enable_flicker=%s\n", cfg->enable_flicker ? "true" : "false");
    fprintf(fp, "enable_chromatic_aberration=%s\n", cfg->enable_chromatic_aberration ? "true" : "false");

    fclose(fp);
    return true;
}
