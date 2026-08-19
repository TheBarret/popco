#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
    // Window / layout
    int  window_width;
    int  window_height;
    bool resizable;
    bool borderless;
    bool always_on_top;
    int  pos_x;             // -1 = center
    int  pos_y;             // -1 = center
    float opacity;          // 0.0 - 1.0, whole-window alpha (compositor dependent)

    // Data
    int  refresh_ms;        // system stats re-sample rate

    // Look & feel
    char font_path[512];
    int  font_size;
    int  amber_r, amber_g, amber_b;

    bool enable_scanlines;
    bool enable_vignette;
    bool enable_glow;
    bool enable_flicker;
    bool enable_chromatic_aberration;
} Config;

// Fills cfg with sane defaults matching the original RobCo Monitor effect.
void config_set_defaults(Config *cfg);

// Loads key=value pairs from path, overriding defaults already in cfg.
// Missing file is not an error -- caller just keeps the defaults.
// Returns true if the file was found and parsed, false if it fell back to defaults.
bool config_load(const char *path, Config *cfg);

// Writes the current config back out in the same key=value format.
// Used to persist things like window position between runs.
bool config_save(const char *path, const Config *cfg);

#endif
