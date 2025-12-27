#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <memory>
#include <vector>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onKill(const std::string& killer, const std::string& victim) = 0;
};

class ConsoleObserver : public Observer {
public:
    void onKill(const std::string& killer, const std::string& victim) override;
};

class FileObserver : public Observer {
public:
    FileObserver();
    ~FileObserver();
    void onKill(const std::string& killer, const std::string& victim) override;
};

class Observable {
public:
    void addObserver(std::shared_ptr<Observer> observer);
    void notifyKill(const std::string& killer, const std::string& victim);

private:
    std::vector<std::shared_ptr<Observer>> observers;
};

#endif  