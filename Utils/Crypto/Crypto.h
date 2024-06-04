#ifndef UNICLIP_CRYPTO_H
#define UNICLIP_CRYPTO_H


int gcd(int a, int b);
int modPow(int base, int exponent, int n);
void generateRSAKeys(int p, int q, int& publicKey, int& privateKey, int& n);
std::string encryptRSA(const std::string& message, int publicKey, int n);
std::string decryptRSA(const std::string& encryptedMessage, int privateKey, int n);

#endif