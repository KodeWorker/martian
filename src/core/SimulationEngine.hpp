#pragma once
#include "Colony.hpp"
#include "EventSystem.hpp"
#include "PolicyEngine.hpp"
#include "ResourceAllocator.hpp"

class SimulationEngine {
public:
    SimulationEngine();

    void tick();

    Colony&       colony()   { return m_colony; }
    PolicyEngine& policies() { return m_policyEngine; }
    EventSystem&  events()   { return m_events; }

private:
    Colony            m_colony;
    PolicyEngine      m_policyEngine;
    ResourceAllocator m_allocator;
    EventSystem       m_events;

    void applyProduction(const PolicyContext& ctx);
    void updatePopulation(const PolicyContext& ctx);
    void updateMoraleAndStability(const PolicyContext& ctx);
    void checkThresholds();
    float derivedMoraleDelta() const;
};
