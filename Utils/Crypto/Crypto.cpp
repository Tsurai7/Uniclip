#include <iostream>
#include <string>

// Function to check if a number is prime using the Miller-Rabin primality test
bool isPrime(int n, int k = 5) {
    if (n <= 1) return false;
    if (n <= 3) return true;

    // Write n as 2^r * d + 1
    int r = 0;
    int d = n - 1;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }

    // Witness loop
    for (int i = 0; i < k; i++) {
        int a = 2 + rand() % (n - 3);
        int x = std::pow(a, d);
        x %= n;
        if (x == 1 || x == n - 1) continue;

        for (int j = 0; j < r - 1; j++) {
            x = std::pow(x, 2);
            x %= n;
            if (x == 1) return false;
            if (x == n - 1) break;
        }

        if (x != n - 1) return false;
    }

    return true;
}

// Function to generate a random prime number between min and max
int generatePrime(int min, int max) {
    srand(time(nullptr));

    int prime;
    do {
        prime = rand() % (max - min + 1) + min;
    } while (!isPrime(prime));

    return prime;
}

// Function to calculate the greatest common divisor of two numbers
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Function to find the modular multiplicative inverse
int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return 1;
}

// Function to generate RSA keys
void generateRSAKeys(int* publicKey, int* privateKey, int* modulus) {
    int p = generatePrime(100, 1000);
    int q = generatePrime(100, 1000);
    int n = p * q;
    int phi = (p - 1) * (q - 1);
    int e;
    do {
        e = generatePrime(2, phi - 1);
    } while (gcd(e, phi) != 1);
    int d = modInverse(e, phi);

    *publicKey = e;
    *privateKey = d;
    *modulus = n;

    std::cout << "Generated public key: " << *publicKey << std::endl;
    std::cout << "Generated private key: " << *privateKey << std::endl;
    std::cout << "Generated modulus: " << *modulus << std::endl;
}

// Function to encrypt a string using RSA
std::string rsaEncrypt(const std::string& plainText, int publicKey, int modulus) {
    std::string encryptedText = "";
    for (char c : plainText) {
        int charValue = static_cast<int>(c);
        int encryptedChar = 1;
        for (int i = 0; i < publicKey; i++) {
            encryptedChar *= charValue;
            encryptedChar %= modulus;
        }
        encryptedText += std::to_string(encryptedChar) + " ";
    }
    return encryptedText;
}

// Function to decrypt a string using RSA
std::string rsaDecrypt(const std::string& encryptedText, int privateKey, int modulus) {
    std::string decryptedText = "";
    std::string currentNumber = "";
    for (char c : encryptedText) {
        if (c == ' ') {
            int encryptedChar = std::stoi(currentNumber);
            int decryptedChar = 1;
            for (int i = 0; i < privateKey; i++) {
                decryptedChar *= encryptedChar;
                decryptedChar %= modulus;
            }
            decryptedText += static_cast<char>(decryptedChar);
            currentNumber = "";
        } else {
            currentNumber += c;
        }
    }
    return decryptedText;
}
