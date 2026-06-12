#include "SimulationEngine.hpp"
#include "Policies.hpp"
#include <algorithm>

SimulationEngine::SimulationEngine() {
    m_colony.stockpile      = {500.f, 500.f, 300.f, 400.f};
    m_colony.productionRate = { 20.f,  25.f,  30.f,  10.f};
    m_colony.consumptionRate= { 15.f,  20.f,  25.f,   5.f};
    m_colony.departments = {
        {"Agriculture", 25, 1.f},
        {"Engineering", 20, 1.f},
        {"Research",    15, 1.f},
        {"Security",    10, 1.f},
        {"Medical",     10, 1.f},
        {"General",     20, 1.f},
    };
    for (auto& p : defaultPolicyCatalog())
        m_policyEngine.addPolicy(std::move(p));
}

void SimulationEngine::tick() {
    m_colony.cycle++;
    PolicyContext ctx = m_policyEngine.resolve();
    applyProduction(ctx);
    m_allocator.allocate(m_colony, ctx);
    updatePopulation(ctx);
    updateMoraleAndStability(ctx);
    checkThresholds();
}

void SimulationEngine::applyProduction(const PolicyContext& ctx) {
    auto& s        = m_colony.stockpile;
    const auto& p  = m_colony.productionRate;
    const auto& c  = m_colony.consumptionRate;
    float popScale = m_colony.population / 100.f;

    float workerMult  = 1.f + ctx.get(PolicyTarget::WorkerProductivity);
    float foodProd    = 1.f + ctx.get(PolicyTarget::FoodProduction);
    float foodCons    = 1.f + ctx.get(PolicyTarget::FoodConsumption);
    float energyEff   = 1.f + ctx.get(PolicyTarget::EnergyEfficiency);
    float researchMul = 1.f + ctx.get(PolicyTarget::ResearchSpeed);

    s.food      += p.food * foodProd * workerMult  - c.food * foodCons * popScale;
    s.water     += p.water * workerMult            - c.water * popScale;
    s.energy    += p.energy * workerMult           - c.energy * energyEff;
    s.materials += p.materials * workerMult        - c.materials;

    m_colony.researchPoints += 5.f * researchMul * workerMult;

    s.food      = std::max(s.food,      0.f);
    s.water     = std::max(s.water,     0.f);
    s.energy    = std::max(s.energy,    0.f);
    s.materials = std::max(s.materials, 0.f);
}

void SimulationEngine::updatePopulation(const PolicyContext& ctx) {
    float baseGrowth = m_colony.population / 50.f;
    int delta = static_cast<int>(baseGrowth + ctx.get(PolicyTarget::PopulationGrowth));
    m_colony.population = std::max(1, m_colony.population + delta);
}

void SimulationEngine::updateMoraleAndStability(const PolicyContext& ctx) {
    float moraleDelta = ctx.get(PolicyTarget::MoralePerCycle) + derivedMoraleDelta();
    m_colony.morale    = std::clamp(m_colony.morale    + moraleDelta,                        0.f, 100.f);
    m_colony.stability = std::clamp(m_colony.stability + ctx.get(PolicyTarget::StabilityPerCycle), 0.f, 100.f);
}

float SimulationEngine::derivedMoraleDelta() const {
    float delta = 0.f;
    if (m_colony.stockpile.food   < 50.f) delta -= 5.f;
    if (m_colony.stockpile.water  < 50.f) delta -= 5.f;
    if (m_colony.stockpile.energy < 30.f) delta -= 3.f;
    return delta;
}

void SimulationEngine::checkThresholds() {
    const auto& c = m_colony;
    if (c.stockpile.food < 50.f)
        m_events.emit({GameEvent::Type::Warning, "Food Shortage",
            "Stockpiles critically low. Morale deteriorating.", c.cycle});
    if (c.stockpile.energy < 30.f)
        m_events.emit({GameEvent::Type::Warning, "Power Grid Strain",
            "Energy reserves dangerously low.", c.cycle});
    if (c.morale < 20.f)
        m_events.emit({GameEvent::Type::Crisis, "Colony Unrest",
            "Morale has collapsed. Risk of civil breakdown imminent.", c.cycle});
    if (c.stability < 20.f)
        m_events.emit({GameEvent::Type::Crisis, "Instability",
            "Colony stability critical. Governance failing.", c.cycle});
    if (c.researchPoints >= 200.f)
        m_events.emit({GameEvent::Type::Opportunity, "Research Milestone",
            "Output sufficient for a breakthrough decision.", c.cycle});
}
