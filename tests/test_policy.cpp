#include <gtest/gtest.h>
#include "core/PolicyEngine.hpp"
#include "core/Policies.hpp"

class PolicyEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        for (auto& p : defaultPolicyCatalog())
            engine.addPolicy(std::move(p));
    }
    PolicyEngine engine;
};

TEST_F(PolicyEngineTest, NoPoliciesActiveByDefault) {
    auto ctx = engine.resolve();
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::FoodConsumption), 0.f);
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::MoralePerCycle),  0.f);
}

TEST_F(PolicyEngineTest, RationingReducesFoodConsumption) {
    engine.enact(1);
    auto ctx = engine.resolve();
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::FoodConsumption), -0.25f);
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::MoralePerCycle),  -2.0f);
}

TEST_F(PolicyEngineTest, RepealClearsEffects) {
    engine.enact(1);
    engine.repeal(1);
    auto ctx = engine.resolve();
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::FoodConsumption), 0.f);
}

TEST_F(PolicyEngineTest, MultipleActivePoliciesStack) {
    engine.enact(1); // Rationing:         Morale -2
    engine.enact(2); // Mandatory Service: Morale -3
    auto ctx = engine.resolve();
    EXPECT_FLOAT_EQ(ctx.get(PolicyTarget::MoralePerCycle), -5.0f);
}

TEST_F(PolicyEngineTest, EnactInvalidIdReturnsFalse) {
    EXPECT_FALSE(engine.enact(999));
}
