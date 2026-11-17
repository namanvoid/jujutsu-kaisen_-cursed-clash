#include "Audio.hpp"
#include <cmath>
#include <vector>
#include <cstdlib>
#include <algorithm>

namespace CursedClash {

static bool audioReady = false;
static float musicVol = 0.5f;
static float sfxVol = 0.7f;
static bool isAudioMuted = false;
static std::map<SoundEffect, Sound> sfxMap;

static Sound generateTone(float startFreq, float endFreq, float duration, float volume, bool noise = false) {
    if (!audioReady) return Sound{};
    int sampleRate = 44100;
    int frameCount = (int)(sampleRate * duration);
    std::vector<short> samples(frameCount);

    for (int i = 0; i < frameCount; ++i) {
        float t = (float)i / frameCount;
        float freq = startFreq + (endFreq - startFreq) * t;
        float amp = (1.0f - t) * volume;
        float sampleVal = 0.0f;
        if (noise) {
            float n = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
            sampleVal = n * amp;
        } else {
            float phase = 2.0f * 3.14159265f * freq * ((float)i / sampleRate);
            sampleVal = std::sin(phase) * amp;
        }
        samples[i] = (short)std::clamp(sampleVal * 32767.0f, -32767.0f, 32767.0f);
    }

    Wave wave{};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = samples.data();

    Sound snd = LoadSoundFromWave(wave);
    return snd;
}

void Audio::init() {
    InitAudioDevice();
    audioReady = IsAudioDeviceReady();
    if (!audioReady) return;

    sfxMap[SoundEffect::UiClick] = generateTone(440, 880, 0.05f, 0.4f);
    sfxMap[SoundEffect::LightHit] = generateTone(200, 60, 0.08f, 0.6f);
    sfxMap[SoundEffect::HeavyHit] = generateTone(140, 40, 0.16f, 0.8f);
    sfxMap[SoundEffect::Block] = generateTone(300, 150, 0.06f, 0.5f);
    sfxMap[SoundEffect::Parry] = generateTone(980, 1960, 0.25f, 0.7f);
    sfxMap[SoundEffect::Dash] = generateTone(1200, 300, 0.11f, 0.5f, true);
    sfxMap[SoundEffect::Jump] = generateTone(160, 360, 0.12f, 0.5f);
    sfxMap[SoundEffect::Blue] = generateTone(80, 160, 0.25f, 0.6f);
    sfxMap[SoundEffect::Red] = generateTone(520, 180, 0.25f, 0.7f);
    sfxMap[SoundEffect::Purple] = generateTone(120, 30, 0.45f, 0.9f);
    sfxMap[SoundEffect::Infinity] = generateTone(660, 880, 0.20f, 0.6f);
    sfxMap[SoundEffect::Void] = generateTone(330, 660, 0.60f, 0.8f);
    sfxMap[SoundEffect::Katana] = generateTone(900, 400, 0.12f, 0.6f, true);
    sfxMap[SoundEffect::Heal] = generateTone(350, 700, 0.30f, 0.6f);
    sfxMap[SoundEffect::Ice] = generateTone(800, 300, 0.18f, 0.6f, true);
    sfxMap[SoundEffect::Lightning] = generateTone(600, 150, 0.18f, 0.7f, true);
    sfxMap[SoundEffect::Drum] = generateTone(120, 50, 0.15f, 0.7f);
    sfxMap[SoundEffect::Fire] = generateTone(250, 80, 0.22f, 0.6f, true);
    sfxMap[SoundEffect::Blood] = generateTone(180, 70, 0.15f, 0.6f);
    sfxMap[SoundEffect::Blade] = generateTone(1100, 400, 0.12f, 0.6f, true);
    sfxMap[SoundEffect::Roots] = generateTone(140, 50, 0.20f, 0.6f);
    sfxMap[SoundEffect::Slash] = generateTone(1000, 300, 0.14f, 0.7f, true);
    sfxMap[SoundEffect::Shrine] = generateTone(150, 40, 0.70f, 0.9f);
    sfxMap[SoundEffect::Domain] = generateTone(220, 55, 0.80f, 0.9f);
    sfxMap[SoundEffect::RoundStart] = generateTone(440, 660, 0.35f, 0.7f);
    sfxMap[SoundEffect::KO] = generateTone(150, 30, 0.60f, 0.9f);
    sfxMap[SoundEffect::Victory] = generateTone(330, 880, 0.50f, 0.8f);
    sfxMap[SoundEffect::Defeat] = generateTone(440, 110, 0.50f, 0.8f);
}

void Audio::cleanup() {
    if (!audioReady) return;
    for (auto& pair : sfxMap) {
        if (pair.second.frameCount > 0) {
            UnloadSound(pair.second);
        }
    }
    sfxMap.clear();
    CloseAudioDevice();
    audioReady = false;
}

void Audio::setVolumes(float music, float sfx) {
    musicVol = std::clamp(music, 0.0f, 1.0f);
    sfxVol = std::clamp(sfx, 0.0f, 1.0f);
}

void Audio::setMuted(bool muted) {
    isAudioMuted = muted;
}

bool Audio::isMuted() {
    return isAudioMuted;
}

void Audio::playSfx(SoundEffect sfx) {
    if (!audioReady || isAudioMuted || sfxVol <= 0.01f) return;
    auto it = sfxMap.find(sfx);
    if (it != sfxMap.end() && it->second.frameCount > 0) {
        SetSoundVolume(it->second, sfxVol);
        PlaySound(it->second);
    }
}

void Audio::update(float dt) { (void)dt; }
void Audio::startMusic(const std::string& theme) { (void)theme; }
void Audio::stopMusic() {}

} // namespace CursedClash
