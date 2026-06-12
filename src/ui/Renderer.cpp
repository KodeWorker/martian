#include "Renderer.hpp"
#include <algorithm>
#include <cstdio>
#include <string>

static constexpr int SCREEN_W = 1200;
static constexpr int SCREEN_H = 800;

static const Color BG           = {10,  10,  30,  255};
static const Color PANEL_BG     = {20,  20,  50,  255};
static const Color PANEL_BORDER = {60,  60, 120,  255};
static const Color TEXT_MAIN    = {220, 220, 230, 255};
static const Color TEXT_DIM     = {120, 120, 140, 255};
static const Color COL_GREEN    = {50,  200, 100, 255};
static const Color COL_YELLOW   = {255, 200,   0, 255};
static const Color COL_RED      = {220,  50,  50, 255};
static const Color COL_BLUE     = {80,  140, 220, 255};

Renderer::Renderer(SimulationEngine& sim) : m_sim(sim) {
    InitWindow(SCREEN_W, SCREEN_H, "Martian — Colony Simulation");
    SetTargetFPS(60);
}

Renderer::~Renderer() {
    CloseWindow();
}

bool Renderer::shouldClose() const { return WindowShouldClose(); }

bool Renderer::pollNextCycle() {
    if (m_nextCycleReq) { m_nextCycleReq = false; return true; }
    return false;
}

void Renderer::draw() {
    BeginDrawing();
    ClearBackground(BG);

    constexpr int statsW = 300;
    constexpr int btnH   = 60;
    constexpr int logH   = 160;
    constexpr int mainH  = SCREEN_H - logH - btnH - 10;
    constexpr int polX   = statsW + 8;
    constexpr int polW   = SCREEN_W - polX;

    drawColonyStats    (0,      8,          statsW, mainH);
    drawPolicies       (polX,   8,          polW,   mainH);
    drawEventLog       (0,      mainH + 10, SCREEN_W, logH);
    drawNextCycleButton(0,      SCREEN_H - btnH, SCREEN_W, btnH);

    EndDrawing();
}

// ── helpers ──────────────────────────────────────────────────────────────────

static void sectionHeader(const char* label, int tx, int& ty, int w, Color col) {
    DrawText(label, tx, ty, 14, col);
    ty += 18;
    DrawLine(tx, ty, tx + w - 24, ty, PANEL_BORDER);
    ty += 10;
}

static Color thresholdColor(float v, float warn, float crit) {
    if (v <= crit)  return {220, 50,  50,  255};
    if (v <= warn)  return {255, 200,  0,  255};
    return TEXT_MAIN;
}

// ── panels ───────────────────────────────────────────────────────────────────

void Renderer::drawColonyStats(int x, int y, int w, int h) {
    DrawRectangle(x, y, w, h, PANEL_BG);
    DrawRectangleLines(x, y, w, h, PANEL_BORDER);

    const Colony& c = m_sim.colony();
    int tx = x + 12, ty = y + 12;
    constexpr int ls = 22;
    constexpr int lx = 140;

    auto row = [&](const char* label, const std::string& val, Color col = TEXT_MAIN) {
        DrawText(label, tx, ty, 13, TEXT_DIM);
        DrawText(val.c_str(), tx + lx, ty, 13, col);
        ty += ls;
    };

    sectionHeader("COLONY STATUS", tx, ty, w, COL_BLUE);
    row("Colony",     c.name);
    row("Cycle",      std::to_string(c.cycle));
    row("Population", std::to_string(c.population));
    row("Morale",     std::to_string((int)c.morale) + "%",    thresholdColor(c.morale,    50, 25));
    row("Stability",  std::to_string((int)c.stability) + "%", thresholdColor(c.stability, 50, 25));
    row("Research",   std::to_string((int)c.researchPoints) + " pts");

    ty += 6;
    sectionHeader("RESOURCES", tx, ty, w, COL_BLUE);
    row("Food",      std::to_string((int)c.stockpile.food),      thresholdColor(c.stockpile.food,      100, 50));
    row("Water",     std::to_string((int)c.stockpile.water),     thresholdColor(c.stockpile.water,     100, 50));
    row("Energy",    std::to_string((int)c.stockpile.energy),    thresholdColor(c.stockpile.energy,     60, 30));
    row("Materials", std::to_string((int)c.stockpile.materials), thresholdColor(c.stockpile.materials,  40, 20));

    ty += 6;
    sectionHeader("DEPARTMENTS", tx, ty, w, COL_BLUE);
    for (const auto& dept : c.departments) {
        if (ty + ls > y + h - 10) break;
        Color pc = dept.productivity > 0.8f ? TEXT_MAIN
                 : dept.productivity > 0.4f ? COL_YELLOW : COL_RED;
        DrawText(dept.name.c_str(), tx, ty, 12, TEXT_DIM);
        std::string ps = std::to_string((int)(dept.productivity * 100)) + "%";
        DrawText(ps.c_str(), tx + lx, ty, 12, pc);
        ty += ls - 4;
    }
}

void Renderer::drawPolicies(int x, int y, int w, int h) {
    DrawRectangle(x, y, w, h, PANEL_BG);
    DrawRectangleLines(x, y, w, h, PANEL_BORDER);

    int tx = x + 12, ty = y + 12;
    sectionHeader("POLICY BOARD", tx, ty, w, COL_BLUE);
    DrawText("Click a policy to enact or repeal it.", tx, ty, 11, TEXT_DIM);
    ty += 22;

    constexpr int rowH = 70;
    const auto& policies = m_sim.policies().policies();

    for (const auto& policy : policies) {
        if (ty + rowH > y + h - 10) break;

        Rectangle row = {(float)(x + 6), (float)ty, (float)(w - 18), (float)(rowH - 4)};
        Color rowBg  = policy.active ? Color{25, 60, 35, 255} : Color{28, 28, 58, 255};
        DrawRectangleRec(row, rowBg);
        DrawRectangleLinesEx(row, 1.f, policy.active ? COL_GREEN : PANEL_BORDER);

        const char* badge = policy.active ? "ACTIVE" : "INACTIVE";
        DrawText(badge, (int)row.x + 8, ty + 8, 11, policy.active ? COL_GREEN : TEXT_DIM);
        DrawText(policy.name.c_str(), (int)row.x + 80, ty + 6, 13, TEXT_MAIN);
        DrawText(policy.description.c_str(), (int)row.x + 8, ty + 26, 11, TEXT_DIM);

        // effects summary
        std::string fx;
        for (const auto& e : policy.effects) {
            if (!fx.empty()) fx += "  ";
            const char* tname = "";
            switch (e.target) {
                case PolicyTarget::FoodProduction:    tname = "FoodProd";   break;
                case PolicyTarget::FoodConsumption:   tname = "FoodCons";   break;
                case PolicyTarget::EnergyEfficiency:  tname = "Energy";     break;
                case PolicyTarget::MoralePerCycle:    tname = "Morale";     break;
                case PolicyTarget::StabilityPerCycle: tname = "Stability";  break;
                case PolicyTarget::WorkerProductivity:tname = "Worker";     break;
                case PolicyTarget::ResearchSpeed:     tname = "Research";   break;
                case PolicyTarget::PopulationGrowth:  tname = "PopGrowth";  break;
                case PolicyTarget::SecurityEfficiency:tname = "Security";   break;
            }
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%s%+.2f", tname, e.value);
            fx += buf;
        }
        DrawText(fx.c_str(), (int)row.x + 8, ty + 48, 10, TEXT_DIM);

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mp = GetMousePosition();
            if (CheckCollisionPointRec(mp, row)) {
                if (policy.active) m_sim.policies().repeal(policy.id);
                else               m_sim.policies().enact(policy.id);
            }
        }

        ty += rowH;
    }
}

void Renderer::drawEventLog(int x, int y, int w, int h) {
    DrawRectangle(x, y, w, h, PANEL_BG);
    DrawRectangleLines(x, y, w, h, PANEL_BORDER);

    int tx = x + 12, ty = y + 10;
    sectionHeader("EVENT LOG", tx, ty, w, COL_BLUE);

    const auto& hist = m_sim.events().history();
    int start = std::max(0, (int)hist.size() - 5);
    for (int i = (int)hist.size() - 1; i >= start; --i) {
        if (ty + 20 > y + h - 6) break;
        const auto& ev = hist[i];
        std::string line = "[" + std::to_string(ev.cycle) + "] "
                         + ev.title + ": " + ev.body;
        DrawText(line.c_str(), tx, ty, 12, eventColor(ev.type));
        ty += 20;
    }
}

void Renderer::drawNextCycleButton(int x, int y, int w, int h) {
    Rectangle btn = {(float)x, (float)y, (float)w, (float)h};
    bool hover = CheckCollisionPointRec(GetMousePosition(), btn);
    Color bg   = hover ? Color{50, 110, 70, 255} : Color{30, 80, 50, 255};

    DrawRectangleRec(btn, bg);
    DrawRectangleLinesEx(btn, 1.f, COL_GREEN);

    const char* label = "ADVANCE CYCLE   [SPACE]";
    int tw = MeasureText(label, 18);
    DrawText(label, x + (w - tw) / 2, y + (h - 18) / 2, 18, COL_GREEN);

    if ((hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_SPACE))
        m_nextCycleReq = true;
}

Color Renderer::eventColor(GameEvent::Type t) {
    switch (t) {
        case GameEvent::Type::Warning:      return COL_YELLOW;
        case GameEvent::Type::Crisis:       return COL_RED;
        case GameEvent::Type::Opportunity:  return COL_GREEN;
        case GameEvent::Type::Notification: return TEXT_MAIN;
    }
    return TEXT_MAIN;
}
