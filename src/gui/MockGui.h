#pragma once
#include "IMainWindow.h"
#include "InputQueue.h"

class MockGui : public IMainWindow {
public:
    void printLog(std::string logMessage) override;
    void showOutput(std::string outputID, std::string outputValue) override;
    void showError(std::string errorMessage) override;
    void setRunning() override;
    void loadFSMFromJson(std::string pathToJson) override;
    void highlightItem(bool on, IActivable& item) override;
    void startInputThread(InputQueue& inputQueue);
private:
    bool running;
    
};