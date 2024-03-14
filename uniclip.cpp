#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Crypto/Crypto.h"
#include "Utils/Data/Data.h"

using namespace std;

void* handleClip(void* arg) {
    string startClip = runGetClipCommand();

    while (1) {
        string localClip = runGetClipCommand();

        if (localClip != startClip) {
            startClip = localClip;
            sendBroadcast(localClip.c_str());
        }

        sleep(1); // !!!sending extra space without sleep (maybe some troubles with threds)
    }
}
int main(int argc, char* argv[]) {
    pthread_t thread_1, thread_2;

    // Создание потока для функции receiveBroadcast
   if (pthread_create(&thread_1, NULL, receiveBroadcast, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&thread_2, NULL, handleClip, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Ожидание завершения потока
    if (pthread_join(thread_1, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    // Ожидание завершения потока
    if (pthread_join(thread_2, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }




    return 0;
}