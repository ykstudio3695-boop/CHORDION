/*
ハードウェア構成
- Teensy4.1
- Teensy Audio Sheild [RevD]
- PAM 8302
- 8ohm spk
- OLED 32 x 128
*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== WAV変換ヘッダのインクルード =====
#include "AudioSampleTom_l.h"
#include "AudioSampleTom_m.h"
#include "AudioSampleTom_h.h"
#include "AudioSampleSnare.h"
#include "AudioSampleKick.h"
#include "AudioSampleHihat.h"
#include "AudioSampleCymbal.h"

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===== LOGO DATA =====
const unsigned char logo [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x0f, 0x0f, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xfd, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x3f, 0xcf, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xfd, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x3f, 0x4f, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xfd, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1c, 0x03, 0x87, 0x7e, 0xec, 0x0c, 0x03, 0x80, 0x07, 0x0e, 0xfd, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1c, 0x03, 0xff, 0x79, 0xef, 0xfd, 0xc3, 0x87, 0x87, 0x0e, 0xff, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1c, 0x03, 0xff, 0x79, 0xef, 0xfd, 0xc3, 0x87, 0x87, 0x0e, 0xff, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1c, 0x03, 0xff, 0x77, 0xef, 0xe1, 0xc3, 0x80, 0x07, 0x0e, 0xef, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x2f, 0xce, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xef, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x3f, 0xce, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xef, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xfb, 0x87, 0x0f, 0x0e, 0xfd, 0xff, 0xbb, 0x77, 0xfe, 0xef, 0xc0, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x3d, 0x17, 0xa0, 0xce, 0x7b, 0x87, 0xb9, 0xdf, 0x73, 0x24, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x04, 0xa1, 0x14, 0x21, 0x29, 0x42, 0x44, 0x24, 0x84, 0x24, 0xb4, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x04, 0xb8, 0xa7, 0x21, 0x2e, 0x73, 0x87, 0x24, 0x84, 0x24, 0xac, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x04, 0xa0, 0xa4, 0x21, 0x28, 0x42, 0x84, 0x24, 0x84, 0x24, 0xa4, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x07, 0x3c, 0x47, 0xbc, 0xc8, 0x7a, 0x47, 0xb9, 0xc4, 0x73, 0x24, 0x80, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ================= INPUT =================
const int NUM_KEYS = 7;
// buttons[4] = 35(戻る), buttons[5] = 34(決定), buttons[6] = 33(進む)
const int buttons[NUM_KEYS] = {39, 38, 37, 36, 35, 34, 33};
const int MODE_BUTTON = 40;
const int OUTPUT_SWITCH_PIN = 41;
const int VOLUME_PIN = A0;
const int PITCH_PIN = A1;

float smoothedPitchRaw = 512.0;
float pitchMultiplier = 1.0;

// ================= BATTERY =================
const int BATTERY_PIN = 22; // Teensy 4.1 Pin 22 (A8)
unsigned long lastBatteryCheckTime = 0;
const unsigned long BATTERY_CHECK_INTERVAL = 1000;
int batteryPercent = 0;
float smoothedBatVoltage = -1.0;

// ================= OUTPUT STATE =================
bool isHeadphoneActive = true;
bool lastOutputBtn = HIGH;

// ================= MODE =================
// ★ MODE_MONO を追加
enum PlayMode { MODE_CHORD, MODE_ARP, MODE_DRUM, MODE_MONO };
PlayMode currentMode = MODE_CHORD;
bool lastModeBtn = HIGH;

// ================= INSTRUMENT PATCHES (MONO MODE) =================
// ★ 単音モード用の音色パッチ構造体とプリセット
struct InstrumentPatch {
  const char* name;
  short waveform;
  float attack;
  float decay;
  float sustain;
  float release;
};

const InstrumentPatch myPatches[] = {
  {"Lead (Saw)",   WAVEFORM_SAWTOOTH, 10.0,  100.0, 0.8,  200.0},
  {"Pluck (Sine)", WAVEFORM_SINE,      1.0,  150.0, 0.0,  100.0}, 
  {"8-bit (Sq)",   WAVEFORM_SQUARE,    5.0,  100.0, 0.5,  100.0}, 
  {"Pad (Tri)",    WAVEFORM_TRIANGLE, 400.0, 200.0, 1.0,  800.0}  
};
const int TOTAL_PATCHES = sizeof(myPatches) / sizeof(InstrumentPatch);
int currentPatchIndex = 0; // 現在適用されているパッチ番号

// ================= CHORD SET (BANK) MANAGEMENT =================
struct ChordData {
  float freqs[4];       // 構成音4つの周波数
  const char* name;    // コードネーム
};

struct ChordBank {
  const char* bankName;       // コードセットの名前
  ChordData chords[NUM_KEYS]; // 7つの鍵盤に対応するコード
};

const ChordBank myProgressionList[] = {
  {
    "Key of C (Pop/Std)",
    {
      {{261.63, 329.63, 392.00, 493.88}, "Cmaj7"},   // I
      {{293.66, 349.23, 440.00, 523.25}, "Dm7"},     // II
      {{329.63, 392.00, 493.88, 587.33}, "Em7"},     // III
      {{349.23, 440.00, 523.25, 659.25}, "Fmaj7"},   // IV
      {{392.00, 493.88, 587.33, 698.46}, "G7"},      // V
      {{440.00, 523.25, 659.25, 783.99}, "Am7"},     // VI
      {{493.88, 587.33, 698.46, 880.00}, "Bm7b5"}    // VII
    }
  },
  {
    "Key of F (Jazz/R&B)",
    {
      {{174.61, 220.00, 261.63, 329.63}, "Fmaj7"},   // I
      {{196.00, 233.08, 293.66, 349.23}, "Gm7"},     // II
      {{220.00, 261.63, 329.63, 392.00}, "Am7"},     // III
      {{233.08, 293.66, 349.23, 440.00}, "Bbmaj7"},  // IV
      {{261.63, 329.63, 392.00, 466.16}, "C7"},      // V
      {{293.66, 349.23, 440.00, 523.25}, "Dm7"},     // VI
      {{329.63, 392.00, 466.16, 587.33}, "Em7b5"}    // VII
    }
  },
  {
    "Key of G (Acoustic)",
    {
      {{196.00, 246.94, 293.66, 369.99}, "Gmaj7"},   // I
      {{220.00, 261.63, 329.63, 392.00}, "Am7"},     // II
      {{246.94, 293.66, 369.99, 440.00}, "Bm7"},     // III
      {{261.63, 329.63, 392.00, 493.88}, "Cmaj7"},   // IV
      {{293.66, 369.99, 440.00, 523.25}, "D7"},      // V
      {{329.63, 392.00, 493.88, 587.33}, "Em7"},     // VI
      {{369.99, 440.00, 523.25, 659.25}, "F#m7b5"}   // VII
    }
  }
};

const int TOTAL_BANKS = sizeof(myProgressionList) / sizeof(ChordBank);
int currentBankIndex = 0;      
int selectedBankIndex = 0;     
bool isBankSelectMode = false; 

const char* noteNames[4] = { "Root", "3rd", "5th", "7th" };

// ================= DRUM (MEMORY) =================
const int DRUM_VOICES = 2;
const char* drumNames[NUM_KEYS] = {
  "Cymbal", "Tom M", "Tom H", "Tom L", "Snare", "Kick", "Hihat"
};
AudioPlayMemory drumPlay[NUM_KEYS][DRUM_VOICES];
int drumVoiceIndex[NUM_KEYS] = {0};

AudioMixer4 drumVoiceMixer[NUM_KEYS];
AudioMixer4 drumMixerA;
AudioMixer4 drumMixerB;

// ================= AUDIO =================
AudioSynthWaveform osc[NUM_KEYS][8];
AudioEffectEnvelope env[NUM_KEYS][8];
AudioMixer4 keyMixerHigh[NUM_KEYS];
AudioMixer4 keyMixerLow[NUM_KEYS];
AudioMixer4 keyMixerSum[NUM_KEYS];
AudioMixer4 masterMixerA;
AudioMixer4 masterMixerB;
AudioMixer4 finalMixer;
AudioEffectFreeverb reverb;
AudioMixer4 reverbMixer;
AudioOutputI2S audioOut;
AudioControlSGTL5000 sgtl5000;

AudioConnection* patchCords[300]; 
int pc = 0;

void patch(AudioStream& s, AudioStream& d) {
  patchCords[pc++] = new AudioConnection(s, d);
}
void patch(AudioStream& s, int so, AudioStream& d, int di) {
  patchCords[pc++] = new AudioConnection(s, so, d, di);
}

// ================= ENVELOPE =================
float envAttack = 500.0;
float envDecay = 120.0;
float envSustain = 1.00;
float envRelease = 500.0;

void applyEnvelopeSettings(float a, float d, float s, float r) {
  envAttack = a;
  envDecay = d;
  envSustain = s;
  envRelease = r;
  
  for (int k = 0; k < NUM_KEYS; k++) {
    for (int i = 0; i < 8; i++) {
      env[k][i].attack(envAttack);
      env[k][i].decay(envDecay);
      env[k][i].sustain(envSustain);
      env[k][i].release(envRelease);
    }
  }
}

// ================= STATE =================
bool keyState[NUM_KEYS] = {false};
int activeDisplayKey = -1; 
bool lastUIBtnState[NUM_KEYS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

// ---- Arp ----
unsigned long lastArpTime[NUM_KEYS] = {0};
int arpIndex[NUM_KEYS] = {0};
bool arpActive[NUM_KEYS] = {false};

int currentArpNote[NUM_KEYS] = {0};
bool arpNoteVisible[NUM_KEYS] = {false};

const int arpInterval = 100;

// ================= VOLUME DISPLAY STATE =================
int lastRawVol = -1;
float smoothedVol = -1.0;
unsigned long volDisplayStartTime = 0;
bool isVolDisplayActive = false;
const int VOL_THRESHOLD = 5;
const unsigned long VOL_SHOW_TIME = 800;

// ================= 41PIN DOUBLE CLICK STATE =================
unsigned long lastOutBtnPressTime = 0;
bool outBtnPending = false;
const unsigned long DOUBLE_CLICK_TIME = 250;

// ================= UTILS =================
void resetArpState(int k) {
  for (int i = 0; i < 8; i++) env[k][i].noteOff();
  arpIndex[k] = 0;
  arpActive[k] = false;
  arpNoteVisible[k] = false;
}

void drawCenter(const String& text, int y, int size) {
  display.setTextSize(size);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - (int)w) / 2;
  display.setCursor(x, y);
  display.print(text);
}

// ================= SETUP =================
void setup() {
  AudioMemory(300); 
  sgtl5000.enable();
  sgtl5000.volume(1.0);

  pinMode(BATTERY_PIN, INPUT);

  for (int i = 0; i < NUM_KEYS; i++) pinMode(buttons[i], INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(OUTPUT_SWITCH_PIN, INPUT_PULLUP);

  for (int k = 0; k < NUM_KEYS; k++) {
    for (int i = 0; i < 8; i++) {
      osc[k][i].begin(WAVEFORM_SAWTOOTH);
      osc[k][i].amplitude(0.32);

      env[k][i].attack(envAttack);
      env[k][i].decay(envDecay);
      env[k][i].sustain(envSustain);
      env[k][i].release(envRelease);

      patch(osc[k][i], env[k][i]);
      if (i < 4) patch(env[k][i], 0, keyMixerHigh[k], i);
      else       patch(env[k][i], 0, keyMixerLow[k], i - 4);
    }

    for (int ch = 0; ch < 4; ch++) {
      keyMixerHigh[k].gain(ch, 0.75);
      keyMixerLow[k].gain(ch, 0.20);
    }

    patch(keyMixerHigh[k], 0, keyMixerSum[k], 0);
    patch(keyMixerLow[k], 0, keyMixerSum[k], 1);

    keyMixerSum[k].gain(0, 0.4);
    keyMixerSum[k].gain(1, 1.0);

    if (k < 4) patch(keyMixerSum[k], 0, masterMixerA, k);
    else       patch(keyMixerSum[k], 0, masterMixerB, k - 4);

    for (int v = 0; v < DRUM_VOICES; v++) {
      patch(drumPlay[k][v], 0, drumVoiceMixer[k], v);
      drumVoiceMixer[k].gain(v, 0.7);
    }
    drumVoiceMixer[k].gain(2, 0.0);
    drumVoiceMixer[k].gain(3, 0.0);

    if (k < 4) patch(drumVoiceMixer[k], 0, drumMixerA, k);
    else       patch(drumVoiceMixer[k], 0, drumMixerB, k - 4);
  }

  for (int ch = 0; ch < 4; ch++) {
    masterMixerA.gain(ch, 0.25);
    masterMixerB.gain(ch, 0.25);

    drumMixerA.gain(ch, 0.3);
    drumMixerB.gain(ch, 0.3);
  }

  patch(masterMixerA, 0, finalMixer, 0);
  patch(masterMixerB, 0, finalMixer, 1);
  patch(drumMixerA,   0, finalMixer, 2);
  patch(drumMixerB,   0, finalMixer, 3);

  finalMixer.gain(0, 0.4);
  finalMixer.gain(1, 0.4);
  finalMixer.gain(2, 0.3);
  finalMixer.gain(3, 0.3);

  patch(finalMixer, 0, reverbMixer, 0);
  patch(finalMixer, 0, reverb, 0);
  patch(reverb, 0, reverbMixer, 1);

  reverbMixer.gain(0, 0.55);
  reverbMixer.gain(1, 0.55);
  reverb.roomsize(0.7);
  reverb.damping(0.75);

  patch(reverbMixer, 0, audioOut, 0);
  patch(reverbMixer, 0, audioOut, 1);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawBitmap(0, 0, logo, 128, 32, SSD1306_WHITE);
  display.display();
  delay(1200);
}

bool modeBlink = false;
unsigned long modeBlinkStart = 0;
bool modeBlinkVisible = true;

const int MODE_BLINK_COUNT = 3;
const int MODE_BLINK_INTERVAL = 50;

// ================= LOOP =================
void loop() {
  // --- 音量ノブの処理（常時バックグラウンド実行） ---
  int rawVol = analogRead(VOLUME_PIN);
  if (smoothedVol < 0) smoothedVol = rawVol;
  smoothedVol = (smoothedVol * 0.8) + (rawVol * 0.2);

  int filteredVol = (int)smoothedVol;
  if (filteredVol > 1005) filteredVol = 1023;
  if (filteredVol < 15) filteredVol = 0;

  if (lastRawVol == -1 || abs(filteredVol - lastRawVol) > VOL_THRESHOLD) {
    lastRawVol = filteredVol;
    isVolDisplayActive = true;
    volDisplayStartTime = millis();
  }

  if (isVolDisplayActive && (millis() - volDisplayStartTime > VOL_SHOW_TIME)) {
    isVolDisplayActive = false;
  }

  float volumeScale = (float)filteredVol / 1023.0;

  if (isHeadphoneActive) {
    sgtl5000.unmuteHeadphone();
    finalMixer.gain(0, volumeScale * 0.4);
    finalMixer.gain(1, volumeScale * 0.4);
    finalMixer.gain(2, volumeScale * 0.3);
    finalMixer.gain(3, volumeScale * 0.3);
  } else {
    sgtl5000.muteHeadphone();
    finalMixer.gain(0, volumeScale * 17.0);
    finalMixer.gain(1, volumeScale * 17.0);
    finalMixer.gain(2, volumeScale * 15.0);
    finalMixer.gain(3, volumeScale * 15.0);
  }
  sgtl5000.volume(volumeScale);


  // --- 41pinボタンのダブルクリック / シングルクリック判定 ---
  bool outBtn = digitalRead(OUTPUT_SWITCH_PIN);
  if (lastOutputBtn == HIGH && outBtn == LOW) { 
    unsigned long now = millis();
    if (now - lastOutBtnPressTime < DOUBLE_CLICK_TIME) {
      isBankSelectMode = true;
      selectedBankIndex = currentBankIndex; 
      outBtnPending = false;
      for (int k = 0; k < NUM_KEYS; k++) resetArpState(k);
    } else {
      outBtnPending = true;
    }
    lastOutBtnPressTime = now;
    delay(10); 
  }
  lastOutputBtn = outBtn;

  if (outBtnPending && (millis() - lastOutBtnPressTime > DOUBLE_CLICK_TIME)) {
    outBtnPending = false;
    isHeadphoneActive = !isHeadphoneActive;

    if (isHeadphoneActive) {
      for (int r = 0; r < 2; r++) {
        display.clearDisplay(); drawCenter("HEADPHONE", 12, 2); display.display(); delay(100);
        display.clearDisplay(); display.display(); delay(100);
      }
      sgtl5000.unmuteHeadphone();
    } else {
      for (int r = 0; r < 2; r++) {
        display.clearDisplay(); drawCenter("SPEAKER", 12, 2); display.display(); delay(100);
        display.clearDisplay(); display.display(); delay(100);
      }
      sgtl5000.muteHeadphone();
    }
  }


  // ========================================================
  // 分岐：コードセット選択UIモード
  // ========================================================
  if (isBankSelectMode) {
    bool btn35 = digitalRead(buttons[4]);
    if (lastUIBtnState[4] == HIGH && btn35 == LOW) {
      selectedBankIndex--;
      if (selectedBankIndex < 0) selectedBankIndex = TOTAL_BANKS - 1;
      delay(10);
    }
    lastUIBtnState[4] = btn35;

    bool btn33 = digitalRead(buttons[6]);
    if (lastUIBtnState[6] == HIGH && btn33 == LOW) {
      selectedBankIndex = (selectedBankIndex + 1) % TOTAL_BANKS;
      delay(10);
    }
    lastUIBtnState[6] = btn33;

    bool btn34 = digitalRead(buttons[5]);
    if (lastUIBtnState[5] == HIGH && btn34 == LOW) {
      currentBankIndex = selectedBankIndex;
      isBankSelectMode = false; 
      
      for (int i = 0; i < NUM_KEYS; i++) {
        keyState[i] = false;
        resetArpState(i);
        lastUIBtnState[i] = HIGH;
      }
      activeDisplayKey = -1;
      delay(10);
    }
    lastUIBtnState[5] = btn34;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SELECT CHORD SET");
    drawCenter(myProgressionList[selectedBankIndex].bankName, 10, 1);
    
    display.drawRect(20, 21, 22, 11, SSD1306_WHITE);
    display.setCursor(28, 23);
    display.print("<");
    
    display.drawRect(58, 21, 11, 11, SSD1306_WHITE);
    
    display.drawRect(84, 21, 22, 11, SSD1306_WHITE);
    display.setCursor(92, 23);
    display.print(">");

  } 
  // ========================================================
  // 分岐：通常演奏モード
  // ========================================================
  else {
    // --- モードボタンの処理 ---
    bool modeBtn = digitalRead(MODE_BUTTON);
    if (lastModeBtn == HIGH && modeBtn == LOW) {
      // ★ 4つのモードを循環するように変更 (3 -> 4)
      currentMode = (PlayMode)((currentMode + 1) % 4);
      for (int k = 0; k < NUM_KEYS; k++) resetArpState(k);

      modeBlink = true;
      modeBlinkStart = millis();
      modeBlinkVisible = true;
    }
    lastModeBtn = modeBtn;

    // --- 画面の基本描画 ---
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(isHeadphoneActive ? "[H] " : "[S] ");
    display.print("mode > ");

    if (modeBlink) {
      unsigned long elapsed = millis() - modeBlinkStart;
      int phase = elapsed / MODE_BLINK_INTERVAL;
      if (phase >= MODE_BLINK_COUNT * 2) {
        modeBlink = false;
        modeBlinkVisible = true;
      } else {
        modeBlinkVisible = (phase % 2 == 0);
      }
    }

    if (modeBlinkVisible) {
      // ★ MODE_MONO の表示処理を追加
      if (currentMode == MODE_CHORD) display.print("chord");
      else if (currentMode == MODE_ARP) display.print("arp");
      else if (currentMode == MODE_DRUM) display.print("drum");
      else display.print("mono");
    }

    // --- ピッチスライダーの処理 ---
    int rawPitch = analogRead(PITCH_PIN);
    smoothedPitchRaw = (smoothedPitchRaw * 0.8) + (rawPitch * 0.2);
    float pitchOctave = ((smoothedPitchRaw - 512.0) / 512.0) * 1.0; 
    pitchMultiplier = pow(2.0, pitchOctave);

    // --- 鍵盤入力の処理と演奏 ---
    for (int k = 0; k < NUM_KEYS; k++) {
      bool pressed = (digitalRead(buttons[k]) == LOW);

      if (pressed && !keyState[k]) {
        keyState[k] = true;

        if (activeDisplayKey == -1) {
          activeDisplayKey = k;
        }

        if (currentMode == MODE_CHORD) {
          applyEnvelopeSettings(100.0, 120.0, 1.0, 500.0);
          for (int v = 0; v < 4; v++) {
            // ★ 単音モードで書き換わった波形をリセット
            osc[k][v].begin(WAVEFORM_SAWTOOTH); 
            osc[k][v + 4].begin(WAVEFORM_SAWTOOTH);

            float f = myProgressionList[currentBankIndex].chords[k].freqs[v];
            float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
            osc[k][v].frequency((f / 2.0) * pitchMultiplier);
            osc[k][v + 4].frequency((rootF / 4.0) * pitchMultiplier);
            env[k][v].noteOn();
            env[k][v + 4].noteOn();
          }
        } else if (currentMode == MODE_ARP) {
          applyEnvelopeSettings(1.0, 1.0, 1.0, 100.0);
          arpActive[k] = true;
          arpIndex[k] = 0;
          lastArpTime[k] = millis();

          int n = 0;
          // ★ 単音モードで書き換わった波形をリセット
          osc[k][n].begin(WAVEFORM_SAWTOOTH); 
          osc[k][n + 4].begin(WAVEFORM_SAWTOOTH);

          float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
          float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
          osc[k][n].frequency((f / 2.0) * pitchMultiplier);
          osc[k][n + 4].frequency((rootF / 8.0) * pitchMultiplier);
          env[k][n].noteOn();
          env[k][n + 4].noteOn();

          currentArpNote[k] = n;
          arpNoteVisible[k] = true;
          arpIndex[k] = 1;
        } else if (currentMode == MODE_DRUM) {
          int v = drumVoiceIndex[k];
          switch (k) {
            case 0: drumPlay[k][v].play(AudioSampleCymbal); break;
            case 1: drumPlay[k][v].play(AudioSampleTom_m);  break;
            case 2: drumPlay[k][v].play(AudioSampleTom_h);  break;
            case 3: drumPlay[k][v].play(AudioSampleTom_l);  break;
            case 4: drumPlay[k][v].play(AudioSampleSnare);  break;
            case 5: drumPlay[k][v].play(AudioSampleKick);   break;
            case 6: drumPlay[k][v].play(AudioSampleHihat);  break;
          }
          drumVoiceIndex[k] = (drumVoiceIndex[k] + 1) % DRUM_VOICES;
        }
        // ★ MODE_MONO の発音処理を追加
        else if (currentMode == MODE_MONO) {
          InstrumentPatch patch = myPatches[currentPatchIndex];
          applyEnvelopeSettings(patch.attack, patch.decay, patch.sustain, patch.release);

          // ダイアトニックスケール（現在選択中のコードセットのルート音）を取得
          float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
          
          // オシレーター0: メイン波形
          osc[k][0].begin(patch.waveform);
          osc[k][0].frequency((rootF / 2.0) * pitchMultiplier);
          env[k][0].noteOn();

          // オシレーター4: サブオシレーター（1オクターブ下を鳴らして音に厚みを持たせる）
          osc[k][4].begin(patch.waveform);
          osc[k][4].frequency((rootF / 4.0) * pitchMultiplier);
          env[k][4].noteOn();
        }
      }

      if (currentMode == MODE_ARP && pressed && arpActive[k]) {
        if (millis() - lastArpTime[k] > arpInterval) {
          for (int i = 0; i < 8; i++) env[k][i].noteOff();

          int n = arpIndex[k] % 4;

          // ★ 単音モードで書き換わった波形をリセット
          osc[k][n].begin(WAVEFORM_SAWTOOTH); 
          osc[k][n + 4].begin(WAVEFORM_SAWTOOTH);

          float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
          float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
          osc[k][n].frequency((f / 2.0) * pitchMultiplier);
          osc[k][n + 4].frequency((rootF / 8.0) * pitchMultiplier);
          env[k][n].noteOn();
          env[k][n + 4].noteOn();

          currentArpNote[k] = n;
          arpNoteVisible[k] = true;

          lastArpTime[k] = millis();
          arpIndex[k]++;
        }
      }

      if (!pressed && keyState[k]) {
        keyState[k] = false;
        resetArpState(k);

        if (activeDisplayKey == k) {
          activeDisplayKey = -1;
          for (int i = 0; i < NUM_KEYS; i++) {
            if (keyState[i]) { activeDisplayKey = i; break; }
          }
        }
      }

      // --- 演奏中の画面描画テキスト / ピッチ更新 ---
      if (pressed && currentMode == MODE_CHORD) {
        if (k == activeDisplayKey) {
          drawCenter(myProgressionList[currentBankIndex].chords[k].name, random(19, 21), 1);
        }
        for (int v = 0; v < 4; v++) {
          float f = myProgressionList[currentBankIndex].chords[k].freqs[v];
          float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
          osc[k][v].frequency((f / 2.0) * pitchMultiplier);
          osc[k][v + 4].frequency((rootF / 4.0) * pitchMultiplier);
        }
      }

      if (pressed && currentMode == MODE_ARP && arpNoteVisible[k]) {
        if (k == activeDisplayKey) {
          drawCenter(noteNames[currentArpNote[k]], 20, 1);
          drawCenter(myProgressionList[currentBankIndex].chords[k].name, 48, 1);
        }
        int n = currentArpNote[k];
        float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
        float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
        osc[k][n].frequency((f / 2.0) * pitchMultiplier);
        osc[k][n + 4].frequency((rootF / 8.0) * pitchMultiplier);
      }

      if (pressed && currentMode == MODE_DRUM) {
        if (k == activeDisplayKey) {
          drawCenter(drumNames[k], 20, 1);
        }
      }

      // ★ MODE_MONO の画面描画とピッチベンド追従処理
      if (pressed && currentMode == MODE_MONO) {
        if (k == activeDisplayKey) {
          drawCenter(myPatches[currentPatchIndex].name, 8, 1);  // パッチ名を表示
          drawCenter(myProgressionList[currentBankIndex].chords[k].name, 20, 1); // 鳴らしているルート音の由来
        }
        float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
        osc[k][0].frequency((rootF / 2.0) * pitchMultiplier);
        osc[k][4].frequency((rootF / 4.0) * pitchMultiplier);
      }
    }
  }

  // --- 音量オーバーレイ表示 ---
  if (isVolDisplayActive) {
    display.clearDisplay();
    int volPercent = (filteredVol * 100) / 1023;
    String volStr = "VOLUME : " + String(volPercent) + " %";
    drawCenter(volStr, 14, 1);
  }

  // --- バッテリー残量取得 ---
  unsigned long currentMillis = millis();
  if (currentMillis - lastBatteryCheckTime >= BATTERY_CHECK_INTERVAL || lastBatteryCheckTime == 0) {
    lastBatteryCheckTime = currentMillis;
    
    long sumADC = 0;
    const int NUM_SAMPLES = 10;
    for (int i = 0; i < NUM_SAMPLES; i++) {
      sumADC += analogRead(BATTERY_PIN);
      delayMicroseconds(200); 
    }
    float avgADC = (float)sumADC / NUM_SAMPLES;

    float pinVoltage = (avgADC / 1023.0) * 3.3;
    float currentBatVoltage = pinVoltage * (133.0 / 33.0); 
    
    if (smoothedBatVoltage < 0) {
      smoothedBatVoltage = currentBatVoltage; 
    } else {
      smoothedBatVoltage = (smoothedBatVoltage * 0.90) + (currentBatVoltage * 0.10);
    }

    float pct = ((smoothedBatVoltage - 3.2) / (4.2 - 3.2)) * 100.0;
    if (pct > 100.0) pct = 100.0;
    if (pct < 0.0) pct = 0.0;
    batteryPercent = (int)pct;
  }

  // --- バッテリー残量描画 ---
  display.setTextSize(1);
  display.setCursor(110, 0); 
  display.print(batteryPercent);
  display.print("%");

  display.display();
}