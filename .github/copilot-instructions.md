# Project Guidelines

## Architecture
- Treat the repo as three layers: `Engine/` is the core runtime, `Game/` is the packaged runtime/assets output, and `ScriptsProject/` is the gameplay DLL that links against `Engine.lib`.
- Default gameplay code changes to `ScriptsProject/` unless the task explicitly requires engine-side edits.
- Use `ScriptsProject/PlayerCharacterGuide.md` for the player-character architecture instead of duplicating that design detail in chat or new docs.
- Scripts are organized as sibling components on `GameObject`s (e.g., a character `GameObject` has `DeathCharacter`, `DeathBasicAttack`, etc.). Abstract base classes (`AbilityBase`, `CharacterBase`) provide shared behavior without engine registration.

## Build And Test
- Build `ScriptsProject/ScriptsProject.sln` with Visual Studio 2022 or `msbuild ScriptsProject\ScriptsProject.vcxproj`.
- `Debug|x64` outputs `GameScripts.dll` to `Engine/`; `Release|x64` outputs `GameScripts.dll` to `Game/`.
- `ScriptsProject` links against `Engine.lib`, so keep the target platform aligned with the engine binaries already present in `Engine/` and `Game/`.
- No automated test project is present in this workspace. Validate by building the affected configuration and, when relevant, running the game/runtime that loads the DLL.

## Code Style
- Keep `#include "pch.h"` as the first include in every `ScriptsProject/*.cpp` file.
- Follow the existing header/source split and `#pragma once` style used throughout `ScriptsProject/`.
- Match the current comment style: brief, implementation-focused comments only where the control flow or engine contract is not obvious.

## Script Conventions
- Concrete engine scripts use `DECLARE_SCRIPT(...)` in the header and `IMPLEMENT_SCRIPT(...)` in the `.cpp`; abstract base classes such as `AbilityBase` and `CharacterBase` must not register themselves.
- Inspector-exposed fields use `ScriptFieldInfo` plus `IMPLEMENT_SCRIPT_FIELDS(...)`.
- Resolve sibling scripts in `Start()`, not constructors. Use `GameObjectAPI::getScript(...)` and guard null results with warnings or early returns.
- If a class derives from `AbilityBase`, call `AbilityBase::Update()` first in `Update()` so death cancellation and cooldown handling remain correct.
- Preserve the existing ability/character contract: abilities toggle `CharacterBase::setCanAct(...)` through `onActivate()` / `onDeactivate()` rather than changing shared state ad hoc.
- Abilities use a mutex (`CharacterBase::m_canAct`) to prevent simultaneous execution; set via `onActivate()`/`onDeactivate()`.
- Abilities read stats from the character script by casting `m_character` to the concrete type (e.g., `DeathCharacter*`).
- For damage, use `dealDamageToCurrentTarget(float)`; finds `Damageable` on target.
- Input is read via `getPlayerIndex()` for multi-player support.
- Override `onDamaged`/`onDeath`/`onRevive` in subclasses, calling base first.

## Common Pitfalls
- Do not break the x64 DLL workflow by changing output paths or platform settings without updating the matching engine/game binaries.
- Avoid assuming engine-side headers exist in `ScriptsProject`; some engine scripts are intentionally referenced through `Script*` or exported APIs only.
- When adding gameplay scripts, prefer representative patterns already in `ScriptsProject/AbilityBase.*`, `ScriptsProject/CharacterBase.*`, `ScriptsProject/Damageable.*`, and `ScriptsProject/DeathBasicAttack.cpp`.