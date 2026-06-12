#pragma once
#include <string>
#include <vector>

enum class PolicyTarget {
    FoodProduction,
    FoodConsumption,
    EnergyEfficiency,
    MoralePerCycle,
    StabilityPerCycle,
    WorkerProductivity,
    ResearchSpeed,
    PopulationGrowth,
    SecurityEfficiency,
};

struct PolicyEffect {
    PolicyTarget target;
    float        value;
};

struct Policy {
    int                       id;
    std::string               name;
    std::string               description;
    std::vector<PolicyEffect> effects;
    bool                      active = false;
};
