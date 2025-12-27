#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "npc.h"
#include "thread_safe_queue.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <shared_mutex>  // Добавьте
#include <map>

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    void run();
    void stop();
    
private:
    void initializeNPCs();
    void movementThread();
    void combatThread();
    void printMapThread();
    
    void processCombat(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender);
    
    std::vector<std::shared_ptr<NPC>> npcs;
    mutable std::shared_mutex npcsMutex;
    
    ThreadSafeQueue combatQueue;
    std::map<std::pair<int, int>, std::shared_ptr<NPC>> positionMap;
    mutable std::mutex positionMapMutex;
    
    std::atomic<bool> running{false};
    std::mutex coutMutex;
    
    std::thread movementWorker;
    std::thread combatWorker;
    std::thread printWorker;
    
    std::mt19937 randomEngine;
    
    void updatePosition(std::shared_ptr<NPC> npc, int oldX, int oldY, int newX, int newY);
    void removeDeadNPC(std::shared_ptr<NPC> npc);
};

#endif