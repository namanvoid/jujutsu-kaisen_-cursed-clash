#pragma once

#include "Types.hpp"
#include <raylib.h>
#include <map>

namespace CursedClash {

class Audio {
public:
    static void init();
    static void cleanup();
    static void setVolumes(float music, float sfx);
    static void setMuted(bool muted);
    static bool isMuted();
    static void playSfx(SoundEffect sfx);
    static void update(float dt);
    static void startMusic(const std::string& theme);
    static void stopMusic();
};

} // namespace CursedClash
