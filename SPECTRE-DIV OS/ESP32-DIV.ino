#include "config.h"
#include "Tools.h"
#include "icon.h" // Ensure this file is in your project folder!

Arduino_DataBus *bus = new Arduino_ESP32SPI(-1, TFT_CS, TFT_SCLK, TFT_MOSI, -1);
Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 0, false, 240, 320);

bool feature_exit_requested = false;
int menuIdx = 0;

// --- Helper Functions ---

void drawScanlines() {
  for(int i=0; i<320; i+=4) gfx->drawFastHLine(0, i, 240, M_DARK);
}

void drawMatrixHeader(String title) {
  gfx->fillRect(0, 0, 240, 35, M_GREEN);
  gfx->setTextColor(M_BLACK);
  gfx->setCursor(10, 10); gfx->setTextSize(2);
  gfx->print("> " + title);
}

// --- The Fixed Menu Function ---
void drawMenu() {
  gfx->fillScreen(M_BLACK);
  drawScanlines();
  drawMatrixHeader("SPECTRE-DIV OS");

  for(int i = 0; i < 6; i++) {
    int y = 75 + (i * 40);
    
    if(i == menuIdx) {
      // Highlight Bar
      gfx->fillRect(0, y - 12, 240, 35, M_GREEN);
      
      // Draw Icon in Black (Inverted contrast)
      gfx->drawBitmap(10, y - 5, myMenu[i].bitmap, 16, 16, M_BLACK);
      
      gfx->setTextColor(M_BLACK);
      gfx->setCursor(35, y); 
      gfx->print(">> " + String(myMenu[i].name));
    } else {
      // Draw Icon in its unique color from icon.h
      gfx->drawBitmap(10, y - 5, myMenu[i].bitmap, 16, 16, myMenu[i].color);
      
      gfx->setTextColor(M_GREEN);
      gfx->setCursor(35, y); 
      gfx->print("   " + String(myMenu[i].name));
    }
  }
}

void showBootLogo() {
  gfx->fillScreen(M_BLACK);
  drawScanlines();
  
  // Header
  gfx->setTextSize(2);
  gfx->setTextColor(M_GREEN);
  gfx->setCursor(10, 20);
  gfx->println("SPECTRE-DIV OS");
  gfx->drawFastHLine(0, 45, 240, M_GREEN);
  
  // Center Ghost Logo (Scaled 4x manually)
  // Parameters: x, y, bitmap, width, height, color, scale
  drawScaledBitmap(88, 70, icon_ghost, 16, 16, M_GREEN, 4); 

  delay(800);
  gfx->setTextSize(1);
  gfx->setCursor(10, 75);
  gfx->println("INITIALIZING SYSTEM...");
  delay(400);

  uint32_t flashSize = ESP.getFlashChipSize() / (1024 * 1024);
  uint32_t cpuSpeed = ESP.getCpuFreqMHz();
  String chipModel = ESP.getChipModel();
  uint64_t chipID = ESP.getEfuseMac();

  gfx->setTextColor(M_GREEN);
  gfx->setCursor(10, 110);
  gfx->printf("MODEL: %s", chipModel.c_str());
  delay(200);
  
  gfx->setCursor(10, 130);
  gfx->printf("CPU  : %d MHz", cpuSpeed);
  delay(200);
  
  gfx->setCursor(10, 150);
  gfx->printf("FLASH: %d MB", flashSize);
  delay(200);
  
  gfx->setCursor(10, 170);
  gfx->printf("ID   : %04X%08X", (uint16_t)(chipID >> 32), (uint32_t)chipID);
  delay(200);

  gfx->setCursor(10, 210);
  gfx->setTextColor(M_BLACK, M_GREEN); 
  gfx->print(" STATUS: SYSTEM_READY ");
  
  delay(1500); 
}

void setup() {
  Serial.begin(115200);
  gfx->begin();
  gfx->setRotation(0); 

  showBootLogo(); 
  
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  
  drawMenu(); 
}

void loop() {
  if(digitalRead(BTN_UP) == LOW) {
    menuIdx = (menuIdx <= 0) ? 5 : menuIdx - 1;
    drawMenu(); delay(200);
  }
  if(digitalRead(BTN_DOWN) == LOW) {
    menuIdx = (menuIdx >= 5) ? 0 : menuIdx + 1;
    drawMenu(); delay(200);
  }
  if(digitalRead(BTN_SELECT) == LOW) {
    delay(300);
    feature_exit_requested = false;
    
    if(menuIdx == 0) runPacketMonitor();
    else if(menuIdx == 1) runBeaconSpam();
    else if(menuIdx == 2) runDeauthDetect();
    else if(menuIdx == 3) runWifiScan();
    else if(menuIdx == 4) runDeauthAttack();
    else if(menuIdx == 5) runPortal();
    
    // Return to Menu
    esp_wifi_set_promiscuous(false);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    drawMenu();
  }
}

bool checkExit() {
  if (digitalRead(BTN_SELECT) == LOW) {
    uint32_t start = millis();
    while(digitalRead(BTN_SELECT) == LOW) {
      if (millis() - start > 800) { 
        while(digitalRead(BTN_SELECT) == LOW) { delay(10); } 
        feature_exit_requested = true; 
        delay(150); 
        return true; 
      }
    }
  }
  return false;
}

void drawScaledBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint8_t scale) {
  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      // Logic for Horizontal/TFT style bitmaps
      if (pgm_read_byte(&bitmap[j * ((w + 7) / 8) + i / 8]) & (0x80 >> (i % 8))) {
        gfx->fillRect(x + (i * scale), y + (j * scale), scale, scale, color);
      }
    }
  }
}