#include "PolicyEngine.hpp"
#include <algorithm>

void PolicyEngine::addPolicy(Policy policy) {
    m_policies.push_back(std::move(policy));
}

bool PolicyEngine::enact(int id) {
    if (auto* p = find(id)) { p->active = true;  return true; }
    return false;
}

bool PolicyEngine::repeal(int id) {
    if (auto* p = find(id)) { p->active = false; return true; }
    return false;
}

PolicyContext PolicyEngine::resolve() const {
    PolicyContext ctx;
    for (const auto& p : m_policies) {
        if (!p.active) continue;
        for (const auto& e : p.effects)
            ctx.deltas[static_cast<int>(e.target)] += e.value;
    }
    return ctx;
}

Policy* PolicyEngine::find(int id) {
    auto it = std::find_if(m_policies.begin(), m_policies.end(),
        [id](const Policy& p) { return p.id == id; });
    return it != m_policies.end() ? &*it : nullptr;
}
