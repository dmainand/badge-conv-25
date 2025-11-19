#ifndef CONFIG_H
#define CONFIG_H

namespace Config
{
    inline float awakeTime = 5.0f;        // Durée avant veille (minutes)
    inline uint8_t sleepBrightness = 10;  // Luminosité en veille (%)
    inline uint8_t activeBrightness = 80; // Luminosité active (%)
    inline bool display_rotated = false;  // Rotation écran 180°

    inline void setActiveBrightness(uint8_t value) { activeBrightness = value; }
    inline void setSleepBrightness(uint8_t value) { sleepBrightness = value; }
    inline void setDisplayRotated(bool value) { display_rotated = value; }
}

#endif // CONFIG_H
