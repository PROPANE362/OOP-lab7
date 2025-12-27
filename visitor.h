#ifndef VISITOR_H
#define VISITOR_H

#include "observer.h"
#include "npc.h"
#include <memory>
#include <vector>

// Предварительные объявления
class Bear;
class Werewolf;  // Изменено с Elf на Werewolf
class Rogue;

class NPCVisitor {
public:
    NPCVisitor(int range, Observable& observable);

    void visit(Bear& bear);
    void visit(Werewolf& werewolf);  // Изменено
    void visit(Rogue& rogue);
    
    void fight(std::vector<std::shared_ptr<NPC>>& npcs);
    
    bool canKill(NPCType killer, NPCType victim) const;  // Сделайте public

private:
    int range;
    Observable& observable;
    
    bool inRange(const NPC& a, const NPC& b) const;
};

#endif