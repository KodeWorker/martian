#pragma once
#include "Colony.hpp"
#include "PolicyEngine.hpp"

class ResourceAllocator {
public:
    void allocate(Colony& colony, const PolicyContext& ctx);
};
