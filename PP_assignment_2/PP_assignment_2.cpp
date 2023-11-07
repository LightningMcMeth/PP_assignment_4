#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <windows.h>
#include <random>

class TextEditor {
    //std::vector<std::vector<std::string>> userTextD;
public:
    std::vector<std::vector<std::string>> userTextD;

    std::vector<std::vector<std::string>>& GetUserTextD()
    {
        return userTextD;
    }

    TextEditor(int initialSize) {
        userTextD.resize(initialSize);
    }

    void appendText(const std::string& text) {
        userTextD.back().push_back(text);
    }

    void printText() {

        std::cout << std::endl;
        for (const std::vector<std::string>& line : userTextD) 
        {
            if (!line.empty()) {
                for (const std::string& words : line) {
                    std::cout << words;
                }
                std::cout << " (" << line[0].size() << ")";
                std::cout << std::endl;
            }
        }
    }

    void addNewline() {
        userTextD.push_back({});
    }

    void clearText() {
        userTextD.clear();
    }
};


class IDemo
{
public:
    virtual ~IDemo() {}
    virtual void Read(const std::string& filepath, std::vector<std::vector<std::string>>& userText) = 0;
    virtual void Write(const std::string& filepath, std::vector<std::vector<std::string>>& userText) = 0;

};

class Files : public IDemo {
public:
     void Write(const std::string& filepath, std::vector<std::vector<std::string>>& userText) override {
        std::ofstream file(filepath);

        if (file.is_open()) {
            for (const std::vector<std::string>& line : userText)
            {

                if (!line.empty()) {
                    for (const std::string& words : line)
                    {
                        file << words;
                    }
                    file << '\n';
                }
            }
            std::cout << "Text written to file successfully.\n";
        }
        else {
            std::cerr << "Error opening file.\n";
        }
    }

    void Read(const std::string& filepath, std::vector<std::vector<std::string>>& userText) override {

        std::ifstream file(filepath);
        if (file.is_open()) {

            std::string line;
            while (std::getline(file, line))
            {
                userText.push_back({ line });
            }
            std::cout << "File read successfully.\n";
        }
        else {
            std::cerr << "Error opening file or file doesn't exist.\n";
        }
    }
};

class FileManager {
public:
    //these can be used for undo/redo commands
    static void writeToFile(const std::string& fileName, std::vector<std::vector<std::string>>& userText) {
        std::ofstream file(fileName);

        if (file.is_open()) {
            for (const std::vector<std::string>& line : userText)
            {

                if (!line.empty()) {
                    for (const std::string& words : line)
                    {
                        file << words;
                    }
                    file << '\n';
                }
            }
            std::cout << "Text written to file successfully.\n";
        }
        else {
            std::cerr << "Error opening file.\n";
        }
    }

    static void readFromFile(const std::string& fileName, std::vector<std::vector<std::string>>& userText) {
        std::ifstream file(fileName);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line))
            {
                userText.push_back({ line });
            }
            std::cout << "File read successfully.\n";
        }
        else {
            std::cerr << "Error opening file or file doesn't exist.\n";
        }
    }
};

class CaesarCipher {
private:
    HINSTANCE handle;

    typedef void(*encryptTextFunc)(char*, int);
    typedef void(*decryptTextFunc)(char*, int);
    typedef void(*printTextFunc)(char*);

    encryptTextFunc encrypt_ptr;
    decryptTextFunc decrypt_ptr;
    printTextFunc print_ptr;

public:
    CaesarCipher() {
        handle = nullptr;
        encrypt_ptr = nullptr;
        decrypt_ptr = nullptr;
        print_ptr = nullptr;
    }

    CaesarCipher(const std::wstring& path) {
        handle = LoadLibrary(path.c_str());
        if (!handle) {
            throw std::runtime_error("Failed to load the DLL");
        }

        encrypt_ptr = (encryptTextFunc)GetProcAddress(handle, "encryptText");
        if (!encrypt_ptr) {
            throw std::runtime_error("Failed to get the encryptText function");
        }

        decrypt_ptr = (decryptTextFunc)GetProcAddress(handle, "decryptText");
        if (!decrypt_ptr) {
            throw std::runtime_error("Failed to get the decryptText function");
        }

        print_ptr = (printTextFunc)GetProcAddress(handle, "printText");
        if (!print_ptr) {
            throw std::runtime_error("Failed to get the printText function");
        }
    }

    ~CaesarCipher() {
        if (handle) {
            FreeLibrary(handle);
        }
    }

    void encrypt(char* text, int key) {
        encrypt_ptr(text, key);
    }

    void decrypt(char* text, int key) {
        decrypt_ptr(text, key);
    }

    void print(char* text) {
        print_ptr(text);
    }
};

class LineEditor {
public:
    static std::vector<size_t> splitIntoVector(const std::string& userInput) {

        std::vector<size_t> tokens;
        std::istringstream intString(userInput);
        std::string token;
        
        while (std::getline(intString, token, ' '))
        {
            tokens.push_back(std::stoul(token));
        }

        return tokens;
    }

    static void deleteString(size_t lineIndex, size_t startIndex, size_t numOfChars, std::vector<std::vector<std::string>>& userText) {

        if (lineIndex >= 0 && lineIndex < userText.size()) {

            if (startIndex >= 0 && startIndex + numOfChars <= userText[lineIndex][0].size()) {
                userText[lineIndex][0].erase(userText[lineIndex][0].begin() + startIndex, userText[lineIndex][0].begin() + startIndex + numOfChars);
            }
            else {
                std::cerr << "\nerror: invalid line indices\n";
            }
        }
        else {
            std::cerr << "\nerror: out of bounds line index\n";
        }
    }

    static void cutString(size_t lineIndex, size_t startIndex, size_t numOfChars, std::vector<std::vector<std::string>>& userText, std::string& exchangeBuffer) {

        if (lineIndex >= 0 && lineIndex < userText.size()) {

            if (startIndex >= 0 && startIndex + numOfChars < userText[lineIndex][0].size()) {

                exchangeBuffer = userText[lineIndex][0].substr(startIndex, numOfChars);
                userText[lineIndex][0].erase(userText[lineIndex][0].begin() + startIndex, userText[lineIndex][0].begin() + startIndex + numOfChars);
            }
            else {
                std::cerr << "\nerror: invalid line indices\n";
            }
        }
        else {
            std::cerr << "\nerror: out of bounds line index\n";
        }
    }

    static void pasteString(size_t lineIndex, size_t startIndex, std::vector<std::vector<std::string>>& userText, const std::string& exchangeBuffer) {
        if (lineIndex >= 0 && lineIndex < userText.size()) {

            if (startIndex >= 0) {

                userText[lineIndex][0].insert(startIndex, exchangeBuffer);
            }
            else {
                std::cerr << "\nerror: invalid line indices\n";
            }
        }
        else {
            std::cerr << "\nerror: out of bounds line index\n";
        }
    }

    //can't copy a sentence into an entirely new row. I guess it's because I'm inserting into an index that doesn't exist yet, as opposed to appending.
    static void copyString(size_t lineIndex, size_t startIndex, size_t numOfChars, std::vector<std::vector<std::string>>& userText, std::string& exchangeBuffer) {

        if (lineIndex >= 0 && lineIndex < userText.size()) {

            if (startIndex >= 0 && startIndex + numOfChars <= userText[lineIndex][0].size()) {

                clearExchangeBuffer(exchangeBuffer);

                for (size_t i = startIndex; i < numOfChars; i++)
                {
                    exchangeBuffer += userText[lineIndex][0][i];
                }
                std::cout << userText[lineIndex][0].size();
            }
            else {
                std::cerr << "\nerror: invalid line indices\n";
            }
        }
        else {
            std::cerr << "\nerror: out of bounds line index\n";
        }
    }

    static void insertString(size_t lineIndex, size_t startIndex, size_t numOfChars, std::vector<std::vector<std::string>>& userText, std::string& textInput) {

        if (lineIndex >= 0 && lineIndex < userText.size()) {

            if (startIndex >= 0 && startIndex + numOfChars <= userText[lineIndex][0].size()) {

                userText[lineIndex][0].erase(userText[lineIndex][0].begin() + startIndex, userText[lineIndex][0].begin() + startIndex + numOfChars);
                userText[lineIndex][0].insert(startIndex, textInput);
            }
            else {
                std::cerr << "\nerror: invalid line indices\n";
            }
        }
        else {
            std::cerr << "\nerror: out of bounds line index\n";
        }
    }

    static void clearExchangeBuffer(std::string& exchangeBuffer) {  
        exchangeBuffer = "";
    }
};


//laptop
//PC C:\\Users\\Gamer\\Documents\\gamer directories\\university stuff\\PP_assignment_4\\CaesarsCipher\\Debug\\CaesarsCipher.dll
class Program {
public:
    TextEditor userText;

    std::string exchangeBuffer;
    std::string textInput;
    std::vector<size_t> indexSpecifier;
    std::string filename;

    CaesarCipher CaesarCipher;
    IDemo* fileManager;

    std::string filepaths[2];
    int key;

    int cmdType;

    Program()
      : userText(1),
        exchangeBuffer(""),
        textInput(""),
        filename(""),
        cmdType(0),
        CaesarCipher(TEXT("C:\\Users\\Gamer\\Documents\\gamer directories\\university stuff\\PP_assignment_4\\CaesarsCipher\\Debug\\CaesarsCipher.dll")),
        fileManager(nullptr),
        filepaths{},
        key(0)
    {
        fileManager = new Files();
    }


    Program(int initialSize, const std::string& inputText, const std::vector<size_t>& indices, const std::string& file, int commandType)
        : userText(initialSize), exchangeBuffer(""), textInput(inputText), indexSpecifier(indices), filename(file), cmdType(commandType), fileManager(), filepaths{}, key(0) {
    }

    void run() {
        while (1) {

            std::cout << "\nChoose a command (Line: " << userText.GetUserTextD().size() << "): ";
            std::cin >> cmdType;

            switch (cmdType)
            {
            case 1:

                std::cin.ignore();
                std::cout << "\nEnter text to append: ";

                std::getline(std::cin, textInput);
                userText.appendText(textInput);

                std::cout << "\nText appended: " + textInput;

                break;
            case 2:

                //printing num of chars per line doesn't work entirely properly after transition to class
                userText.printText();
                std::cout << "\n";

                break;
            case 3:

                userText.addNewline();
                std::cout << "\nNewline added.";

                break;
            case 4:

                std::cin.ignore();
                std::cout << "\nInput file name: ";
                std::getline(std::cin, filename);

                FileManager::writeToFile(filename, userText.GetUserTextD());

                std::cout << "\ntext written to file.";

                break;
            case 5:

                std::cin.ignore();
                std::cout << "\nInput file name: ";
                std::getline(std::cin, filename);

                FileManager::readFromFile(filename, userText.GetUserTextD());

                std::cout << "\ntext read from file.";

                break;
            case 8:

                std::cin.ignore();
                std::cout << "\nSpecify line, starting index, and number of characters: ";
                std::getline(std::cin, textInput);

                indexSpecifier = LineEditor::splitIntoVector(textInput);
                LineEditor::deleteString(indexSpecifier[0], indexSpecifier[1], indexSpecifier[2], userText.userTextD);

                std::cout << "\nString deleted. Line: " << indexSpecifier[0] << ", index: " << indexSpecifier[1] << ", length: " << indexSpecifier[2];

                break;
            case 11:

                std::cin.ignore();
                std::cout << "\nSpecify line, starting index, and number of characters: ";
                std::getline(std::cin, textInput);

                indexSpecifier = LineEditor::splitIntoVector(textInput);
                LineEditor::cutString(indexSpecifier[0], indexSpecifier[1], indexSpecifier[2], userText.userTextD, exchangeBuffer);

                std::cout << "\nString cut. Line: " << indexSpecifier[0] << ", index: " << indexSpecifier[1] << ", length: " << indexSpecifier[2];
                std::cout << "\nExchange buffer: " << exchangeBuffer;

                break;
            case 12:

                std::cin.ignore();
                std::cout << "\nSpecify line, starting index, and number of characters: ";
                std::getline(std::cin, textInput);

                indexSpecifier = LineEditor::splitIntoVector(textInput);
                LineEditor::pasteString(indexSpecifier[0], indexSpecifier[1], userText.userTextD, exchangeBuffer);

                std::cout << "\nString pasted. Line: " << indexSpecifier[0] << ", index: " << indexSpecifier[1] << ", length: " << indexSpecifier[2];
                std::cout << "\nExchange buffer: " << exchangeBuffer;


                break;
            case 13:

                std::cin.ignore();
                std::cout << "\nSpecify line, starting index, and number of characters: ";
                std::getline(std::cin, textInput);

                indexSpecifier = LineEditor::splitIntoVector(textInput);
                LineEditor::copyString(indexSpecifier[0], indexSpecifier[1], indexSpecifier[2], userText.userTextD, exchangeBuffer);

                std::cout << "\nString copied. Line: " << indexSpecifier[0] << ", index: " << indexSpecifier[1] << ", length: " << indexSpecifier[2];
                std::cout << "\nExchange buffer: " << exchangeBuffer;

                break;
            case 14:

                std::cin.ignore();
                std::cout << "\nSpecify line, starting index, and number of characters: ";
                std::getline(std::cin, textInput);

                indexSpecifier = LineEditor::splitIntoVector(textInput);

                std::cin.ignore();
                std::cout << "\nEnter text to insert: ";
                std::getline(std::cin, textInput);

                LineEditor::insertString(indexSpecifier[0], indexSpecifier[1], indexSpecifier[2], userText.userTextD, textInput);

                std::cout << "\nString inserted. Line: " << indexSpecifier[0] << ", index: " << indexSpecifier[1] << ", length: " << indexSpecifier[2];
                std::cout << "\nExchange buffer: " << exchangeBuffer;

                break;
            case 15:
                //system('cls');
                break;
            case 20:
                //encrypt   //C:/Users/Gamer/Documents/gamer directories/university stuff/PP_assignment_4/PP_assignment_2/test.txt

                std::cin.ignore();
                std::cout << "\nEnter input file path: ";
                std::getline(std::cin, filepaths[0]);

                std::cin.ignore();
                std::cout << "\nEnter outnput file path: ";
                std::getline(std::cin, filepaths[1]);

                std::cin.ignore();
                std::cout << "\nEnter key: ";
                std::cin >> key;

                fileManager->Read(filepaths[0], userText.userTextD);

                for (auto& line : userText.userTextD) {
                    if (!line.empty()) {
                        char* text = &line[0][0];
                        CaesarCipher.encrypt(text, key);
                    }
                }

                fileManager->Write(filepaths[1], userText.userTextD);

                break;
            case 21:
                //decrypt

                std::cin.ignore();
                std::cout << "\nEnter input file path: ";
                std::getline(std::cin, filepaths[0]);

                std::cin.ignore();
                std::cout << "\nEnter outnput file path: ";
                std::getline(std::cin, filepaths[1]);

                std::cin.ignore();
                std::cout << "\nEnter key: ";
                std::cin >> key;

                userText.clearText();
                fileManager->Read(filepaths[0], userText.userTextD);

                for (auto& line : userText.userTextD) {
                    if (!line.empty()) {

                        char* text = &line[0][0];
                        CaesarCipher.decrypt(text, key);
                    }
                }

                fileManager->Write(filepaths[1], userText.userTextD);

                break;
            case 22: { //secret mode

                std::random_device rd;

                std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, 100000);

                int randKey = distrib(gen);

                std::cin.ignore();
                std::cout << "\nEnter input file path: ";
                std::getline(std::cin, filepaths[0]);

                std::cin.ignore();
                std::cout << "\nEnter outnput file path: ";
                std::getline(std::cin, filepaths[1]);

                userText.clearText();
                fileManager->Read(filepaths[0], userText.userTextD);

                for (auto& line : userText.userTextD) {
                    if (!line.empty()) {

                        char* text = &line[0][0];
                        CaesarCipher.encrypt(text, randKey);
                    }
                }

                fileManager->Write(filepaths[1], userText.userTextD);

                break;
            }
            case 99:
                exit(0);
                break;
            default:

                std::cout << "\n\n---=== List of commands ===---\n\n" << "8. Delete string.\n 11. Cut string. \n 12. Paste string. \n 13. Copy string. \n 14. Insert string.\n";
                break;
            }
        }
    }
};

int main()
{
    Program program;

    program.run();

    return 0;
}
