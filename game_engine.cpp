#include "game_engine.h"
#include "game_constants.h"
#include "factory.h"
#include "visitor.h"
#include "observer.h"
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

GameEngine::GameEngine() : randomEngine(std::random_device{}()) {
    initializeNPCs();
}

GameEngine::~GameEngine() {
    stop();
}

void GameEngine::initializeNPCs() {
    std::uniform_int_distribution<int> posDist(0, MAP_WIDTH - 1);
    std::uniform_int_distribution<int> typeDist(0, 2);
    
    for (int i = 0; i < INITIAL_NPC_COUNT; ++i) {
        int x = posDist(randomEngine);
        int y = posDist(randomEngine);
        
        NPCType type;
        switch (typeDist(randomEngine)) {
            case 0: type = NPCType::Bear; break;
            case 1: type = NPCType::Werewolf; break;
            case 2: type = NPCType::Rogue; break;
        }
        
        std::string name = "NPC_" + std::to_string(i);
        auto npc = NPCFactory::create(type, x, y, name);
        npc->setRandomEngine(randomEngine);
        
        {
            std::lock_guard<std::mutex> lock(positionMapMutex);
            positionMap[{x, y}] = npc;
        }
        
        npcs.push_back(npc);
    }
}

void GameEngine::run() {
    running = true;
    
    movementWorker = std::thread(&GameEngine::movementThread, this);
    combatWorker = std::thread(&GameEngine::combatThread, this);
    printWorker = std::thread(&GameEngine::printMapThread, this);
    
    // Ждем 30 секунд
    std::this_thread::sleep_for(std::chrono::seconds(GAME_DURATION_SECONDS));
    
    stop();
    
    // Выводим список выживших
    {
        std::shared_lock<std::shared_mutex> lock(npcsMutex);
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << "\n=== SURVIVORS AFTER " << GAME_DURATION_SECONDS << " SECONDS ===" << std::endl;
        
        int survivorCount = 0;
        for (const auto& npc : npcs) {
            if (npc->isAlive()) {
                std::string typeStr;
                switch (npc->getType()) {
                    case NPCType::Bear: typeStr = "Bear"; break;
                    case NPCType::Werewolf: typeStr = "Werewolf"; break;
                    case NPCType::Rogue: typeStr = "Rogue"; break;
                }
                std::cout << typeStr << " '" << npc->getName() << "' at (" 
                          << npc->getX() << ", " << npc->getY() << ")" << std::endl;
                survivorCount++;
            }
        }
        std::cout << "Total survivors: " << survivorCount << std::endl;
    }
}

void GameEngine::stop() {
    running = false;
    
    if (movementWorker.joinable()) movementWorker.join();
    if (combatWorker.joinable()) combatWorker.join();
    if (printWorker.joinable()) printWorker.join();
}

void GameEngine::movementThread() {
    Observable observable;
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    observable.addObserver(consoleObserver);
    
    NPCVisitor visitor(KILL_DISTANCE, observable);
    
    while (running) {
        {
            std::shared_lock<std::shared_mutex> lock(npcsMutex);
            
            for (size_t i = 0; i < npcs.size() && running; ++i) {
                auto& npc = npcs[i];
                if (!npc->isAlive()) continue;
                
                int oldX = npc->getX();
                int oldY = npc->getY();
                
                // Двигаем NPC
                npc->move(MAP_WIDTH, MAP_HEIGHT);
                
                int newX = npc->getX();
                int newY = npc->getY();
                
                // Обновляем позицию на карте
                updatePosition(npc, oldX, oldY, newX, newY);
                
                // Проверяем соседей на возможность боя
                std::vector<std::shared_ptr<NPC>> neighbors;
                {
                    std::lock_guard<std::mutex> mapLock(positionMapMutex);
                    for (int dx = -KILL_DISTANCE; dx <= KILL_DISTANCE; ++dx) {
                        for (int dy = -KILL_DISTANCE; dy <= KILL_DISTANCE; ++dy) {
                            if (dx == 0 && dy == 0) continue;
                            
                            int checkX = newX + dx;
                            int checkY = newY + dy;
                            
                            if (checkX >= 0 && checkX < MAP_WIDTH && 
                                checkY >= 0 && checkY < MAP_HEIGHT) {
                                auto it = positionMap.find({checkX, checkY});
                                if (it != positionMap.end() && it->second->isAlive()) {
                                    neighbors.push_back(it->second);
                                }
                            }
                        }
                    }
                }
                
                // Создаем задачи для боя
                for (auto& neighbor : neighbors) {
                    if (visitor.canKill(npc->getType(), neighbor->getType()) ||
                        visitor.canKill(neighbor->getType(), npc->getType())) {
                        
                        combatQueue.push([this, attacker = npc, defender = neighbor]() {
                            processCombat(attacker, defender);
                        });
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void GameEngine::combatThread() {
    while (running) {
        ThreadSafeQueue::Task task;
        if (combatQueue.tryPop(task)) {
            task();
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void GameEngine::processCombat(std::shared_ptr<NPC> attacker, std::shared_ptr<NPC> defender) {
    if (!attacker->isAlive() || !defender->isAlive()) return;
    
    Observable observable;
    auto consoleObserver = std::make_shared<ConsoleObserver>();
    observable.addObserver(consoleObserver);
    
    NPCVisitor visitor(KILL_DISTANCE, observable);
    
    bool attackerKillsDefender = visitor.canKill(attacker->getType(), defender->getType()) &&
                                 attacker->tryKill(*defender);
    
    bool defenderKillsAttacker = visitor.canKill(defender->getType(), attacker->getType()) &&
                                 defender->tryKill(*attacker);
    
    {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        if (attackerKillsDefender && defenderKillsAttacker) {
            std::cout << "MUTUAL KILL: " << attacker->getName() << " and " 
                      << defender->getName() << " killed each other!" << std::endl;
            defender->markDead();
            attacker->markDead();
            removeDeadNPC(defender);
            removeDeadNPC(attacker);
        } else if (attackerKillsDefender) {
            std::cout << attacker->getName() << " killed " << defender->getName() << std::endl;
            defender->markDead();
            removeDeadNPC(defender);
        } else if (defenderKillsAttacker) {
            std::cout << defender->getName() << " killed " << attacker->getName() << std::endl;
            attacker->markDead();
            removeDeadNPC(attacker);
        }
    }
}

void GameEngine::printMapThread() {
    while (running) {
        {
            std::lock_guard<std::mutex> coutLock(coutMutex);
            std::cout << "\n=== CURRENT MAP ===" << std::endl;
            
            // Создаем карту
            std::vector<std::vector<char>> map(MAP_HEIGHT, std::vector<char>(MAP_WIDTH, '.'));
            
            {
                std::shared_lock<std::shared_mutex> lock(npcsMutex);
                for (const auto& npc : npcs) {
                    if (npc->isAlive()) {
                        int x = npc->getX();
                        int y = npc->getY();
                        
                        char symbol;
                        switch (npc->getType()) {
                            case NPCType::Bear: symbol = 'B'; break;
                            case NPCType::Werewolf: symbol = 'W'; break;
                            case NPCType::Rogue: symbol = 'R'; break;
                        }
                        
                        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                            map[y][x] = symbol;
                        }
                    }
                }
            }
            
            // Печатаем карту
            for (int y = 0; y < MAP_HEIGHT; ++y) {
                for (int x = 0; x < MAP_WIDTH; ++x) {
                    std::cout << map[y][x];
                }
                std::cout << std::endl;
            }
            std::cout << "Legend: B=Bear, W=Werewolf, R=Rogue, .=empty" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void GameEngine::updatePosition(std::shared_ptr<NPC> npc, int oldX, int oldY, int newX, int newY) {
    std::lock_guard<std::mutex> lock(positionMapMutex);
    
    // Удаляем старую позицию
    auto oldIt = positionMap.find({oldX, oldY});
    if (oldIt != positionMap.end() && oldIt->second == npc) {
        positionMap.erase(oldIt);
    }
    
    // Добавляем новую позицию
    positionMap[{newX, newY}] = npc;
}

void GameEngine::removeDeadNPC(std::shared_ptr<NPC> npc) {
    std::lock_guard<std::mutex> lock(positionMapMutex);
    
    // Удаляем из карты позиций
    for (auto it = positionMap.begin(); it != positionMap.end();) {
        if (it->second == npc) {
            it = positionMap.erase(it);
        } else {
            ++it;
        }
    }
}