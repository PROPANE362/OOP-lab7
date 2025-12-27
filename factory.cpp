#include "factory.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <memory>  // Добавьте эту строку!

std::shared_ptr<NPC> NPCFactory::create(NPCType type, int x, int y, const std::string& name) {
    switch (type) {
        case NPCType::Bear:
            return std::make_shared<Bear>(x, y, name);
        case NPCType::Werewolf:  // Изменено с Elf на Werewolf
            return std::make_shared<Werewolf>(x, y, name);
        case NPCType::Rogue:
            return std::make_shared<Rogue>(x, y, name);
        default:
            throw std::runtime_error("Unknown NPC type");
    }
}

std::shared_ptr<NPC> NPCFactory::loadFromString(const std::string& data) {
    std::istringstream iss(data);
    std::string typeStr;
    std::string name;
    int x, y;
    
    if (!(iss >> typeStr >> x >> y)) {
        throw std::runtime_error("Invalid NPC data format");
    }
    
    std::getline(iss >> std::ws, name);
    
    NPCType type;
    if (typeStr == "Bear") type = NPCType::Bear;
    else if (typeStr == "Werewolf") type = NPCType::Werewolf;  // Изменено
    else if (typeStr == "Rogue") type = NPCType::Rogue;
    else throw std::runtime_error("Unknown NPC type: " + typeStr);
    
    return create(type, x, y, name);
}

std::vector<std::shared_ptr<NPC>> NPCFactory::loadFromFile(const std::string& filename) {
    std::vector<std::shared_ptr<NPC>> npcs;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            npcs.push_back(loadFromString(line));
        }
    }
    
    return npcs;
}

void NPCFactory::saveToFile(const std::string& filename, const std::vector<std::shared_ptr<NPC>>& npcs) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    for (const auto& npc : npcs) {
        if (npc->isAlive()) {
            std::string typeStr;
            switch (npc->getType()) {
                case NPCType::Bear: typeStr = "Bear"; break;
                case NPCType::Werewolf: typeStr = "Werewolf"; break;  // Изменено
                case NPCType::Rogue: typeStr = "Rogue"; break;
            }
            file << typeStr << " " << npc->getX() << " " << npc->getY() << " " << npc->getName() << std::endl;
        }
    }
}