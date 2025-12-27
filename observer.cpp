#include "observer.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <iomanip>

void ConsoleObserver::onKill(const std::string& killer, const std::string& victim) {
    std::cout << "[BATTLE] " << killer << " killed " << victim << std::endl;
}

FileObserver::FileObserver() {
}

FileObserver::~FileObserver() {
}

void FileObserver::onKill(const std::string& killer, const std::string& victim) {
    std::ofstream file("log.txt", std::ios::app);
    if (file.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
        localtime_s(&timeinfo, &time_t_now);
        
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
        
        file << buffer << " KILL: " << killer << " -> " << victim << std::endl;
    }
}

void Observable::addObserver(std::shared_ptr<Observer> observer) {
    observers.push_back(observer);
}

void Observable::notifyKill(const std::string& killer, const std::string& victim) {
    for (auto& observer : observers) {
        observer->onKill(killer, victim);
    }
}