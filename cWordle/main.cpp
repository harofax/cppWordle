#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <map>
#include <sstream>
#include <cwchar>

#define NOMINMAX
#include <Windows.h>

#define FOREGROUND(color, text) "\x1B[" << static_cast<int>(color) << "m" << text << "\033[0m"
#define BACKGROUND(color, text) "\033[3;42;" << static_cast<int>(color) << "m" << text << "\033[0m"

enum class ForegroundColor : int {
    Black = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    BrightBlack = 90,
    BrightRed = 91,
    BrightGreen = 92,
    BrightYellow = 93,
    BrightCyan = 96
};

enum class BackgroundColor : int {
    Red = 41,
    Green = 42,
    Yellow = 43,
    BrightRed = 101,
    BrightGreen = 102,
    BrightYellow = 103,
    BrightBlack = 100
};

constexpr int WORD_LENGTH = 5;

constexpr int MAX_GUESSES = 6;

bool get_random_word(const char* filename, std::string& word);

std::string get_guess();

int parse_guess(std::string& word, std::map<char, int> word_data, std::string& guess, std::stringstream& output);


void init_console()
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 24;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_BOLD;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

int main()
{
    init_console();


    std::string word;

    get_random_word("words.txt", word);

    std::map<char, int> word_data;

    for (const char c : word)
    {
        word_data[c]++;
    }

    //std::cout << FOREGROUND(ForegroundColor::BrightYellow, word) << std::endl;

    std::cout << FOREGROUND(ForegroundColor::BrightYellow, "WELCOME TO CWORDLE (COOL WORDLE)\n");
    std::cout << FOREGROUND(ForegroundColor::BrightYellow, "-----------------------------------\n");

    std::stringstream output;

    bool won = false;

    int i = 0;

    for (; i < MAX_GUESSES; i++)
    {
    std::cout << FOREGROUND(ForegroundColor::BrightCyan, "Enter a guess: \n");
        auto guess = get_guess();

        // make guess string uppercase
        for (auto& c : guess) c = toupper(c);

        const int correct = parse_guess(word, word_data, guess, output);

        std::cout << output.str() << std::endl;

        // empty output stream
        output.str("");

        if (correct == 5)
        {
            won = true;
            break;
        }
    }

    if ( won )
    {
        std::cout << FOREGROUND(ForegroundColor::BrightGreen, "YOU WON!!!\n");
        std::cout << "It took you " << i+1 << " guesses." << std::endl;
    }
	else
    {
        std::cout << BACKGROUND(BackgroundColor::Red, "Better luck next time, bucko" << std::endl);
        std::cout << "The word was " <<BACKGROUND(BackgroundColor::BrightBlack, word) << "." << std::endl;
    }

    
}

bool get_random_word(const char* filename, std::string& word)
{
    std::ifstream word_file(filename);

    if (!word_file.is_open())
    {
        std::cout << BACKGROUND(BackgroundColor::BrightRed, "Could not open word file!") << std::endl;
        return false;
    }

    srand(time(nullptr));

    // get how many words are in the list
    word_file.seekg(0, std::ifstream::end);
    const int length = word_file.tellg();
    word_file.seekg(0, std::ifstream::beg);

    // we add 1 because of the eof symbol, and then another 1 because of starting at 0
    const int num_words = length / (WORD_LENGTH + 1) + 1;

    const int random_line = rand() % num_words;

    word_file.seekg((WORD_LENGTH + 1) * random_line);

    // put word into the word ref
    std::getline(word_file, word);

    return true;
}

void flush_cin()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string get_guess()
{
    std::string guess;
	while (true)
	{
        std::cin >> guess;
        if (std::cin.fail())
        {
            std::cout << "Invalid input\n";
            flush_cin();
            continue;
        }
        if (guess.length() != WORD_LENGTH)
        {
            std::cout << FOREGROUND(ForegroundColor::BrightRed, "Guess must consist of 5 characters. Please try again: \n");
            flush_cin();
            continue;
        }
        break;
	}

    return guess;
}

int parse_guess(std::string& word, std::map<char, int> word_data, std::string& guess, std::stringstream& output)
{
    int correct_guesses = 0;

    for (int i = 0; i < WORD_LENGTH; i++)
    {
        const auto guess_char = guess[i];
        const auto word_char = word[i];

        if (guess_char == word_char)
        {
            word_data[guess_char]--;
            correct_guesses++;
        }
    }

    for (int i = 0; i < WORD_LENGTH; i++)
    {
        const auto guess_char = guess[i];
        const auto word_char = word[i];

        if (guess_char == word_char)
        {
            output << BACKGROUND(BackgroundColor::BrightGreen, FOREGROUND(ForegroundColor::Black, guess_char));
        }
        else if (word_data[guess_char] > 0)
        {
            output << BACKGROUND(BackgroundColor::BrightYellow, FOREGROUND(ForegroundColor::Black, guess_char));
            word_data[guess_char]--;
            correct_guesses++;
        }
        else
        {
            output << BACKGROUND(BackgroundColor::BrightBlack, guess_char);
        }
    }

    return correct_guesses;
}