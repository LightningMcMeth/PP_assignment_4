#include "pch.h"
#include "CaesarCipher.h"
#include <utility>
#include <limits.h>
#include <iostream>
#include <cctype>


void encryptText(char* rawText, int key) {
    int range = 128;

    key = (key % range + range) % range;

    for (size_t i = 0; rawText[i] != '\0'; i++) {

        if (rawText[i] == ' ' || rawText[i] == '\n') {

            continue;
        }

        rawText[i] = (rawText[i] + key) % range;
    }
}

void decryptText(char* encryptedText, int key) {
    int range = 128;

    key = (key % range + range) % range;

    for (size_t i = 0; encryptedText[i] != '\0'; i++) {

        if (encryptedText[i] == ' ' || encryptedText[i] == '\n') {

            continue;
        }

        encryptedText[i] = (encryptedText[i] - key + range) % range;
    }
}

void printText(char* encryptedText) {

    for (size_t i = 0; encryptedText[i] != '\0'; i++)
    {
        std::cout << encryptedText[i];
    }
}