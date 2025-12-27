#include "dungeon_editor.h"
#include "factory.h"
#include "visitor.h"
#include "observer.h"
#include <iostream>

void DungeonEditor::addNPC(NPCType type, int x, int y, const std::string& name) {
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::runtime_error("Coordinates out of bounds (0-500)");
    }
    npcs.push_back(NPCFactory::create(type, x, y, name));
}

void DungeonEditor::printAll() const {
    std::cout << "NPCs in dungeon:" << std::endl;
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::string typeStr;
            switch (npc->getType()) {
                case NPCType::Bear: typeStr = "Bear"; break;
                case NPCType::Werewolf: typeStr = "Werewolf"; break;  // Изменено
                case NPCType::Rogue: typeStr = "Rogue"; break;
            }
            std::cout << typeStr << " '" << npc->getName() << "' at (" 
                      << npc->getX() << ", " << npc->getY() << ")" << std::endl;
        }
    }
}

void DungeonEditor::save(const std::string& filename) const {
    NPCFactory::saveToFile(filename, npcs);
}

void DungeonEditor::load(const std::string& filename) {
    npcs = NPCFactory::loadFromFile(filename);
}

void DungeonEditor::battle(int range) {
    Observable observable;
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    auto fileObserver = std::make_shared<FileObserver>();
    
    observable.addObserver(consoleObserver);
    observable.addObserver(fileObserver);
    
    NPCVisitor visitor(range, observable);
    visitor.fight(npcs);
}

const std::vector<std::shared_ptr<NPC>>& DungeonEditor::getNPCs() const {
    return npcs;
}