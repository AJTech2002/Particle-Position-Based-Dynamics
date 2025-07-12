#include "tfn/grid.h"
#include "tfn/particle.h"
#include <iostream>
#include <random>

static std::mt19937 rng(std::random_device{}());

void tfn::ParticleGrid::update() {
    this->clear();

    // std::cout << particles.size() << " particles to update." << std::endl;
    std::shuffle(particles.begin(), particles.end(), rng);

    for (auto& particle : particles) {
        int oldX = particle->x;
        int oldY = particle->y;
        particle->simulate();
        setCell(particle->x, particle->y, particle);
    }
}
