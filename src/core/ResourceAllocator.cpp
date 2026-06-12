#include "ResourceAllocator.hpp"
#include <algorithm>

void ResourceAllocator::allocate(Colony& colony, const PolicyContext& ctx) {
    float energyRatio = (colony.consumptionRate.energy > 0.f)
        ? std::min(colony.stockpile.energy / colony.consumptionRate.energy, 1.f)
        : 1.f;

    float popScale  = colony.population / 100.f;
    float foodDemand = colony.consumptionRate.food * popScale;
    float foodRatio  = (foodDemand > 0.f)
        ? std::min(colony.stockpile.food / foodDemand, 1.f)
        : 1.f;

    float securityMult = 1.f + ctx.get(PolicyTarget::SecurityEfficiency);
    float supplyRatio  = std::min(energyRatio, foodRatio);

    for (auto& dept : colony.departments) {
        float base = (dept.name == "Security") ? securityMult : 1.f;
        dept.productivity = base * supplyRatio;
    }
}
