#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class TextEditor {
public:
    std::vector<std::vector<std::string>> userTextD;

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
                //file << '\n';
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

class LineEditor {
public:
    static std::string exchangeBuffer;  //the problem lies within this variable. Get a linker error when I try to use it.

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

class Program {
public:
    TextEditor userText;

    std::string exchangeBuffer;
    std::string textInput;
    std::vector<size_t> indexSpecifier;
    std::string filename;

    int cmdType;

    Program() : userText(1), exchangeBuffer(""), textInput(""), filename(""), cmdType(0) {
    }

    Program(int initialSize, const std::string& inputText, const std::vector<size_t>& indices, const std::string& file, int commandType)
        : userText(initialSize), exchangeBuffer(""), textInput(inputText), indexSpecifier(indices), filename(file), cmdType(commandType) {
    }

    void run() {
        while (1) {

            std::cout << "\nChoose a command (Line: " << userText.userTextD.size() << "): ";
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

                FileManager::writeToFile(filename, userText.userTextD);

                std::cout << "\ntext written to file.";

                break;
            case 5:

                std::cin.ignore();
                std::cout << "\nInput file name: ";
                std::getline(std::cin, filename);

                FileManager::readFromFile(filename, userText.userTextD);

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
}
