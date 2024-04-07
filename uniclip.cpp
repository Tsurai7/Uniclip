#include "Utils/Notifications/Notify.h"
#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Logging/Logging.h"
#include "Utils/Crypto/Crypto.h"
#include "Utils/Data/Data.h"
#include <unistd.h>


using namespace std;


int main(int argc, char* argv[])
{
    pthread_t receiveBroadcastThread, tcpSocketThread, manageClipThread;

    sendBroadcast(defineLocalEn0Interface());

    // Creating thread for func
    if (pthread_create(&receiveBroadcastThread, NULL, receiveBroadcast, NULL) != 0) {
        printf("receiveBroadcastThread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tcpSocketThread, NULL, setUpTCPServer, NULL) != 0) {
        printf("receiveBroadcastThread_create");
        exit(EXIT_FAILURE);
    }


    if (pthread_create(&manageClipThread, NULL, manageClip, NULL) != 0) {
        printf("manageClipThread_create");
        exit(EXIT_FAILURE);
    }


    // Waiting for thread ending
    if (pthread_join(receiveBroadcastThread, NULL) != 0) {
        printf("receiveBroadcastThread_join");
        exit(EXIT_FAILURE);
    }


    if (pthread_join(manageClipThread, NULL) != 0) {
        printf("manageClipThread_join");
        exit(EXIT_FAILURE);
    }

    return 0;

}