#include "visitor.h"
#include "game_constants.h"
#include <cmath>
#include <iostream>

NPCVisitor::NPCVisitor(int range, Observable& observable)
    : range(range), observable(observable) {
}

bool NPCVisitor::inRange(const NPC& a, const NPC& b) const {
    int dx = a.getX() - b.getX();
    int dy = a.getY() - b.getY();
    double distance = std::sqrt(dx*dx + dy*dy);
    return distance <= range;
}

bool NPCVisitor::canKill(NPCType killer, NPCType victim) const {
    // Оборотни убивают Разбойников
    if (killer == NPCType::Werewolf && victim == NPCType::Rogue) return true;
    // Разбойники убивают медведей
    if (killer == NPCType::Rogue && victim == NPCType::Bear) return true;
    // Медведи убивают оборотней
    if (killer == NPCType::Bear && victim == NPCType::Werewolf) return true;
    return false;
}


void NPCVisitor::visit(Bear& bear) {
    // Реализация если нужна
    (void)bear;  // Чтобы убрать warning
}

void NPCVisitor::visit(Werewolf& werewolf) {  // Изменено
    // Реализация если нужна
    (void)werewolf;  // Чтобы убрать warning
}

void NPCVisitor::visit(Rogue& rogue) {
    // Реализация если нужна
    (void)rogue;  // Чтобы убрать warning
}

void NPCVisitor::fight(std::vector<std::shared_ptr<NPC>>& npcs) {
    for (size_t i = 0; i < npcs.size(); ++i) {
        if (!npcs[i]->isAlive()) continue;
        
        for (size_t j = i + 1; j < npcs.size(); ++j) {
            if (!npcs[j]->isAlive()) continue;
            
            if (inRange(*npcs[i], *npcs[j])) {
                bool iKillsJ = canKill(npcs[i]->getType(), npcs[j]->getType());
                bool jKillsI = canKill(npcs[j]->getType(), npcs[i]->getType());
                
                if (iKillsJ && jKillsI) {
                    observable.notifyKill(npcs[i]->getName(), npcs[j]->getName());
                    observable.notifyKill(npcs[j]->getName(), npcs[i]->getName());
                    npcs[i]->markDead();
                    npcs[j]->markDead();
                } else if (iKillsJ) {
                    observable.notifyKill(npcs[i]->getName(), npcs[j]->getName());
                    npcs[j]->markDead();
                } else if (jKillsI) {
                    observable.notifyKill(npcs[j]->getName(), npcs[i]->getName());
                    npcs[i]->markDead();
                }
            }
        }
    }
}