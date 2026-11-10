# Bound By Death

**Two souls. One fate.**

A cooperative action hack-and-slash adventure developed by **Ravenwhisp Studios**, featuring two characters bound together by a story about mortality, sacrifice, and the fear of letting go.

[![Ravenwhisp Studios](https://img.shields.io/badge/Developer-Ravenwhisp%20Studios-7b526f)](https://ravenwhisp.github.io/Ravenwhisp/)
[![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)](https://isocpp.org/)
[![Engine](https://img.shields.io/badge/Engine-Custom%20Engine-555555)](https://github.com/Ravenwhisp/BoundByDeath)

**[Official Website](https://ravenwhisp.github.io/Ravenwhisp/) · [GitHub Repository](https://github.com/Ravenwhisp/BoundByDeath) · [Releases](https://github.com/Ravenwhisp/BoundByDeath/releases)** · [Itch.io Downlaod Release](https://ravenwhisp-studios.itch.io/bound-by-death)

---

## 1. About the Game

**Bound By Death** is a 3D cooperative action hack-and-slash game developed by Ravenwhisp Studios using a custom C++ game engine.

Players step into a fantasy world and take on the roles of **Lyriel**, the elven heir to the throne, and **Death itself**, two characters whose destinies become intertwined.

Through combat, exploration, environmental interactions, and cooperative gameplay, players experience a narrative exploring the fear of letting go and the consequences of defying mortality.

The project combines character-driven gameplay, enemy encounters, environmental challenges, and a custom-engine development approach.

## 2. Game Overview

Bound By Death is designed around two playable characters with different abilities and gameplay roles.

The game combines action-oriented combat with exploration and environmental interactions.

### Core Gameplay

The gameplay systems include:

- **Cooperative gameplay:** Two playable characters designed to work together.
- **Character abilities:** Distinct attacks, charged attacks, and movement abilities.
- **Combat:** Real-time encounters against enemies and bosses.
- **Enemy AI:** Enemy behaviour and state-based combat systems.
- **Environmental interaction:** Breakable objects, puzzles, traps, and interactive elements.
- **Level progression:** Objectives, checkpoints, and progression systems.
- **Gameplay feedback:** Visual effects, sound effects, animations, and haptic feedback.

The repository includes systems supporting local multiplayer, including configurations for keyboard and gamepad or two gamepads.

## 3. Story and Setting

In a world where mortality and destiny are deeply intertwined, Lyriel, the elven heir to the throne, finds herself connected to Death itself.

Their journey explores the consequences of challenging the natural order and the difficulty of accepting what must eventually be let go.

The story revolves around themes of mortality, loss, attachment, and the boundaries between life and death.

Players experience this narrative through the game's characters, environments, encounters, and progression.

## 4. Characters

### Lyriel

The elven heir to the throne and one of the two playable protagonists.

Lyriel has her own character abilities and combat systems, including ranged attacks, charged attacks, and movement abilities.

### Death

The personification of Death and Lyriel's companion throughout the adventure.

Death is the second playable protagonist, with a distinct combat and ability system.

Both characters have dedicated gameplay scripts, character configurations, UI elements, and audiovisual feedback systems.

## 5. Features

The following list describes functionality represented in the current repository. It distinguishes implemented code systems from features that cannot be independently confirmed as fully playable.

### Gameplay Systems

- Two playable character systems: Lyriel and Death.
- Character-specific attack and ability classes.
- Basic and charged attack systems.
- Dash and movement-related abilities.
- Projectile-based combat.
- Damage, health, and character-state management.
- Enemy AI and state machines.
- Boss encounter controllers.
- Puzzle management systems.
- Breakable and interactive environmental objects.
- Checkpoint and progression management.
- Gameplay events and objective systems.
- Local multiplayer input configuration.

### Technical and Presentation Systems

- Custom C++ game engine integration.
- Particle and visual effects management.
- Audio event and music management.
- UI and HUD systems.
- Camera behaviour and transitions.
- Gameplay haptics and controller feedback.
- Runtime gameplay configuration and engine API integration.

> **Development note:** The presence of a system or class in the source code does not necessarily mean every associated feature is complete, fully integrated, or validated in a playable build.

## 6. Technologies and Tools

Bound By Death is developed using a custom game engine and a C++ gameplay scripting project.

| Technology | Purpose |
|---|---|
| C++ | Gameplay programming and engine integration |
| Custom Game Engine | Runtime, rendering, engine systems, and game execution |
| Microsoft Visual Studio | C++ development environment |
| MSVC v143 | Visual Studio platform toolset |
| C++20 | Configured C++ language standard |
| DirectX SimpleMath | Mathematical operations and graphics-related types |
| RapidJSON | JSON parsing and serialization support |
| Wwise integration | Audio events and music management |
| Git | Version control |
| GitHub | Repository hosting and collaboration |

### Development Environment

The checked-in Visual Studio project configuration specifies:

- Windows development environment.
- Windows 10 SDK.
- Microsoft Visual C++ toolset v143.
- C++20 for the x64 configurations.
- Dynamic library output for the gameplay scripts.

The complete engine build environment and all external SDK requirements are not documented in this repository.

## 7. Project Structure

The repository is divided into three main directories.

```text
BoundByDeath/
│
├── Engine/
│   └── Assets/
│       ├── Prefabs/
│       └── Scripts/
│
├── Game/
│   └── Assets/
│
├── ScriptsProject/
│   ├── EngineDependencies/
│   ├── *.h
│   ├── *.cpp
│   ├── ScriptsProject.vcxproj
│   ├── ScriptsProject.vcxproj.filters
│   └── PlayerCharacterGuide.md
│
├── .gitignore
└── README.md
```

*This is a simplified overview, not an exhaustive listing of every repository file.*

### Engine/

Contains engine-related resources and project assets.

The directory includes asset and prefab resources used by the game and engine.

### Game/

Contains game runtime resources and project-specific assets.

The Visual Studio gameplay-script configuration references the game runtime directory when producing the Release configuration output.

### ScriptsProject/

Contains the C++ gameplay scripting project.

It includes:

- Character and ability implementations.
- Combat and damage systems.
- Enemy controllers and state machines.
- Puzzle and environmental interaction systems.
- UI and gameplay event systems.
- Audio, particle, and haptic integrations.
- Engine API interfaces and supporting dependencies.

The `EngineDependencies/` directory contains engine-facing headers and supporting code used by the gameplay scripts.

### Important Files

| File | Purpose |
|---|---|
| `ScriptsProject.vcxproj` | Visual Studio C++ project configuration |
| `ScriptsProject.vcxproj.filters` | Source organization within Visual Studio |
| `PlayerCharacterGuide.md` | Documentation of the player character architecture |
| `.gitignore` | Git ignore configuration |

## 8. Installation and Setup

### Requirements

The checked-in gameplay scripting project specifies the following development environment:

- Windows.
- Microsoft Visual Studio with C++ development support.
- MSVC v143 platform toolset.
- Windows 10 SDK.
- C++20 support.
- The corresponding custom engine libraries and runtime files.

Additional external dependencies or SDK configuration may be necessary depending on the local engine environment.

### Clone the Repository

Clone the repository using Git:

```bash
git clone https://github.com/Ravenwhisp/BoundByDeath.git
```

Navigate to the project directory:

```bash
cd BoundByDeath
```

### Open the Project

Open the gameplay scripting project in Microsoft Visual Studio:

```text
ScriptsProject/ScriptsProject.vcxproj
```

Select the appropriate x64 configuration.

The project file defines Debug and Release configurations and references the custom engine library.

### Building

The checked-in Visual Studio project is configured to build the gameplay scripts as a dynamic library named:

```text
GameScripts.dll
```

The Debug and Release configurations use different output directories and engine library locations.

**Important:** A complete, reproducible build procedure for the entire game cannot currently be provided from the repository documentation alone.

The project references engine libraries and runtime components whose complete build and installation procedures are not documented here. A successful build of the gameplay scripting project should not be interpreted as proof that the complete game can be independently rebuilt from a fresh clone.

### Running the Game

For available playable versions, consult the repository's GitHub Releases page or itch.io for the official release:

**[Bound By Death – GitHub Releases](https://github.com/Ravenwhisp/BoundByDeath/releases)**
**[Bound By Death – Itch.io Download](https://ravenwhisp-studios.itch.io/bound-by-death)**
Steps:
1. Download .zip file
2. Extract the files
3. Find Engine.exe and execute
4. A pop up will apear, click "ACCEPT"

## 9. Gameplay and Controls

Bound By Death supports a local multiplayer configuration with two players.

The source code identifies two configuration options:

- **Keyboard + Gamepad:** One player uses the keyboard and the other uses a gamepad.
- **Two Gamepads:** Both players use separate gamepads.

The project includes character-specific input handling and player-to-gamepad assignment.

The complete keyboard and controller button mappings are not documented in the repository. Consult the instructions accompanying the playable build for the applicable controls.

## 10. Development Status

Bound By Death is an evolving game development project maintained by Ravenwhisp Studios.

The repository contains a custom engine integration and a substantial collection of gameplay systems, including character controllers, combat, enemy AI, boss encounters, puzzles, progression, and audiovisual systems.

The historical README documents a release named **0.3.1 – Vertical Slice 3**. However, this version has not been independently confirmed as the latest public release.

The current repository also contains source code beyond the scope of that historical description.

For the most recent available builds, consult the [GitHub Releases page](https://github.com/Ravenwhisp/BoundByDeath/releases).

## 11. Team and Credits

**Developed by Ravenwhisp Studios.**

Bound By Death is a collaborative game development project involving the development of a custom game engine and the implementation of the game's gameplay, technical, visual, and audio systems.

For additional information about the studio and its development journey, visit:

[https://ravenwhisp.github.io/Ravenwhisp/](https://ravenwhisp.github.io/Ravenwhisp/)

## 12. Links and Resources

| Resource | Link |
|---|---|
| Official Website | [Ravenwhisp Studios](https://ravenwhisp.github.io/Ravenwhisp/) |
| GitHub Repository | [Bound By Death](https://github.com/Ravenwhisp/BoundByDeath) |
| GitHub Releases | [Available Releases](https://github.com/Ravenwhisp/BoundByDeath/releases) |
| Studio Information | [About Ravenwhisp](https://ravenwhisp.github.io/Ravenwhisp/about-us/) |

## 13. License

No separate license file was identified in the repository at the time of writing.

The project's licensing terms have therefore not been independently established.

Until the authors publish an explicit license, users should not assume that the source code, game assets, or other project materials are available for reuse, modification, redistribution, or commercial exploitation.

For permissions and licensing questions, contact Ravenwhisp Studios through its official website.

---

**Bound By Death**  
Developed by **Ravenwhisp Studios**.

[Official Website](https://ravenwhisp.github.io/Ravenwhisp/) · [GitHub Repository](https://github.com/Ravenwhisp/BoundByDeath)
