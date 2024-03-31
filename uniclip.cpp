#include "Utils/Notifications/Notify.h"
#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Logging/Logging.h"
#include "Utils/Crypto/Crypto.h"
#include "Utils/Data/Data.h"
#include <unistd.h>


using namespace std;


int main(int argc, char* argv[]) {

    logger("NEW SESSION STARTED", "");

    const char* helpMessage = "Uniclip - Universal Clipboard\n"
                         "With Uniclip, you can copy from one device and paste on another.\n"
                         "Usage: uniclip  [--key/-k ] [ --debug/-d ] [ --help/-h ]\n"
                         "Examples:\n"
                         "Running just  'uniclip' will start a new clipboard.\n"
                         "Refer to https://github.com/Tsurai7/Kharashun-Demidovich_OSISP_prj_2024\n";

    pthread_t receiveBroadcastThread, manageClipThread;

    // Creating thread for func
    if (pthread_create(&receiveBroadcastThread, NULL, receiveBroadcast, NULL) != 0) {
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