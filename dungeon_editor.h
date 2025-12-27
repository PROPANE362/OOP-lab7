#ifndef DUNGEON_EDITOR_H
#define DUNGEON_EDITOR_H

#include "npc.h"
#include <memory>
#include <vector>
#include <string>

class DungeonEditor {
public:
    void addNPC(NPCType type, int x, int y, const std::string& name);
    void printAll() const;
    void save(const std::string& filename) const;
    void load(const std::string& filename);
    void battle(int range);
    
    const std::vector<std::shared_ptr<NPC>>& getNPCs() const;

private:
    std::vector<std::shared_ptr<NPC>> npcs;
};

#endif