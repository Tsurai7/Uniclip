#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Crypto/Crypto.h"
#include "Utils/Data/Data.h"
#include "iostream"
#include <unistd.h>


void* handleClip(void* arg) {
    std::string startClip = runGetClipCommand();

    while (1) {
        std::string localClip = runGetClipCommand();

        if (localClip != startClip) {
            startClip = localClip;
            sendBroadcast(localClip.c_str());
        }

        sleep(1); // !!! sending extra space without sleep (maybe some troubles with threads)
    }
}


int main(int argc, char* argv[]) {

    pthread_t recieveBroadcastThread, handleClipThread;

    // Creating thread for func
    if (pthread_create(&recieveBroadcastThread, NULL, receiveBroadcast, NULL) != 0) {
        printf("recieveBroadcastThread_create");
        exit(EXIT_FAILURE);
    }


    if (pthread_create(&handleClipThread, NULL, handleClip, NULL) != 0) {
        printf("handleClipThread_create");
        exit(EXIT_FAILURE);
    }


    // Waiting for thread ending
    if (pthread_join(recieveBroadcastThread, NULL) != 0) {
        printf("recieveBroadcastThread_join");
        exit(EXIT_FAILURE);
    }


    if (pthread_join(handleClipThread, NULL) != 0) {
        printf("handleClipThread_join");
        exit(EXIT_FAILURE);
    }

    return 0;

}