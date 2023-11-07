#include "pch.h"
#include "CaesarCipher.h"
#include <utility>
#include <limits.h>
#include <iostream>
#include <cctype>


void encryptText(char* rawText, int key) {
    int start = 32;
    int end = 126;
    int range = end - start + 1;

    key = (key % range + range) % range;

    for (size_t i = 0; rawText[i] != '\0'; i++) {

        int charToInt = static_cast<int>(rawText[i]);

        rawText[i] = static_cast<char>(start + (charToInt - start + key) % range);
    }
}


void decryptText(char* encryptedText, int key) {
    int start = 32;
    int end = 126;
    int range = end - start + 1;

    key = (key % range + range) % range;

    for (size_t i = 0; encryptedText[i] != '\0'; i++) {

        int charToInt = static_cast<int>(encryptedText[i]);
        int shifted = charToInt - key;

        if (shifted < start) {
            shifted += range;
        }
        encryptedText[i] = static_cast<char>(start + (shifted - start) % range);
    }
}


void printText(char* encryptedText) {

    for (int i = 0; encryptedText[i] != '\0'; i++)
    {
        std::cout << encryptedText[i];
    }
}