#pragma once

#ifdef CAESARCIPHER_EXPORTS
#define CAESARCIPHER_API __declspec(dllexport)
#else
#define CAESARCIPHER_API __declspec(dllimport)
#endif

extern "C" CAESARCIPHER_API void encryptText(char* rawText, int key);

extern "C" CAESARCIPHER_API void decryptText(char* rawText, int key);

extern "C" CAESARCIPHER_API void printText(char* encryptedText);