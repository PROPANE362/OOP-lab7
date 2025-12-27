#include "gtest/gtest.h"
#include "dungeon_editor.h"
#include "factory.h"
#include "game_engine.h"
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include "visitor.h"        
#include "observer.h"       
#include "game_constants.h" 
#include <random>           

class DungeonEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream testFile("test_dungeon.txt");
        testFile << "Bear 10 10 Bear1\n";
        testFile << "Werewolf 20 20 Wolf1\n";
        testFile << "Rogue 30 30 Rogue1\n";
        testFile.close();
    }
    
    void TearDown() override {
        std::remove("test_dungeon.txt");
        std::remove("test_save.txt");
        std::remove("log.txt");
    }
};

TEST_F(DungeonEditorTest, AddNPC) {
    DungeonEditor editor;
    editor.addNPC(NPCType::Bear, 100, 100, "TestBear");
    editor.addNPC(NPCType::Werewolf, 200, 200, "TestWerewolf");
    
    auto npcs = editor.getNPCs();
    ASSERT_EQ(npcs.size(), 2);
    EXPECT_EQ(npcs[0]->getName(), "TestBear");
    EXPECT_EQ(npcs[1]->getName(), "TestWerewolf");
}

TEST_F(DungeonEditorTest, AddNPCOutOfBounds) {
    DungeonEditor editor;
    EXPECT_THROW(editor.addNPC(NPCType::Bear, 600, 100, "BadBear"), std::runtime_error);
    EXPECT_THROW(editor.addNPC(NPCType::Werewolf, 100, -50, "BadWerewolf"), std::runtime_error);
}

TEST_F(DungeonEditorTest, SaveAndLoad) {
    DungeonEditor editor1;
    editor1.addNPC(NPCType::Bear, 10, 10, "Bear1");
    editor1.addNPC(NPCType::Werewolf, 20, 20, "Wolf1");  // Изменено
    
    editor1.save("test_save.txt");
    
    DungeonEditor editor2;
    editor2.load("test_save.txt");
    
    auto npcs = editor2.getNPCs();
    ASSERT_EQ(npcs.size(), 2);
    EXPECT_EQ(npcs[0]->getName(), "Bear1");
    EXPECT_EQ(npcs[1]->getName(), "Wolf1");
}

TEST_F(DungeonEditorTest, LoadFromFile) {
    DungeonEditor editor;
    editor.load("test_dungeon.txt");
    
    auto npcs = editor.getNPCs();
    ASSERT_EQ(npcs.size(), 3);
    EXPECT_EQ(npcs[0]->getName(), "Bear1");
    EXPECT_EQ(npcs[1]->getName(), "Wolf1");
    EXPECT_EQ(npcs[2]->getName(), "Rogue1");
}

TEST_F(DungeonEditorTest, BattleWerewolfKillsRogue) {
    DungeonEditor editor;
    editor.addNPC(NPCType::Werewolf, 100, 100, "Wolf1");  // Оборотень убивает Разбойника
    editor.addNPC(NPCType::Rogue, 101, 101, "Rogue1");
    
    editor.battle(10);
    
    auto npcs = editor.getNPCs();
    EXPECT_FALSE(npcs[1]->isAlive());  // Rogue должен быть мертв
    EXPECT_TRUE(npcs[0]->isAlive());   // Werewolf должен быть жив
}

TEST_F(DungeonEditorTest, BattleRogueKillsBear) {
    DungeonEditor editor;
    editor.addNPC(NPCType::Rogue, 100, 100, "Rogue1");  // Разбойник убивает Медведя
    editor.addNPC(NPCType::Bear, 101, 101, "Bear1");
    
    editor.battle(10);
    
    auto npcs = editor.getNPCs();
    EXPECT_FALSE(npcs[1]->isAlive());  // Bear должен быть мертв
    EXPECT_TRUE(npcs[0]->isAlive());   // Rogue должен быть жив
}

TEST_F(DungeonEditorTest, BattleBearKillsWerewolf) {
    DungeonEditor editor;
    editor.addNPC(NPCType::Bear, 100, 100, "Bear1");  // Медведь убивает Оборотня
    editor.addNPC(NPCType::Werewolf, 101, 101, "Wolf1");
    
    editor.battle(10);
    
    auto npcs = editor.getNPCs();
    EXPECT_FALSE(npcs[1]->isAlive());  // Werewolf должен быть мертв
    EXPECT_TRUE(npcs[0]->isAlive());   // Bear должен быть жив
}

TEST_F(DungeonEditorTest, BattleOutOfRange) {
    DungeonEditor editor;
    editor.addNPC(NPCType::Werewolf, 100, 100, "Wolf1");
    editor.addNPC(NPCType::Rogue, 200, 200, "Rogue1");
    
    editor.battle(10);
    
    auto npcs = editor.getNPCs();
    EXPECT_TRUE(npcs[0]->isAlive());
    EXPECT_TRUE(npcs[1]->isAlive());
}

TEST_F(DungeonEditorTest, FactoryCreate) {
    auto bear = NPCFactory::create(NPCType::Bear, 1, 1, "Bear");
    auto wolf = NPCFactory::create(NPCType::Werewolf, 2, 2, "Wolf");  // Изменено
    auto rogue = NPCFactory::create(NPCType::Rogue, 3, 3, "Rogue");
    
    EXPECT_EQ(bear->getType(), NPCType::Bear);
    EXPECT_EQ(wolf->getType(), NPCType::Werewolf);
    EXPECT_EQ(rogue->getType(), NPCType::Rogue);
}

TEST_F(DungeonEditorTest, NPCDeath) {
    auto npc = NPCFactory::create(NPCType::Bear, 1, 1, "TestBear");
    EXPECT_TRUE(npc->isAlive());
    
    npc->markDead();
    EXPECT_FALSE(npc->isAlive());
}

TEST(GameEngineTest, Initialization) {
    GameEngine engine;
    
    // Не можем напрямую проверить private методы, но можем запустить на короткое время
    std::thread t([&engine]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        engine.stop();
    });
    
    // Запускаем движок на очень короткое время
    engine.run();
    t.join();
    
    // Если программа не упала, тест пройден
    SUCCEED();
}

TEST(NPCTest, MovementWithinBounds) {
    std::mt19937 rng(42);
    auto npc = NPCFactory::create(NPCType::Bear, 50, 50, "Test");
    
    // Для теста используем фиксированный генератор
    npc->setRandomEngine(rng);
    
    // Двигаем NPC много раз
    for (int i = 0; i < 100; i++) {
        npc->move(100, 100);
        int x = npc->getX();
        int y = npc->getY();
        
        EXPECT_GE(x, 0);
        EXPECT_LT(x, 100);
        EXPECT_GE(y, 0);
        EXPECT_LT(y, 100);
    }
}

TEST(NPCTest, DiceRoll) {
    std::mt19937 rng(42);
    auto npc = NPCFactory::create(NPCType::Bear, 0, 0, "Test");
    npc->setRandomEngine(rng);
    
    // Бросаем кубик несколько раз
    for (int i = 0; i < 100; i++) {
        int roll = npc->rollDice();
        EXPECT_GE(roll, 1);
        EXPECT_LE(roll, 6);
    }
}

TEST(VisitorTest, CanKillRules) {
    Observable obs;
    NPCVisitor visitor(10, obs);
    
    // Оборотень убивает Разбойника
    EXPECT_TRUE(visitor.canKill(NPCType::Werewolf, NPCType::Rogue));
    
    // Разбойник убивает Медведя
    EXPECT_TRUE(visitor.canKill(NPCType::Rogue, NPCType::Bear));
    
    // Медведь убивает Оборотня
    EXPECT_TRUE(visitor.canKill(NPCType::Bear, NPCType::Werewolf));
    
    // Обратные комбинации не должны работать
    EXPECT_FALSE(visitor.canKill(NPCType::Rogue, NPCType::Werewolf));
    EXPECT_FALSE(visitor.canKill(NPCType::Bear, NPCType::Rogue));
    EXPECT_FALSE(visitor.canKill(NPCType::Werewolf, NPCType::Bear));
    
    // Одинаковые типы не убивают друг друга
    EXPECT_FALSE(visitor.canKill(NPCType::Bear, NPCType::Bear));
    EXPECT_FALSE(visitor.canKill(NPCType::Werewolf, NPCType::Werewolf));
    EXPECT_FALSE(visitor.canKill(NPCType::Rogue, NPCType::Rogue));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}