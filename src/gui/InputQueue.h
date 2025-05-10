#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
#include <utility>

class InputQueue {
public:
    void push(const std::pair<std::string, std::string>& input) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(input);
        cv.notify_one();
    }

    std::pair<std::string, std::string> waitAndPop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !q.empty(); });
        auto val = q.front();
        q.pop();
        return val;
    }

private:
    std::queue<std::pair<std::string, std::string>> q;
    std::mutex mtx;
    std::condition_variable cv;
};
