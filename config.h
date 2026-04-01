#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "esp_wifi.h"

// --- PINS ---
#define TFT_SCLK 18
#define TFT_MOSI 23
#define TFT_CS   5
#define TFT_RST  4

#define BTN_UP    12
#define BTN_DOWN  13
#define BTN_SELECT 14

// --- MATRIX UI COLORS ---
#define M_GREEN  0x07E0 
#define M_BLACK  0x0000
#define M_DARK   0x0020 

// --- SHARED GLOBALS ---
extern Arduino_GFX *gfx;
extern bool feature_exit_requested;

// --- SHARED FUNCTIONS ---
bool checkExit();
void drawMatrixHeader(String title);
void drawScanlines();

#endif