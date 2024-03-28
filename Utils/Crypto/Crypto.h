#ifndef UNICLIP_CRYPTO_H
#define UNICLIP_CRYPTO_H

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

void generateKeys(int p, int q, int &n, int &e, int &d);
vector<int> encrypt(const string& msg, int e, int n);
string decrypt(const vector<int>& encryptedMsg, int d, int n);

#endif