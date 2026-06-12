#include "core/SimulationEngine.hpp"
#include "ui/Renderer.hpp"

int main() {
    SimulationEngine sim;
    Renderer renderer(sim);

    while (!renderer.shouldClose()) {
        if (renderer.pollNextCycle())
            sim.tick();
        renderer.draw();
    }

    return 0;
}
