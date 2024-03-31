#ifndef UNICLIP_CRYPTO_H
#define UNICLIP_CRYPTO_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

std::string rsaDecrypt(const std::string &encryptedText, int privateKey, int modulus);
std::string rsaEncrypt(const std::string &plainText, int publicKey, int modulus);
void generateRSAKeys(int* publicKey, int* privateKey, int* modulus);



#endif