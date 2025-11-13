#pragma once

#include <cstdint>

class AppState
{
public:
    int likes = 0;
    float t = 0.0f;
    float neon_intensity = 1.0f;
    int scanline_offset = 0;
    int anim_glow_level = 0;
    int anim_glow_direction = 1;
    int screenW = 0;
    int screenH = 0;
    int touch_x = -1;
    int touch_y = -1;

    // Variables pour l'animation du néon
    unsigned long neon_last_update = 0;
    float neon_time = 0.0f;
    unsigned long neon_flicker_start = 0;
    unsigned long neon_next_flicker = 0;
    bool neon_is_flickering = false;
    float neon_flicker_intensity = 1.0f;
};
