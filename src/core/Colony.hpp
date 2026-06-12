#pragma once
#include <string>
#include <vector>

struct Resources {
    float food      = 0.f;
    float water     = 0.f;
    float energy    = 0.f;
    float materials = 0.f;
};

struct Department {
    std::string name;
    int         workers      = 0;
    float       productivity = 1.0f;
};

struct Colony {
    std::string name       = "New Eden";
    int         population = 100;
    float       morale     = 70.f;
    float       stability  = 80.f;
    int         cycle      = 0;

    Resources stockpile;
    Resources productionRate;
    Resources consumptionRate;

    float researchPoints = 0.f;

    std::vector<Department> departments;
};
