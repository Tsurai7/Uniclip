#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Crypto/Crypto.h"
#include "Utils/Data/Data.h"

//using namespace std;

void* handleClip(void* arg) {
    string startClip = runGetClipCommand();

    while (1) {
        string localClip = runGetClipCommand();

        if (localClip != startClip) {
            startClip = localClip;
            sendBroadcast(localClip.c_str());
        }

        sleep(1);
    }
}

void* handleBroadcast(void* arg) {
    char* text;
    receiveBroadcast(&text);
    runSetClipCommand(text);
}

int main(int argc, char* argv[]) {
    //sendBroadcast("hiiiii");

    // runSetClipCommand("hi");

    // cout << runGetClipCommand() << endl;

     /* const char *inputData = "Text messaging, or texting, is the act of composing and sending electronic messages, typically consisting of alphabetic and numeric characters, between two or more users of mobile devices, desktops/laptops, or another type of compatible computer. Text messages may be sent over a cellular network or may also be sent via satellite or Internet connection.\n"
                            "\n"
                            "The term originally referred to messages sent using the Short Message Service (SMS). It has grown beyond alphanumeric text to include multimedia messages using the Multimedia Messaging Service (MMS) and Rich Communication Services (RCS), which can contain digital images, videos, and sound content, as well as ideograms known as emoji (happy faces, sad faces, and other icons), and instant messenger applications (usually the term is used when on mobile devices).\n"
                            "\n"
                            "Text messages are used for personal, family, business, and social purposes. Governmental and non-governmental organizations use text messaging for communication between colleagues. In the 2010s, the sending of short informal messages became an accepted part of many cultures, as happened earlier with emailing.[1] This makes texting a quick and easy way to communicate with friends, family, and colleagues, including in contexts where a call would be impolite or inappropriate (e.g., calling very late at night or when one knows the other person is busy with family or work activities). Like e-mail and voicemail, and unlike calls (in which the caller hopes to speak directly with the recipient), texting does not require the caller and recipient to both be free at the same moment; this permits communication even between busy individuals. Text messages can also be used to interact with automated systems, for example, to order products or services from e-commerce websites or to participate in online contests. Advertisers and service providers use direct text marketing to send messages to mobile users about promotions, payment due dates, and other notifications instead of using postal mail, email, or voicemail.";
    size_t inputSize = strlen(inputData);

    char *compressedData;
    size_t compressedSize;

    compressData(inputData, inputSize, &compressedData, &compressedSize);

    char *decompressedData;
    size_t decompressedSize;

    decompressData(compressedData, compressedSize, &decompressedData, &decompressedSize);

    printf("Input data: %lu\n", strlen(inputData));
    printf("Compressed data: %lu\n", compressedData));
    printf("Decompressed data: %lu\n", strlen(decompressedData)); */

     /*while (1) {
         string ch = runGetClipCommand();

         cout << ch << endl;

         sendBroadcast(ch.c_str());
         sleep(1);
     } */

    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, handleClip, NULL) != 0) {
        printf("Thread 1 creation failed");
        return 1;
    }

    if (pthread_create(&thread2, NULL, handleBroadcast, NULL) != 0) {
        printf("Thread 2 creation failed");
        return 1;
    }

    if (pthread_join(thread1, NULL) != 0) {
        printf("Thread 1 join failed");
        return 1;
    }

    if (pthread_join(thread2, NULL) != 0) {
        printf("Thread 2 join failed");
        return 1;
    }

    return 0;
}


