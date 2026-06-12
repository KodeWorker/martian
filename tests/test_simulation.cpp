#include <gtest/gtest.h>
#include "core/SimulationEngine.hpp"

TEST(SimulationTest, TickAdvancesCycle) {
    SimulationEngine sim;
    EXPECT_EQ(sim.colony().cycle, 0);
    sim.tick();
    EXPECT_EQ(sim.colony().cycle, 1);
}

TEST(SimulationTest, PopulationGrowsEachCycle) {
    SimulationEngine sim;
    int initial = sim.colony().population;
    sim.tick();
    EXPECT_GT(sim.colony().population, initial);
}

TEST(SimulationTest, RationingPreservesFoodStockpile) {
    SimulationEngine simBaseline;
    SimulationEngine simRationed;
    simRationed.policies().enact(1); // Rationing Protocol

    simBaseline.tick();
    simRationed.tick();

    EXPECT_GT(simRationed.colony().stockpile.food,
              simBaseline.colony().stockpile.food);
}

TEST(SimulationTest, LowFoodFiresWarningEvent) {
    SimulationEngine sim;
    sim.colony().stockpile.food = 10.f;
    sim.tick();

    bool found = false;
    for (const auto& ev : sim.events().history())
        if (ev.type == GameEvent::Type::Warning && ev.title == "Food Shortage")
            found = true;
    EXPECT_TRUE(found);
}

TEST(SimulationTest, SecurityCrackdownDrainsMorale) {
    SimulationEngine sim;
    sim.policies().enact(5); // Security Crackdown: Morale -8/cycle
    float initial = sim.colony().morale;
    sim.tick();
    EXPECT_LT(sim.colony().morale, initial);
}

TEST(SimulationTest, ResearchAccumulatesEachCycle) {
    SimulationEngine sim;
    sim.tick();
    EXPECT_GT(sim.colony().researchPoints, 0.f);
}
