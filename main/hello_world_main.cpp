/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */


#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "LovyanGFX.hpp"
#if 0 // Touchscreen temporairement désactivé
#include "XPT2046_Touchscreen.h"
#endif
#if 0 // Preferences temporairement désactivé
#include "Preferences.h"
#endif
#include "esp_log.h"
#define TAG "BADGE"

// --- LGFX config custom (garde ta config) ---
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _panel_instance;
  lgfx::Bus_SPI _bus_instance;
  lgfx::Light_PWM _light_instance;
public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host = HSPI_HOST;
      cfg.spi_mode = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire = false;
      cfg.use_lock = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 14;
      cfg.pin_mosi = 13;
      cfg.pin_miso = 12;
      cfg.pin_dc   = 2;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = 15;
      cfg.pin_rst = -1;
      cfg.pin_busy = -1;
      cfg.memory_width = 320;
      cfg.memory_height = 240;
      cfg.panel_width = 320;
      cfg.panel_height = 240;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = false;
      cfg.invert = false;
      cfg.rgb_order = true;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 21;
      cfg.invert = false;
      cfg.freq   = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }
    setPanel(&_panel_instance);
  }
};
LGFX lcd;

// --- Touchscreen config ---

// Gestion du touchscreen désactivée temporairement

// Gestion Preferences désactivée temporairement
int likes = 0;
int screenW, screenH;
LGFX_Sprite spr(&lcd);

// Timing animation
// Glitch désactivé

// Colors (neon-ish)
uint16_t colBackground;
uint16_t colNeonViolet;
uint16_t colCyan;
uint16_t colYellow;
uint16_t colHeart;

// Offset pour l'animation des scanlines
int scanline_offset = 0;
// Animation glow progressive
int anim_glow_level = 0;
int anim_glow_direction = 1;

// --- Utilitaires ---
void drawScanlines();
void drawNameWithGlow(const char *name1, const char *name2);
void drawLikeCounter();
void drawStringGlowed(const char *str, int x, int y, int offset) {
  // Dessine le texte avec un effet glow (néon)
  spr.drawString(str, x + 1 + offset, y + 1 + offset);
  spr.drawString(str, x - 1 - offset, y + 1 + offset);
  spr.drawString(str, x + 1 + offset, y - 1 - offset);
  spr.drawString(str, x - 1 - offset, y - 1 - offset);
}

void display_loop_task(void *pvParameter);
extern "C" void app_main(void)
{
  ESP_LOGI(TAG, "Initialisation de l'écran...");
  lcd.init();
  srand((unsigned int)time(NULL)); // Init aléatoire
  lcd.setBrightness(255);
  lcd.setRotation(5); // mode portrait custom
  screenW = lcd.width();
  screenH = lcd.height();

  // Colors
  colBackground   = lcd.color565(8, 6, 20);
  colNeonViolet   = lcd.color565(199, 120, 255);
  colCyan         = lcd.color565(0, 224, 255);
  colYellow       = lcd.color565(255, 195, 0);
  colHeart        = lcd.color565(255, 40, 180);

  // Touch init désactivé

  // Sprite double buffering
  spr.setColorDepth(16);
  spr.createSprite(screenW, screenH);

  // Load likes désactivé

  // Initial draw
  spr.fillSprite(colBackground);
  drawScanlines();
  drawNameWithGlow("YANN", "SERGENT");
  drawLikeCounter();
  spr.pushSprite(0, 0);

  // Création de la tâche d'affichage à 60 fps
  xTaskCreate(display_loop_task, "display_loop", 4096, NULL, 2, NULL);
}

// Tâche d'affichage à 60 fps
void display_loop_task(void *pvParameter) {
  const TickType_t frame_delay = pdMS_TO_TICKS(16); // 60 fps = 16 ms
  unsigned long lastFrame = 0;
  while (true) {
    unsigned long now = lgfx::v1::millis();
    if (now - lastFrame < 16) {
      vTaskDelay(1 / portTICK_PERIOD_MS);
      continue;
    }
    lastFrame = now;

  // Animation scanlines
  scanline_offset = (scanline_offset + 1) % 10;
  // Animation glow progressive
  anim_glow_level += anim_glow_direction;
  if (anim_glow_level > 6) {
    anim_glow_level = 6;
    anim_glow_direction = -1;
  }
  if (anim_glow_level < -24) {
    anim_glow_level = -24;
    anim_glow_direction = 1;
  }
  spr.fillSprite(colBackground);
  drawScanlines();
  // Header "TAP TO LIKE"
  spr.setTextDatum(TC_DATUM);
  spr.setTextFont(1);
  spr.setTextSize(2);
  spr.setTextColor(colCyan);
  spr.drawString("TAP TO LIKE", screenW/2, 20);
  // Heart
  spr.fillCircle(screenW/2, 54, 16, colHeart);
  // Nom avec glow sur deux lignes (animation)
  drawNameWithGlow("YANN", "SERGENT");
  // Séparateur
  spr.fillRect(36, 180, screenW - 80, 3, colCyan);
  // Team
  spr.setTextDatum(TC_DATUM);
  spr.setTextFont(1);
  spr.setTextSize(2);
  spr.setTextColor(colCyan);
  spr.drawString("open|core", screenW/2, 200);
  // Ville + rôle
  spr.setTextDatum(TC_DATUM);
  spr.setTextFont(1);
  spr.setTextSize(2);
  spr.setTextColor(colYellow);
  spr.drawString("LYON", screenW/2, 230);
  spr.drawString("DEV FRONTEND", screenW/2, 260);
  // Compteur likes
  drawLikeCounter();
  spr.pushSprite(0, 0);
  vTaskDelay(frame_delay);
  }
}

// --- Fonctions utilitaires ---
void drawScanlines() {
  for (int y = 0; y < screenH; y += 10) {
    int animY = y + scanline_offset;
    if (animY < screenH) {
      uint16_t col = lcd.color565(12, 8, 30);
      spr.drawFastHLine(0, animY, screenW, col);
    }
  }
}

void drawNameWithGlow(const char *name1, const char *name2) {

  // Effet néon progressif
  spr.setTextDatum(MC_DATUM);
  spr.setTextFont(4);
  int x = screenW/2;
  int y1 = 110;
  int y2 = 145;

  if (anim_glow_level >= 5) {
    // Glow 3
    spr.setTextSize(1.8);
    spr.setTextColor(lcd.color565(60, 0, 80));
    drawStringGlowed(name1, x, y1, 4);
  drawStringGlowed(name2, x, y2, 4);
  }
  if (anim_glow_level >= 4) {
    // Glow 3 base
    spr.setTextSize(1.8);
    spr.setTextColor(lcd.color565(60, 0, 80));
    spr.drawString(name1, x, y1);
    spr.drawString(name2, x, y2);
    spr.setTextSize(1.7);
    drawStringGlowed(name1, x, y1, 1);
  drawStringGlowed(name2, x, y2, 1);
  }
  if (anim_glow_level >= 3) {
    // Glow 2
    spr.setTextSize(1.7);
    spr.setTextColor(lcd.color565(140, 60, 200));
    drawStringGlowed(name1, x, y1, 3);
  drawStringGlowed(name2, x, y2, 3);
  }
  if (anim_glow_level >= 2) {
    // Glow 2 base
    spr.setTextSize(1.7);
    spr.setTextColor(lcd.color565(140, 60, 200));
   spr.drawString(name1, x, y1);
    spr.drawString(name2, x, y2);
    spr.setTextSize(1.6);
    drawStringGlowed(name1, x, y1, 1);
  drawStringGlowed(name2, x, y2, 1);
  }
  if (anim_glow_level >= 1) {
    // Glow 1
    spr.setTextSize(1.6);
    spr.setTextColor(lcd.color565(255, 180, 255));
    drawStringGlowed(name1, x, y1, 2);
  drawStringGlowed(name2, x, y2, 2);
  }
  // Glow 1 base
  spr.setTextColor(lcd.color565(255, 180, 255));
  spr.setTextSize(1.5);
  drawStringGlowed(name1, x, y1, 1);
  drawStringGlowed(name2, x, y2, 1);

  // Texte principal blanc
  spr.setTextSize(1.5);
  spr.setTextColor(lcd.color565(255, 255, 255));
  spr.drawString(name1, x, y1);
  spr.drawString(name2, x, y2);

}


void drawLikeCounter() {
  spr.setTextDatum(TR_DATUM);
  spr.setTextFont(1);
  spr.setTextSize(2);
  spr.setTextColor(colHeart);
  char buf[16];
  snprintf(buf, sizeof(buf), "\xe2\x99\xa5 %d", likes); // coeur unicode
  spr.drawString(buf, screenW - 8, 8);
}