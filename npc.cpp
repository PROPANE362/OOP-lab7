#include "npc.h"
#include "visitor.h"  // Добавьте эту строку!
#include "game_constants.h"
#include <random>
#include <algorithm>

NPC::NPC(NPCType type, int x, int y, const std::string& name)
    : type(type), x(x), y(y), name(name), alive(true), randomEngine(nullptr) {
}

NPCType NPC::getType() const {
    return type;
}

int NPC::getX() const {
    return x;
}

int NPC::getY() const {
    return y;
}

void NPC::setPosition(int newX, int newY) {
    x = std::max(0, std::min(MAP_WIDTH - 1, newX));
    y = std::max(0, std::min(MAP_HEIGHT - 1, newY));
}

const std::string& NPC::getName() const {
    return name;
}

bool NPC::isAlive() const {
    return alive;
}

void NPC::markDead() {
    alive = false;
}

void NPC::move(int maxX, int maxY) {
    if (!alive || !randomEngine) return;
    
    std::uniform_int_distribution<int> dirDist(-MOVE_DISTANCE, MOVE_DISTANCE);
    int newX = x + dirDist(*randomEngine);
    int newY = y + dirDist(*randomEngine);
    
    newX = std::max(0, std::min(maxX - 1, newX));
    newY = std::max(0, std::min(maxY - 1, newY));
    
    setPosition(newX, newY);
}

int NPC::rollDice() {
    if (!randomEngine) return 0;
    std::uniform_int_distribution<int> dice(1, 6);
    return dice(*randomEngine);
}

bool NPC::tryKill(NPC& other) {
    if (!randomEngine) return false;
    
    int attack = rollDice();
    int defense = other.rollDice();
    
    return attack > defense;
}

void NPC::setRandomEngine(std::mt19937& engine) {
    randomEngine = &engine;
}

Bear::Bear(int x, int y, const std::string& name)
    : NPC(NPCType::Bear, x, y, name) {
}

void Bear::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

Werewolf::Werewolf(int x, int y, const std::string& name)
    : NPC(NPCType::Werewolf, x, y, name) {
}

void Werewolf::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}

Rogue::Rogue(int x, int y, const std::string& name)
    : NPC(NPCType::Rogue, x, y, name) {
}

void Rogue::accept(NPCVisitor& visitor) {
    visitor.visit(*this);
}