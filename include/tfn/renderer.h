#pragma once
#define ROOT "C:/Users/avenkat/Desktop/Development/samurai-gl/src/"

namespace tfn {

    struct DisplayCell;
    
    class ComputeRenderer {
        public:
            void init(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, unsigned int GRID_WIDTH, unsigned int GRID_HEIGHT);
            void render();
            void update(tfn::DisplayCell* cells);

        private:
            unsigned int vertexShader;
            unsigned int fragmentShader;

            void quadSetup();
    };
}
