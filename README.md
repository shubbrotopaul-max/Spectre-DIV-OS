# 👻 SPECTRE-DIV OS
**A High-Performance WiFi Security & Research Suite for ESP32 & ST7789 TFT.**

SPECTRE-DIV OS হলো একটি কাস্টম বিল্ট ESP32 ফার্মওয়্যার যা মূলত ৩২০x২৪০ রেজোলিউশনের ST7789 TFT ডিসপ্লের জন্য তৈরি করা হয়েছে। এটি নেটওয়ার্ক সিকিউরিটি রিসার্চার এবং ইথিক্যাল হ্যাকারদের জন্য একটি পোর্টেবল সলিউশন।

![Platform](https://img.shields.io/badge/Platform-ESP32-green.svg)
![Display](https://img.shields.io/badge/Display-ST7789_320x240-orange.svg)
![License](https://img.shields.io/badge/License-MIT-blue.svg)

---

## 🛠 Features (মূল বৈশিষ্ট্যসমূহ)
* **Packet Monitor:** রিয়েল-টাইম WiFi ট্রাফিক এবং চ্যানেল লোড ভিজুয়ালাইজেশন।
* **Beacon Spam:** হাজার হাজার ফেক এক্সেস পয়েন্ট তৈরি করে স্ক্যানার স্ট্যাবিলিটি টেস্ট করা।
* **Deauth Detect:** আশেপাশে কোনো Deauthentication অ্যাটাক হচ্ছে কি না তা ডিটেক্ট করা।
* **WiFi Scanner:** সিগন্যাল স্ট্রেন্থ (RSSI) এবং এনক্রিপশন ডিটেইলস সহ নেটওয়ার্ক খোঁজা।
* **WiFi Jammer:** নেটওয়ার্ক রেজিলিয়েন্স চেক করার জন্য একটি শক্তিশালী স্ট্রেস-টেস্টিং টুল।
* **Captive Portal:** সিকিউরিটি অডিটের জন্য কাস্টম ফিশিং বা পোর্টাল পেজ তৈরি করা।
* **Custom Icon Engine:** ১৬x১৬ আইকনকে ৪x স্কেলে (৬৪x৬৪) শার্প রেন্ডার করার ক্ষমতা।

---

## 🚀 Applications (ব্যবহারক্ষেত্র)
* **Security Auditing:** অফিসের বা নিজের ওয়াইফাই নেটওয়ার্কের নিরাপত্তা পরীক্ষা করা।
* **Learning Tool:** ওয়াইফাই প্রোটোকল (802.11) এবং প্যাকেট হ্যালো কিভাবে কাজ করে তা শেখা।
* **Network Research:** আশেপাশে থাকা ডিভাইসের ঘনত্ব এবং চ্যানেল ইন্টারফেয়ারেন্স এনালাইসিস করা।

---

## 🔌 Pin Configuration (কানেকশন ডায়াগ্রাম)

নিচের টেবিল অনুযায়ী আপনার ESP32 এবং ST7789 ডিসপ্লে ও বাটনগুলোর কানেকশন দিন:

| Component | ESP32 Pin | Function | Description |
| :--- | :--- | :--- | :--- |
| **ST7789 TFT** | GPIO 18 | **SCLK** | Serial Clock |
| | GPIO 23 | **MOSI** | SPI Data Input |
| | GPIO 5 | **CS** | Chip Select |
| | GPIO 4 | **RST** | Display Reset |
| | 3.3V | **VCC** | Power Supply |
| | GND | **GND** | Ground |
| **Buttons** | GPIO 12 | **BTN_UP** | Menu Navigation (Up) |
| | GPIO 14 | **BTN_DOWN** | Menu Navigation (Down) |
| | GPIO 27 | **BTN_SELECT** | Confirm / Enter |

---

## 📚 Library Required (প্রয়োজনীয় লাইব্রেরি)
কোডটি কম্পাইল করার আগে Arduino IDE-তে নিচের লাইব্রেরিগুলো ইনস্টল করে নিন:
1. **Arduino_GFX_Library** (By Moon On Our Nation)
2. **WiFi** (Built-in for ESP32)
3. **esp_wifi** (Built-in for ESP32)

---

## 💻 How to Run Examples (কিভাবে চালাবেন)
১. **Repo ক্লোন করুন:** `git clone https://github.com/shubbrotopaul-max/Spectre-DIV-OS.git`
২. **Arduino IDE ওপেন করুন:** `ESP32-DIV.ino` ফাইলটি ওপেন করুন।
৩. **Config চেক করুন:** আপনার হার্ডওয়্যার অনুযায়ী `config.h` ফাইলে পিন নম্বরগুলো মিলিয়ে নিন।
৪. **বোর্ড সিলেক্ট করুন:** Tools > Board > ESP32 Dev Module।
৫. **Upload:** কোডটি ভেরিফাই করে আপলোড বাটনে ক্লিক করুন।

---

## 🎥 Demo Preview


---

## ⚠️ Disclaimer (সতর্কবার্তা)
এই প্রজেক্টটি শুধুমাত্র শিক্ষামূলক এবং ইথিক্যাল হ্যাকিং রিসার্চের উদ্দেশ্যে তৈরি। অনুমতি ছাড়া অন্যের নেটওয়ার্কে এটি ব্যবহার করা আইনত দণ্ডনীয় অপরাধ। কোনো অবৈধ কাজের জন্য ডেভেলপার দায়ী থাকবে না।

---

## ✉️ Contact (যোগাযোগ)
আপনার যদি কোনো প্রশ্ন থাকে বা প্রজেক্টে কন্ট্রিবিউট করতে চান, তবে যোগাযোগ করুন:

* **Developer:** Shubbroto Paul
* **GitHub:** [shubbrotopaul-max](https://github.com/shubbrotopaul-max)
* **Facebook:** [Shubbroto Paul](https://www.facebook.com/jpishubbroto)
* **Location:** Bangladesh 🇧🇩


ধন্যবাদ এই প্রজেক্টটি দেখার জন্য! যদি ভালো লাগে তবে একটি **Star** দিয়ে আমাদের সাপোর্ট করবেন। 🇧🇩🚀
