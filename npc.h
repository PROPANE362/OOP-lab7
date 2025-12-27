#ifndef NPC_H
#define NPC_H

#include <string>
#include <memory>
#include <vector>
#include <random>
#include <mutex>  // Добавьте

enum class NPCType {
    Bear,
    Werewolf,  // Изменено с Elf на Werewolf
    Rogue
};

class NPCVisitor;

class NPC {
public:
    NPC(NPCType type, int x, int y, const std::string& name);
    virtual ~NPC() = default;
    
    virtual void accept(NPCVisitor& visitor) = 0;
    
    NPCType getType() const;
    int getX() const;
    int getY() const;
    void setPosition(int newX, int newY);
    const std::string& getName() const;
    bool isAlive() const;
    void markDead();
    
    void move(int maxX, int maxY);
    int rollDice();
    bool tryKill(NPC& other);
    void setRandomEngine(std::mt19937& engine);

private:
    NPCType type;
    int x;
    int y;
    std::string name;
    bool alive;
    std::mt19937* randomEngine;
};

class Bear : public NPC {
public:
    Bear(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
};

class Werewolf : public NPC {  // Изменено с Elf на Werewolf
public:
    Werewolf(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
};

class Rogue : public NPC {
public:
    Rogue(int x, int y, const std::string& name);
    void accept(NPCVisitor& visitor) override;
};

#endif