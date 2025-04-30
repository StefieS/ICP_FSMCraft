#include "fsm/FSM.h"
#include "fsm/State.h"
#include "fsm/Transition.h"
#include <memory>
#include <iostream>

int main() {
    auto fsm = std::make_shared<FSM>("TOF5s");

    // === Add States ===
    auto idle = std::make_shared<State>("IDLE", R"(output("out", 0);)", true);
    auto active = std::make_shared<State>("ACTIVE", R"(output("out", 1);)");
    auto timing = std::make_shared<State>("TIMING", "");

    fsm->addState(idle);
    fsm->addState(active);
    fsm->addState(timing);

    // === Add Transitions ===
    auto t1 = std::make_shared<Transition>("IDLE", "ACTIVE");
    t1->setInputEvent("in");
    t1->setGuardCondition("value == 1");
    fsm->addTransition(t1);

    auto t2 = std::make_shared<Transition>("ACTIVE", "TIMING");
    t2->setInputEvent("in");
    t2->setGuardCondition("value == 0");
    fsm->addTransition(t2);

    auto t3 = std::make_shared<Transition>("TIMING", "ACTIVE");
    t3->setInputEvent("in");
    t3->setGuardCondition("value == 1");
    fsm->addTransition(t3);

    // We'll skip the timeout transition for now

    // === Start simulation loop ===
    std::string inputVal;
    while (true) {
        std::cout << "Enter value for input 'in' (or q to quit): ";
        std::cin >> inputVal;
        if (inputVal == "q") break;

        fsm->setInput("in", inputVal);
        fsm->process();
        fsm->printStatus();
    }

    return 0;
}
