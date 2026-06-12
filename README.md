# Martian

A policy-driven space colonization simulation game. You govern a fledgling colony by drafting and repealing laws that shape how resources are produced, consumed, and distributed. Every policy has trade-offs — there are no free lunches on Mars.

## Gameplay

The colony runs in discrete **cycles**. Each cycle:

1. Active policies are resolved into a combined effect context
2. Resources are produced and consumed (modified by policies)
3. The resource allocator adjusts department productivity based on supply
4. Population grows, morale and stability shift
5. Threshold events fire (shortages, unrest, milestones)

Press **Space** or click **Advance Cycle** to step forward. Click any policy in the board to enact or repeal it.

## Architecture

The simulation core is fully decoupled from the renderer and can be run headlessly (e.g. in tests).

```
src/
├── core/
│   ├── Colony.hpp/cpp          — Colony state: population, morale, resources, departments
│   ├── Policy.hpp              — PolicyTarget enum, PolicyEffect, Policy
│   ├── Policies.hpp            — Default policy catalog (inline)
│   ├── PolicyEngine.hpp/cpp    — Enact/repeal policies, resolve() → PolicyContext
│   ├── ResourceAllocator.hpp/cpp — Supply ratios → department productivity
│   ├── SimulationEngine.hpp/cpp  — tick() orchestrates the full simulation step
│   └── EventSystem.hpp/cpp     — Emit/subscribe to game events, event history
└── ui/
    ├── Renderer.hpp/cpp        — raylib panels: colony stats, policy board, event log
```

**Data flow per tick:**

```
PolicyEngine::resolve() → PolicyContext
    → SimulationEngine::applyProduction()    (resource deltas)
    → ResourceAllocator::allocate()          (department productivity)
    → updatePopulation() / updateMoraleAndStability()
    → checkThresholds() → EventSystem::emit()
```

## Building

**Dependencies** (Ubuntu/Debian):
```bash
sudo apt-get install libx11-dev libxrandr-dev libxinerama-dev \
                     libxcursor-dev libxi-dev libxext-dev libgl1-mesa-dev
```

raylib and GoogleTest are fetched automatically by CMake.

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
```

## Running

```bash
./build/src/martian          # game
./build/tests/martian_tests  # test suite
```

## Policies

| # | Name | Effects |
|---|------|---------|
| 1 | Rationing Protocol | FoodCons −25%, Morale −2/cycle |
| 2 | Mandatory Service | Worker productivity +20%, Morale −3/cycle |
| 3 | Scientific Priority | Research speed +40%, Worker productivity −15% |
| 4 | Open Door Immigration | Population growth +0.5/cycle, FoodCons +10% |
| 5 | Security Crackdown | Security efficiency +50%, Stability +3/cycle, Morale −8/cycle |
| 6 | Energy Conservation | Energy consumption −20%, Morale −1/cycle |
| 7 | Agricultural Expansion | Food production +30%, Worker productivity −10% |

Multiple policies stack. Conflicting policies (e.g. Rationing + Open Door Immigration) create genuine tension.

## Extending

**Add a policy:** add an entry to `defaultPolicyCatalog()` in `src/core/Policies.hpp`. No other changes needed.

**Add a PolicyTarget:** extend the `PolicyTarget` enum in `src/core/Policy.hpp`, handle it in `SimulationEngine::applyProduction()`, and add a label in `Renderer::drawPolicies()`.

**Add an event trigger:** add a condition in `SimulationEngine::checkThresholds()`.
