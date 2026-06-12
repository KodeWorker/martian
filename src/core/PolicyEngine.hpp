#pragma once
#include "Policy.hpp"
#include <unordered_map>
#include <vector>

struct PolicyContext {
    std::unordered_map<int, float> deltas; // keyed by PolicyTarget cast to int

    float get(PolicyTarget t) const {
        auto it = deltas.find(static_cast<int>(t));
        return it != deltas.end() ? it->second : 0.f;
    }
};

class PolicyEngine {
public:
    void addPolicy(Policy policy);
    bool enact(int policyId);
    bool repeal(int policyId);
    PolicyContext resolve() const;
    const std::vector<Policy>& policies() const { return m_policies; }

private:
    std::vector<Policy> m_policies;
    Policy* find(int id);
};
