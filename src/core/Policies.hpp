#pragma once
#include "Policy.hpp"
#include <vector>

inline std::vector<Policy> defaultPolicyCatalog() {
    return {
        {
            1, "Rationing Protocol",
            "Strict food allocation. Cuts consumption 25% but hurts morale.",
            {
                {PolicyTarget::FoodConsumption, -0.25f},
                {PolicyTarget::MoralePerCycle,  -2.0f},
            }
        },
        {
            2, "Mandatory Service",
            "All colonists contribute labor. Boosts productivity at morale cost.",
            {
                {PolicyTarget::WorkerProductivity,  0.20f},
                {PolicyTarget::MoralePerCycle,      -3.0f},
            }
        },
        {
            3, "Scientific Priority",
            "Redirect workforce to research. Accelerates discovery, reduces output.",
            {
                {PolicyTarget::ResearchSpeed,       0.40f},
                {PolicyTarget::WorkerProductivity, -0.15f},
            }
        },
        {
            4, "Open Door Immigration",
            "Attract settlers. Grows population faster but strains food supply.",
            {
                {PolicyTarget::PopulationGrowth,  0.5f},
                {PolicyTarget::FoodConsumption,   0.10f},
            }
        },
        {
            5, "Security Crackdown",
            "Tighten security. Improves stability but severely damages morale.",
            {
                {PolicyTarget::SecurityEfficiency, 0.50f},
                {PolicyTarget::StabilityPerCycle,  3.0f},
                {PolicyTarget::MoralePerCycle,     -8.0f},
            }
        },
        {
            6, "Energy Conservation",
            "Mandate reduced energy use. Cuts consumption by 20%.",
            {
                {PolicyTarget::EnergyEfficiency, -0.20f},
                {PolicyTarget::MoralePerCycle,   -1.0f},
            }
        },
        {
            7, "Agricultural Expansion",
            "Redirect land and labor to farming. Boosts food production significantly.",
            {
                {PolicyTarget::FoodProduction,     0.30f},
                {PolicyTarget::WorkerProductivity, -0.10f},
            }
        },
    };
}
