#ifndef FACTORY_H
#define FACTORY_H

#include "npc.h"
#include <memory>
#include <string>
#include <vector>

class NPCFactory {
public:
    static std::shared_ptr<NPC> create(NPCType type, int x, int y, const std::string& name);
    static std::shared_ptr<NPC> loadFromString(const std::string& data);
    static std::vector<std::shared_ptr<NPC>> loadFromFile(const std::string& filename);
    static void saveToFile(const std::string& filename, const std::vector<std::shared_ptr<NPC>>& npcs);
};

#endif