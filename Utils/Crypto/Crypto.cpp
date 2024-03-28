#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

int gcd(int a, int b) {
    if (b == 0)
        return a;

    return gcd(b, a % b);
}


bool isPrime(int n) {
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;
    if (n % 2 == 0 || n % 3 == 0)
        return false;

    for (int i = 5; i * i <= n; i = i + 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }
    return true;
}

void generateKeys(int p, int q, int &n, int &e, int &d) {

    if (!isPrime(p) || !isPrime(q)) {
        cout << "Both numbers must be prime." << endl;
        return;
    }

    n = p * q;
    int phi = (p - 1) * (q - 1);

    // Choose e such that 1 < e < phi and gcd(e, phi) = 1
    for (e = 2; e < phi; e++) {
        if (gcd(e, phi) == 1) break;
    }

    // Calculate d such that (d * e) % phi = 1
    for (d = 1; d < phi; d++) {
        if ((d * e) % phi == 1) break;
    }
}


vector<int> encrypt(const string& msg, int e, int n) {

    vector<int> encryptedMsg;

    for (char c : msg) {
        encryptedMsg.push_back(static_cast<int>(pow(c, e)) % n);
    }

    return encryptedMsg;
}


string decrypt(const vector<int>& encryptedMsg, int d, int n) {

    string decryptedMsg;

    for (int num : encryptedMsg) {
        int decryptedChar = static_cast<int>(std::pow(num, d)) % n;
        decryptedMsg.push_back(static_cast<char>(decryptedChar));
    }
    return decryptedMsg;
}