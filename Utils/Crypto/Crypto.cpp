#include <iostream>

int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}


int modPow(int base, int exponent, int n) {
    int result = 1;
    base = base % n;
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = (result * base) % n;
        }
        exponent = exponent >> 1;
        base = (base * base) % n;
    }
    return result;
}


void generateRSAKeys(int p, int q, int& publicKey, int& privateKey, int& n) {
    n = p * q;
    int phi = (p - 1) * (q - 1);

    publicKey = 2;
    while (publicKey < phi) {
        if (gcd(publicKey, phi) == 1)
            break;
        else
            publicKey++;
    }

    privateKey = 1;
    while (true) {
        if ((publicKey * privateKey) % phi == 1)
            break;
        else
            privateKey++;
    }
}


std::string encryptRSA(const std::string& message, int publicKey, int n) {
    std::string encryptedMessage = "";
    for (char c : message) {
        int m = c;
        int encryptedChar = modPow(m, publicKey, n);
        encryptedMessage += std::to_string(encryptedChar) + " ";
    }
    return encryptedMessage;
}

std::string decryptRSA(const std::string& encryptedMessage, int privateKey, int n) {
    std::string decryptedMessage = "";
    std::string encryptedChar = "";

    for (char c : encryptedMessage) {
        if (c == ' ') {
            int m = stoi(encryptedChar);
            int decryptedChar = modPow(m, privateKey, n);
            decryptedMessage += char(decryptedChar);
            encryptedChar = "";
        } else {
            encryptedChar += c;
        }
    }
    return decryptedMessage;
}
