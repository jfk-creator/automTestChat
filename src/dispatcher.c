#include "dispatcher.h"

bool mainDispatcher(int selectedTest) {
    switch (selectedTest) {
        case RUN_ALL_TESTS:
            runTest_01();
            break;

        default:
            errorPrint("No Test selected");
            break;
    }
    return true;
}