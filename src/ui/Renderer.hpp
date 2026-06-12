#pragma once
#include "../core/SimulationEngine.hpp"
#include "raylib.h"

class Renderer {
public:
    explicit Renderer(SimulationEngine& sim);
    ~Renderer();

    bool shouldClose() const;
    void draw();
    bool pollNextCycle();

private:
    SimulationEngine& m_sim;
    bool              m_nextCycleReq = false;

    void drawColonyStats(int x, int y, int w, int h);
    void drawPolicies(int x, int y, int w, int h);
    void drawEventLog(int x, int y, int w, int h);
    void drawNextCycleButton(int x, int y, int w, int h);

    static Color eventColor(GameEvent::Type t);
};
