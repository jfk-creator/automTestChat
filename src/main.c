#include "dispatcher.h"
#include "utils.h"

int main(int argc, char** argv) {
    setDebugOn();
    int selectedTest = readArgs(argc, argv);
    mainDispatcher(selectedTest);
}