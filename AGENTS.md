# AGENTS

Agent instructions for BoundByDeath.

## Scope
- This repository has three major folders:
- `Engine/`: engine runtime binaries, shaders, and assets.
- `Game/`: release-oriented runtime mirror.
- `ScriptsProject/`: gameplay C++ scripts built as `GameScripts.dll`.

## Start Here
- Read `ScriptsProject/PlayerCharacterGuide.md` first for character/ability architecture and lifecycle decisions.
- Read `ScriptsProject/ScriptsProject.vcxproj` for build output paths, includes, and linker settings.

## Build And Run
- Main build target is the Visual Studio solution in `ScriptsProject/` (toolset `v143`, C++17, Windows).
- Typical commands from repository root:
- `msbuild ScriptsProject\ScriptsProject.sln /p:Configuration=Debug /p:Platform=x64`
- `msbuild ScriptsProject\ScriptsProject.sln /p:Configuration=Release /p:Platform=x64`
- Output routing in x64:
- `Debug` writes `GameScripts.dll` to `Engine/`.
- `Release` writes `GameScripts.dll` to `Game/`.

## Script Registration Rules
- Concrete script classes must include both macros:
- `DECLARE_SCRIPT(ClassName)` in the header.
- `IMPLEMENT_SCRIPT(ClassName)` at end of the source file.
- Abstract base classes must not be registered with these macros.

## Common Code Patterns
- Include order in `.cpp`: `#include "pch.h"` first, then local headers.
- Cache sibling script references in `Start()` using `GameObjectAPI::getScript(...)` and `dynamic_cast`.
- Always null-check cached pointers in update/state methods before use.
- Keep inspector fields public and exposed through `IMPLEMENT_SCRIPT_FIELDS(...)` with `SERIALIZED_*` entries.
- In derived overrides, call base class lifecycle when relevant (for example `CharacterBase::Start()`, `AbilityBase::Update()`).

## State And Ability Conventions
- Enemy state classes (`EnemyIDLE`, `EnemyCHASE`, etc.) inherit `StateMachineScript` and use `OnStateEnter/OnStateUpdate/OnStateExit`.
- Ability classes inherit from shared bases (`AbilityBase`, `DeathAbilityBase`, `LyrielAbilityBase`) and rely on cooldown/state gating in base logic.
- Character classes (`DeathCharacter`, `LyrielCharacter`) own tunable stat fields consumed by ability scripts.

## Pitfalls To Avoid
- Do not assume `Engine/` and `Game/` are interchangeable during iteration; debug changes are expected under `Engine/` outputs.
- String-based script lookup is fragile. Keep script class names exact when calling `GameObjectAPI::getScript(...)`.
- If a script is not appearing at runtime, check registration macros first.

## Change Guidance
- Keep edits scoped to `ScriptsProject/` unless engine/runtime changes are explicitly requested.
- Preserve existing naming style (`PascalCase` types, `m_` member fields, macro-based serialization metadata).
- Prefer small, localized changes; avoid broad refactors without a clear request.
