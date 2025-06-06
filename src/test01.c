#include "test01.h"

bool runTest_01() {
    normalPrint("Starting Test01");
    pthread_t threadID;
    ClientArgs* clientArgs = malloc(sizeof(ClientArgs));
    if (clientArgs == NULL) {
        errorPrint("clientArgs not allocated");
    }
    strncpy(clientArgs->name, "Frederike", 32);
    int result = pthread_create(&threadID, NULL, client, (void*)clientArgs);
    if (result != 0) {
        errorPrint("Failed to create client thread");
        free(clientArgs);
        return false;
    } else {
        debugPrint("Client thread created successfully with ID: %lu", (unsigned long)threadID);
    }
    return true;
}