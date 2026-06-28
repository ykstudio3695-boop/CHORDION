/*
  Hardware target
  - Daisy Seed
  - 128 x 32 SSD1306 OLED over I2C
  - 7 key buttons, mode button, output/bank button
  - Volume, pitch, and battery analog inputs

  This sketch keeps the behavior of teensy.ino, but replaces the Teensy Audio
  Library graph with a Daisy Seed audio callback and small local DSP engine.
*/

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DaisyDuino.h>

#include "AudioSampleTom_l.h"
#include "AudioSampleTom_m.h"
#include "AudioSampleTom_h.h"
#include "AudioSampleSnare.h"
#include "AudioSampleKick.h"
#include "AudioSampleHihat.h"
#include "AudioSampleCymbal.h"

#ifndef D0
#define D0 0
#define D1 1
#define D2 2
#define D3 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define D8 8
#endif

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DaisyHardware hw;

const int NUM_KEYS = 7;

// Daisy Seed pin assignment. Change these to match your carrier board wiring.
const int buttons[NUM_KEYS] = {D0, D1, D2, D3, D4, D5, D6};
const int MODE_BUTTON = D7;
const int OUTPUT_SWITCH_PIN = D8;
const int VOLUME_PIN = A0;
const int PITCH_PIN = A1;
const int BATTERY_PIN = A2;

const float AUDIO_SAMPLE_RATE = 48000.0f;
const float DRUM_SOURCE_RATE = 44100.0f;
const float DRUM_STEP = DRUM_SOURCE_RATE / AUDIO_SAMPLE_RATE;
const float TWO_PI_F = 6.28318530718f;
const int ANALOG_MAX = 1023;

enum PlayMode { MODE_CHORD, MODE_ARP, MODE_DRUM, MODE_MONO };
enum WaveformType { WAVEFORM_SINE, WAVEFORM_SAWTOOTH, WAVEFORM_SQUARE, WAVEFORM_TRIANGLE };

struct InstrumentPatch {
  const char* name;
  WaveformType waveform;
  float attack;
  float decay;
  float sustain;
  float release;
};

struct ChordData {
  float freqs[4];
  const char* name;
};

struct ChordBank {
  const char* bankName;
  ChordData chords[NUM_KEYS];
};

const InstrumentPatch myPatches[] = {
  {"Lead (Saw)",   WAVEFORM_SAWTOOTH, 10.0,  100.0, 0.8,  200.0},
  {"Pluck (Sine)", WAVEFORM_SINE,      1.0,  150.0, 0.0,  100.0},
  {"8-bit (Sq)",   WAVEFORM_SQUARE,    5.0,  100.0, 0.5,  100.0},
  {"Pad (Tri)",    WAVEFORM_TRIANGLE, 400.0, 200.0, 1.0,  800.0}
};
const int TOTAL_PATCHES = sizeof(myPatches) / sizeof(InstrumentPatch);
int currentPatchIndex = 0;

const ChordBank myProgressionList[] = {
  {
    "Key of C (Pop/Std)",
    {
      {{261.63, 329.63, 392.00, 493.88}, "Cmaj7"},
      {{293.66, 349.23, 440.00, 523.25}, "Dm7"},
      {{329.63, 392.00, 493.88, 587.33}, "Em7"},
      {{349.23, 440.00, 523.25, 659.25}, "Fmaj7"},
      {{392.00, 493.88, 587.33, 698.46}, "G7"},
      {{440.00, 523.25, 659.25, 783.99}, "Am7"},
      {{493.88, 587.33, 698.46, 880.00}, "Bm7b5"}
    }
  },
  {
    "Key of F (Jazz/R&B)",
    {
      {{174.61, 220.00, 261.63, 329.63}, "Fmaj7"},
      {{196.00, 233.08, 293.66, 349.23}, "Gm7"},
      {{220.00, 261.63, 329.63, 392.00}, "Am7"},
      {{233.08, 293.66, 349.23, 440.00}, "Bbmaj7"},
      {{261.63, 329.63, 392.00, 466.16}, "C7"},
      {{293.66, 349.23, 440.00, 523.25}, "Dm7"},
      {{329.63, 392.00, 466.16, 587.33}, "Em7b5"}
    }
  },
  {
    "Key of G (Acoustic)",
    {
      {{196.00, 246.94, 293.66, 369.99}, "Gmaj7"},
      {{220.00, 261.63, 329.63, 392.00}, "Am7"},
      {{246.94, 293.66, 369.99, 440.00}, "Bm7"},
      {{261.63, 329.63, 392.00, 493.88}, "Cmaj7"},
      {{293.66, 369.99, 440.00, 523.25}, "D7"},
      {{329.63, 392.00, 493.88, 587.33}, "Em7"},
      {{369.99, 440.00, 523.25, 659.25}, "F#m7b5"}
    }
  }
};
const int TOTAL_BANKS = sizeof(myProgressionList) / sizeof(ChordBank);

const char* noteNames[4] = {"Root", "3rd", "5th", "7th"};
const char* drumNames[NUM_KEYS] = {"Cymbal", "Tom M", "Tom H", "Tom L", "Snare", "Kick", "Hihat"};

const unsigned int* drumSamples[NUM_KEYS] = {
  AudioSampleCymbal,
  AudioSampleTom_m,
  AudioSampleTom_h,
  AudioSampleTom_l,
  AudioSampleSnare,
  AudioSampleKick,
  AudioSampleHihat
};

const unsigned char logo[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x78, 0x7c, 0x7c, 0x78, 0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x40, 0x40, 0x48, 0x40, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x78, 0x78, 0x48, 0x78, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x48, 0x40, 0x40, 0x48, 0x40, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x78, 0x7c, 0x7c, 0x78, 0x7c, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct Voice {
  float phase;
  float frequency;
  float level;
  float attackMs;
  float decayMs;
  float sustain;
  float releaseMs;
  float releaseStart;
  WaveformType waveform;
  bool gate;
  uint8_t stage;
};

struct DrumVoice {
  const unsigned int* sample;
  uint32_t lengthBytes;
  float position;
  bool active;
};

Voice voices[NUM_KEYS][8];
DrumVoice drumVoices[NUM_KEYS][2];
int drumVoiceIndex[NUM_KEYS] = {0};

float reverbBuffer[4096] = {0.0f};
uint16_t reverbIndex = 0;

bool keyState[NUM_KEYS] = {false};
bool lastUIBtnState[NUM_KEYS] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool lastModeBtn = HIGH;
bool lastOutputBtn = HIGH;

int activeDisplayKey = -1;
int currentBankIndex = 0;
int selectedBankIndex = 0;
bool isBankSelectMode = false;
bool isHeadphoneActive = true;

float envAttack = 500.0;
float envDecay = 120.0;
float envSustain = 1.0;
float envRelease = 500.0;
float smoothedPitchRaw = 512.0;
float pitchMultiplier = 1.0;
float masterVolume = 0.0;

PlayMode currentMode = MODE_CHORD;

unsigned long lastArpTime[NUM_KEYS] = {0};
int arpIndex[NUM_KEYS] = {0};
bool arpActive[NUM_KEYS] = {false};
int currentArpNote[NUM_KEYS] = {0};
bool arpNoteVisible[NUM_KEYS] = {false};
const int arpInterval = 100;

int lastRawVol = -1;
float smoothedVol = -1.0;
unsigned long volDisplayStartTime = 0;
bool isVolDisplayActive = false;
const int VOL_THRESHOLD = 5;
const unsigned long VOL_SHOW_TIME = 800;

unsigned long lastOutBtnPressTime = 0;
bool outBtnPending = false;
const unsigned long DOUBLE_CLICK_TIME = 250;

unsigned long lastBatteryCheckTime = 0;
const unsigned long BATTERY_CHECK_INTERVAL = 1000;
int batteryPercent = 0;
float smoothedBatVoltage = -1.0;

bool modeBlink = false;
unsigned long modeBlinkStart = 0;
bool modeBlinkVisible = true;
const int MODE_BLINK_COUNT = 3;
const int MODE_BLINK_INTERVAL = 50;

static inline uint32_t sampleLengthBytes(const unsigned int* sample) {
  return sample[0] & 0x00FFFFFFUL;
}

static inline int16_t decodeULaw(uint8_t uval) {
  uval = ~uval;
  int t = ((uval & 0x0F) << 3) + 132;
  t <<= ((unsigned)uval & 0x70) >> 4;
  return (uval & 0x80) ? (132 - t) : (t - 132);
}

static inline uint8_t sampleByte(const unsigned int* sample, uint32_t index) {
  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(sample + 1);
  return bytes[index];
}

static inline float waveformSample(WaveformType waveform, float phase) {
  switch (waveform) {
    case WAVEFORM_SINE:
      return sinf(TWO_PI_F * phase);
    case WAVEFORM_SQUARE:
      return phase < 0.5f ? 1.0f : -1.0f;
    case WAVEFORM_TRIANGLE:
      return 4.0f * fabsf(phase - 0.5f) - 1.0f;
    case WAVEFORM_SAWTOOTH:
    default:
      return (phase * 2.0f) - 1.0f;
  }
}

void voiceNoteOn(Voice& v, WaveformType waveform, float frequency) {
  v.waveform = waveform;
  v.frequency = frequency;
  v.gate = true;
  v.stage = 1;
}

void voiceNoteOff(Voice& v) {
  if (v.stage != 0) {
    v.gate = false;
    v.releaseStart = v.level;
    v.stage = 4;
  }
}

float renderEnvelope(Voice& v) {
  const float attackStep = v.attackMs <= 0.0f ? 1.0f : 1000.0f / (v.attackMs * AUDIO_SAMPLE_RATE);
  const float decayStep = v.decayMs <= 0.0f ? 1.0f : 1000.0f / (v.decayMs * AUDIO_SAMPLE_RATE);
  const float releaseStep = v.releaseMs <= 0.0f ? 1.0f : 1000.0f / (v.releaseMs * AUDIO_SAMPLE_RATE);

  switch (v.stage) {
    case 1:
      v.level += attackStep;
      if (v.level >= 1.0f) {
        v.level = 1.0f;
        v.stage = 2;
      }
      break;
    case 2:
      v.level -= decayStep * (1.0f - v.sustain);
      if (v.level <= v.sustain) {
        v.level = v.sustain;
        v.stage = 3;
      }
      break;
    case 3:
      v.level = v.sustain;
      break;
    case 4:
      v.level -= releaseStep * max(v.releaseStart, 0.001f);
      if (v.level <= 0.0f) {
        v.level = 0.0f;
        v.stage = 0;
      }
      break;
    default:
      v.level = 0.0f;
      break;
  }

  return v.level;
}

float renderVoice(Voice& v) {
  if (v.stage == 0) return 0.0f;

  float env = renderEnvelope(v);
  float out = waveformSample(v.waveform, v.phase) * env * 0.32f;
  v.phase += v.frequency / AUDIO_SAMPLE_RATE;
  while (v.phase >= 1.0f) v.phase -= 1.0f;
  return out;
}

float renderDrumVoice(DrumVoice& v) {
  if (!v.active || v.sample == nullptr) return 0.0f;

  uint32_t index = (uint32_t)v.position;
  if (index >= v.lengthBytes) {
    v.active = false;
    return 0.0f;
  }

  float sample = (float)decodeULaw(sampleByte(v.sample, index)) / 32768.0f;
  v.position += DRUM_STEP;
  return sample;
}

static inline float softClip(float x) {
  if (x > 1.0f) return 1.0f;
  if (x < -1.0f) return -1.0f;
  return x;
}

float renderReverb(float dry) {
  float wet = reverbBuffer[reverbIndex];
  reverbBuffer[reverbIndex] = softClip(dry + (wet * 0.55f));
  reverbIndex = (reverbIndex + 1) & 4095;
  return (dry * 0.55f) + (wet * 0.55f);
}

void AudioCallback(float** in, float** out, size_t size) {
  (void)in;

  for (size_t i = 0; i < size; i++) {
    float synthMixA = 0.0f;
    float synthMixB = 0.0f;
    float drumMixA = 0.0f;
    float drumMixB = 0.0f;

    for (int k = 0; k < NUM_KEYS; k++) {
      float high = 0.0f;
      float low = 0.0f;
      for (int v = 0; v < 4; v++) high += renderVoice(voices[k][v]) * 0.75f;
      for (int v = 4; v < 8; v++) low += renderVoice(voices[k][v]) * 0.20f;

      float keyMix = (high * 0.4f) + low;
      if (k < 4) synthMixA += keyMix * 0.25f;
      else synthMixB += keyMix * 0.25f;

      float drumMix = 0.0f;
      for (int v = 0; v < 2; v++) drumMix += renderDrumVoice(drumVoices[k][v]) * 0.7f;
      if (k < 4) drumMixA += drumMix * 0.3f;
      else drumMixB += drumMix * 0.3f;
    }

    float dry = (synthMixA * 0.4f) + (synthMixB * 0.4f) + (drumMixA * 0.3f) + (drumMixB * 0.3f);
    float output = softClip(renderReverb(dry) * masterVolume);

    out[0][i] = output;
    out[1][i] = output;
  }
}

void applyEnvelopeSettings(float a, float d, float s, float r) {
  envAttack = a;
  envDecay = d;
  envSustain = s;
  envRelease = r;

  for (int k = 0; k < NUM_KEYS; k++) {
    for (int i = 0; i < 8; i++) {
      voices[k][i].attackMs = envAttack;
      voices[k][i].decayMs = envDecay;
      voices[k][i].sustain = envSustain;
      voices[k][i].releaseMs = envRelease;
    }
  }
}

void resetArpState(int k) {
  for (int i = 0; i < 8; i++) voiceNoteOff(voices[k][i]);
  arpIndex[k] = 0;
  arpActive[k] = false;
  arpNoteVisible[k] = false;
}

void triggerDrum(int k) {
  int v = drumVoiceIndex[k];
  drumVoices[k][v].sample = drumSamples[k];
  drumVoices[k][v].lengthBytes = sampleLengthBytes(drumSamples[k]);
  drumVoices[k][v].position = 0.0f;
  drumVoices[k][v].active = true;
  drumVoiceIndex[k] = (drumVoiceIndex[k] + 1) % 2;
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

void startChord(int k) {
  applyEnvelopeSettings(100.0, 120.0, 1.0, 500.0);
  for (int v = 0; v < 4; v++) {
    float f = myProgressionList[currentBankIndex].chords[k].freqs[v];
    float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
    voiceNoteOn(voices[k][v], WAVEFORM_SAWTOOTH, (f / 2.0f) * pitchMultiplier);
    voiceNoteOn(voices[k][v + 4], WAVEFORM_SAWTOOTH, (rootF / 4.0f) * pitchMultiplier);
  }
}

void startArp(int k) {
  applyEnvelopeSettings(1.0, 1.0, 1.0, 100.0);
  arpActive[k] = true;
  arpIndex[k] = 0;
  lastArpTime[k] = millis();

  int n = 0;
  float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
  float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
  voiceNoteOn(voices[k][n], WAVEFORM_SAWTOOTH, (f / 2.0f) * pitchMultiplier);
  voiceNoteOn(voices[k][n + 4], WAVEFORM_SAWTOOTH, (rootF / 8.0f) * pitchMultiplier);

  currentArpNote[k] = n;
  arpNoteVisible[k] = true;
  arpIndex[k] = 1;
}

void startMono(int k) {
  InstrumentPatch patch = myPatches[currentPatchIndex];
  applyEnvelopeSettings(patch.attack, patch.decay, patch.sustain, patch.release);

  float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
  voiceNoteOn(voices[k][0], patch.waveform, (rootF / 2.0f) * pitchMultiplier);
  voiceNoteOn(voices[k][4], patch.waveform, (rootF / 4.0f) * pitchMultiplier);
}

void updateChordPitch(int k) {
  for (int v = 0; v < 4; v++) {
    float f = myProgressionList[currentBankIndex].chords[k].freqs[v];
    float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
    voices[k][v].frequency = (f / 2.0f) * pitchMultiplier;
    voices[k][v + 4].frequency = (rootF / 4.0f) * pitchMultiplier;
  }
}

void updateArpPitch(int k) {
  int n = currentArpNote[k];
  float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
  float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
  voices[k][n].frequency = (f / 2.0f) * pitchMultiplier;
  voices[k][n + 4].frequency = (rootF / 8.0f) * pitchMultiplier;
}

void updateMonoPitch(int k) {
  float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
  voices[k][0].frequency = (rootF / 2.0f) * pitchMultiplier;
  voices[k][4].frequency = (rootF / 4.0f) * pitchMultiplier;
}

void setup() {
  analogReadResolution(10);

  for (int i = 0; i < NUM_KEYS; i++) pinMode(buttons[i], INPUT_PULLUP);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(OUTPUT_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BATTERY_PIN, INPUT);

  for (int k = 0; k < NUM_KEYS; k++) {
    for (int i = 0; i < 8; i++) {
      voices[k][i].phase = 0.0f;
      voices[k][i].frequency = 220.0f;
      voices[k][i].level = 0.0f;
      voices[k][i].waveform = WAVEFORM_SAWTOOTH;
      voices[k][i].stage = 0;
    }
  }
  applyEnvelopeSettings(envAttack, envDecay, envSustain, envRelease);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.drawBitmap(0, 0, logo, 128, 5, SSD1306_WHITE);
  drawCenter("DAISY SEED", 16, 1);
  display.display();
  delay(1200);

  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  hw.StartAudio(AudioCallback);
}

void loop() {
  int rawVol = analogRead(VOLUME_PIN);
  if (smoothedVol < 0) smoothedVol = rawVol;
  smoothedVol = (smoothedVol * 0.8f) + (rawVol * 0.2f);

  int filteredVol = (int)smoothedVol;
  if (filteredVol > 1005) filteredVol = ANALOG_MAX;
  if (filteredVol < 15) filteredVol = 0;

  if (lastRawVol == -1 || abs(filteredVol - lastRawVol) > VOL_THRESHOLD) {
    lastRawVol = filteredVol;
    isVolDisplayActive = true;
    volDisplayStartTime = millis();
  }

  if (isVolDisplayActive && (millis() - volDisplayStartTime > VOL_SHOW_TIME)) {
    isVolDisplayActive = false;
  }

  float volumeScale = (float)filteredVol / (float)ANALOG_MAX;
  masterVolume = isHeadphoneActive ? volumeScale * 0.9f : volumeScale * 1.8f;

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

    for (int r = 0; r < 2; r++) {
      display.clearDisplay();
      drawCenter(isHeadphoneActive ? "HEADPHONE" : "SPEAKER", 12, 2);
      display.display();
      delay(100);
      display.clearDisplay();
      display.display();
      delay(100);
    }
  }

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
  } else {
    bool modeBtn = digitalRead(MODE_BUTTON);
    if (lastModeBtn == HIGH && modeBtn == LOW) {
      currentMode = (PlayMode)((currentMode + 1) % 4);
      for (int k = 0; k < NUM_KEYS; k++) resetArpState(k);
      modeBlink = true;
      modeBlinkStart = millis();
      modeBlinkVisible = true;
    }
    lastModeBtn = modeBtn;

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
      if (currentMode == MODE_CHORD) display.print("chord");
      else if (currentMode == MODE_ARP) display.print("arp");
      else if (currentMode == MODE_DRUM) display.print("drum");
      else display.print("mono");
    }

    int rawPitch = analogRead(PITCH_PIN);
    smoothedPitchRaw = (smoothedPitchRaw * 0.8f) + (rawPitch * 0.2f);
    float pitchOctave = ((smoothedPitchRaw - 512.0f) / 512.0f);
    pitchMultiplier = powf(2.0f, pitchOctave);

    for (int k = 0; k < NUM_KEYS; k++) {
      bool pressed = (digitalRead(buttons[k]) == LOW);

      if (pressed && !keyState[k]) {
        keyState[k] = true;
        if (activeDisplayKey == -1) activeDisplayKey = k;

        if (currentMode == MODE_CHORD) startChord(k);
        else if (currentMode == MODE_ARP) startArp(k);
        else if (currentMode == MODE_DRUM) triggerDrum(k);
        else if (currentMode == MODE_MONO) startMono(k);
      }

      if (currentMode == MODE_ARP && pressed && arpActive[k]) {
        if (millis() - lastArpTime[k] > arpInterval) {
          for (int i = 0; i < 8; i++) voiceNoteOff(voices[k][i]);

          int n = arpIndex[k] % 4;
          float f = myProgressionList[currentBankIndex].chords[k].freqs[n];
          float rootF = myProgressionList[currentBankIndex].chords[k].freqs[0];
          voiceNoteOn(voices[k][n], WAVEFORM_SAWTOOTH, (f / 2.0f) * pitchMultiplier);
          voiceNoteOn(voices[k][n + 4], WAVEFORM_SAWTOOTH, (rootF / 8.0f) * pitchMultiplier);

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
            if (keyState[i]) {
              activeDisplayKey = i;
              break;
            }
          }
        }
      }

      if (pressed && currentMode == MODE_CHORD) {
        if (k == activeDisplayKey) drawCenter(myProgressionList[currentBankIndex].chords[k].name, random(19, 21), 1);
        updateChordPitch(k);
      }

      if (pressed && currentMode == MODE_ARP && arpNoteVisible[k]) {
        if (k == activeDisplayKey) {
          drawCenter(noteNames[currentArpNote[k]], 12, 1);
          drawCenter(myProgressionList[currentBankIndex].chords[k].name, 22, 1);
        }
        updateArpPitch(k);
      }

      if (pressed && currentMode == MODE_DRUM) {
        if (k == activeDisplayKey) drawCenter(drumNames[k], 20, 1);
      }

      if (pressed && currentMode == MODE_MONO) {
        if (k == activeDisplayKey) {
          drawCenter(myPatches[currentPatchIndex].name, 8, 1);
          drawCenter(myProgressionList[currentBankIndex].chords[k].name, 20, 1);
        }
        updateMonoPitch(k);
      }
    }
  }

  if (isVolDisplayActive) {
    display.clearDisplay();
    int volPercent = (filteredVol * 100) / ANALOG_MAX;
    String volStr = "VOLUME : " + String(volPercent) + " %";
    drawCenter(volStr, 14, 1);
  }

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
    float pinVoltage = (avgADC / (float)ANALOG_MAX) * 3.3f;
    float currentBatVoltage = pinVoltage * (133.0f / 33.0f);

    if (smoothedBatVoltage < 0) smoothedBatVoltage = currentBatVoltage;
    else smoothedBatVoltage = (smoothedBatVoltage * 0.90f) + (currentBatVoltage * 0.10f);

    float pct = ((smoothedBatVoltage - 3.2f) / (4.2f - 3.2f)) * 100.0f;
    if (pct > 100.0f) pct = 100.0f;
    if (pct < 0.0f) pct = 0.0f;
    batteryPercent = (int)pct;
  }

  display.setTextSize(1);
  display.setCursor(110, 0);
  display.print(batteryPercent);
  display.print("%");
  display.display();
}
