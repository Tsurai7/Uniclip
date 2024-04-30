#include "Utils/Notifications/Notify.h"
#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Logging/Logging.h"
#include "Utils/Crypto/Crypto.h"
#include <unistd.h>
#include <iostream>

using namespace std;


int main(int argc, char* argv[])
{
    /* pthread_t receiveBroadcastThread, tcpSocketThread, manageClipThread;

    send_broadcast(get_ip_command().c_str());

    // Creating thread for func
    if (pthread_create(&receiveBroadcastThread, NULL, recieve_broadcast, NULL) != 0) {
        printf("receiveBroadcastThread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tcpSocketThread, NULL, run_tcp_server, NULL) != 0) {
        printf("receiveBroadcastThread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&manageClipThread, NULL, manage_clip, NULL) != 0) {
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
    } */

    int p = 61;
    int q = 53;
    int publicKey, privateKey, n;
    generateRSAKeys(p, q, publicKey, privateKey, n);

    string message = "its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf its a very long message sdf ";
    string encryptedMessage = encryptRSA(message, publicKey, n);
    cout << "Encrypted message: " << encryptedMessage << endl;

    string decryptedMessage = decryptRSA(encryptedMessage, privateKey, n);
    cout << "Decrypted message: " << decryptedMessage << endl;

    cout << decryptedMessage.size() << endl;
    cout << encryptedMessage.size();


    return 0;
}