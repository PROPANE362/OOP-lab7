#include "game_engine.h"
#include <iostream>

int main() {
    try {
        GameEngine engine;
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}