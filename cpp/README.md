# Jujutsu Kaisen: Cursed Clash — Native C++ & Raylib Port

A high-performance, native 2D pixel-art fighting game inspired by *Jujutsu Kaisen* and arcade fighting games, ported from TypeScript/React/Canvas to modern **C++17** and **Raylib**.

---

## Technical Stack & Architecture

* **Language:** C++17
* **Framework:** [Raylib](https://www.raylib.com/) (Windowing, 2D Graphics, Input, Audio)
* **Build Systems:** Makefile & CMake (`CMakeLists.txt`)
* **Rendering Architecture:**
  * Virtual resolution of **480x270** rendered into an offscreen `RenderTexture2D`.
  * Upscaled to native window resolutions (960x540 default, resizable) with `TEXTURE_FILTER_POINT` (nearest-neighbor) for razor-sharp retro pixel art without bilinear blur.
* **Audio Architecture:**
  * Procedural synthesized 16-bit sound generation (impact booms, laser beams, parry rings, spatial hums, and domain expansion rumbles) using Raylib's `Wave` API.
* **Procedural Pixel Art:**
  * 56x64 procedural character pixel grids generated algorithmically at runtime for every character and pose (`idle0/1`, `walk0..3`, `jump`, `dash`, `sidestep`, `block`, `parry`, `light1..3`, `heavy`, `special`, `ultimate`, `hitstun`, `ko`).

---

## Directory Structure

```
├── Makefile                # Root Makefile (delegates to cpp/)
├── bin/
│   └── cursed_clash        # Compiled native executable
└── cpp/
    ├── Makefile            # Standalone Makefile
    ├── CMakeLists.txt      # CMake build configuration
    ├── include/
    │   ├── Types.hpp       # Enums, constants, FighterState, MoveSpec, InputState
    │   ├── Roster.hpp      # Character profiles, skill lists, arenas, story stages
    │   ├── Rules.hpp       # Frame data, hit resolution, camera math, combo scaling
    │   ├── PixelSprites.hpp# Procedural 56x64 pixel sprite generator & palette cache
    │   ├── PixelArenas.hpp # Procedural arena backdrop generator & domain effects
    │   ├── Audio.hpp       # Procedural sound synthesis engine
    │   ├── Ai.hpp          # Character-specific villain AI decision tree
    │   ├── Engine.hpp      # Simulation game loop, projectiles, hazards, stage logic
    │   ├── Renderer.hpp    # 2D world renderer, camera transform, afterimages
    │   └── UI.hpp          # Title screen, fighting game combat HUD, modals
    └── src/
        ├── main.cpp        # Entry point & window event loop
        ├── Roster.cpp
        ├── Rules.cpp
        ├── PixelSprites.cpp
        ├── PixelArenas.cpp
        ├── Audio.cpp
        ├── Ai.cpp
        ├── Engine.cpp
        ├── Renderer.cpp
        └── UI.cpp
```

---

## Building and Running

### Prerequisites
* **macOS:**
  ```bash
  brew install raylib
  ```
* **Linux (Ubuntu/Debian):**
  ```bash
  sudo apt install libraylib-dev libgl1-mesa-dev
  ```

### Quick Build & Run
From the project root:
```bash
# Build the executable
make

# Run the game
make run

# Run headless simulation test
make test
```

Or directly run the compiled binary:
```bash
./bin/cursed_clash
```

---

## Controls

| Action | Primary Key | Alternate Key |
| :--- | :--- | :--- |
| **Move Left / Right** | `A` / `D` | `Left` / `Right` Arrow |
| **Jump** | `W` | `Up` Arrow |
| **Dodge / Sidestep** | `S` | `Down` Arrow |
| **Dash** | `Space` | — |
| **Block** | `L` | `Right Shift` |
| **Parry (Deflect)** | `Q` | `F` |
| **Light Attack Combo** | `J` | — |
| **Heavy Launcher** | `K` | — |
| **Skill 1** | `U` | Character specific |
| **Skill 2** | `I` | Character specific |
| **Skill 3** | `O` | Character specific |
| **Skill 4 (Stance / Buff / RCT)** | `H` | Character specific |
| **Skill 5 (Domain Expansion)** | `Y` | Requires 100 Cursed Energy |
| **Pause Menu** | `Escape` | `P` |

---

## Features

1. **14 Characters:**
   * **Sorcerers:** Satoru Gojo, Yuta Okkotsu, Megumi Fushiguro, Panda
   * **Villains:** Jogo, Mahito, Hanami, Choso, Suguru Geto, Uraume, Hajime Kashimo, Toji Fushiguro, Kenjaku, Ryomen Sukuna
2. **5 Arenas:**
   * Tokyo Metropolitan Jujutsu High Courtyard
   * Abandoned School Cursed Corridors
   * Cursed Forest of Kyoto
   * Shibuya Station Crossing (October 31)
   * Malevolent Shrine (Innate Domain)
3. **Combat Mechanics:**
   * Frame data with Startup, Active, Recovery, Hitstun, Blockstun, and Hitstop freeze-frames.
   * Just-Parry window with deflection ring and frame advantage.
   * Gojo's Limitless Infinity barrier deflecting non-piercing hits.
   * 3-Phase Sukuna Boss encounter featuring adaptive Cleave/Dismantle and ambient domain slashes.
   * 10-Stage Story Campaign and Free Practice Mode.
