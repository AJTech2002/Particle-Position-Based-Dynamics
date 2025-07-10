#include "tfn/grid.h"
#include "tfn/particle.h"
#include <iostream>

void tfn::ParticleGrid::update() {
    this->clear();

    // std::cout << particles.size() << " particles to update." << std::endl;

    for (auto& particle : particles) {
        int oldX = particle.x;
        int oldY = particle.y;
        particle.simulate();
        setCell(particle.x, particle.y, particle);
    }
}
