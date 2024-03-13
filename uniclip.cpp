#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#include "Utils/Clipboard/Clipboard.h"
#include "Utils/Network/Network.h"
#include "Utils/Data/Data.h"
#include "Utils/Crypto/Crypto.h"

#define BROADCAST_PORT 8787
#define BROADCAST_ADDRESS "192.168.1.255"

using namespace std;

void sendBroadcast(char* message);

int main() {
    //sendBroadcast("hiiiii");

    // runSetClipCommand("hi");

    // cout << runGetClipCommand() << endl;

    const char *inputData = "Text messaging, or texting, is the act of composing and sending electronic messages, typically consisting of alphabetic and numeric characters, between two or more users of mobile devices, desktops/laptops, or another type of compatible computer. Text messages may be sent over a cellular network or may also be sent via satellite or Internet connection.\n"
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
    printf("Compressed data: %lu\n", strlen(compressedData));
    printf("Decompressed data: %lu\n", strlen(decompressedData));

    return 0;
}

void sendBroadcast(char* message) {
    int socket_fd;
    struct sockaddr_in broadcast_address;

    // Создание сокета
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса для широковещательной рассылки
    memset(&broadcast_address, 0, sizeof(broadcast_address));
    broadcast_address.sin_family = AF_INET;
    broadcast_address.sin_port = htons(BROADCAST_PORT);
    if (inet_pton(AF_INET, BROADCAST_ADDRESS, &broadcast_address.sin_addr) <= 0) {
        perror("Invalid address");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    int broadcast_enable = 1;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) == -1) {
        perror("Setsockopt failed");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {

        if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&broadcast_address, sizeof(broadcast_address)) == -1) {
            perror("Sendto failed");
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        printf("Broadcast message sent: %s\n", message);

        sleep(1);
    }

    close(socket_fd);
}

