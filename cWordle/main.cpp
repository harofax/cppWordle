#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <unordered_set>

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

std::unordered_set<std::string> valid_words;
std::vector<std::string> word_list;

std::string get_random_word();

bool load_words(const char* filename, 
    std::vector<std::string>& word_list, 
    std::unordered_set<std::string>& valid_words);

std::string get_guess();

int parse_guess(const std::string& word, std::map<char, int> word_data, const std::string& guess, std::stringstream& output);


int main()
{
    srand(time(0));
    if (!load_words("words.txt", word_list, valid_words)) 
    {
        std::cout << BACKGROUND(BackgroundColor::BrightRed, 
            "COULD NOT LOAD WORD LIST. MAKE SURE A \"words.txt\" file exists where this executable resides.")
            << std::endl;

        return -1;
    }

    char play_again = 'y';

    do {
        std::string word = get_random_word();

        std::map<char, int> word_data;

        for (const char c : word)
        {
            word_data[c]++;
        }

        // Uncomment to cheat vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
         std::cout << FOREGROUND(ForegroundColor::BrightYellow, word) << std::endl;
        // Uncomment to cheat ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
            for (auto& c : guess)
            {
                c = toupper(c);
            }

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

        if (won)
        {
            std::cout << FOREGROUND(ForegroundColor::BrightGreen, "YOU WON!!!\n");
            std::cout << "It took you " << i + 1 << " guesses." << std::endl;
        }
        else
        {
            std::cout << BACKGROUND(BackgroundColor::Red, "Better luck next time, bucko" ) << std::endl;
            std::cout << "The word was " << BACKGROUND(BackgroundColor::BrightBlack, word) << "." << std::endl;
        }

        std::cout << "Play again? \n";
        do
        {
            std::cout << "Please enter [y] or [n]:" << std::endl;
            std::cin >> play_again;
            play_again = std::tolower(play_again);
        } while (play_again != 'y' && play_again != 'n');
        
    } while (std::tolower(play_again) == 'y');    
}

bool load_words(const char* filename, std::vector<std::string>& word_list, std::unordered_set<std::string>& valid_words) 
{
    std::ifstream word_file(filename);

    if (!word_file.is_open())
    {
        std::cout << BACKGROUND(BackgroundColor::BrightRed, "Could not open word file!") << std::endl;
        return false;
    }

    std::string current_word;

    while (std::getline(word_file, current_word))
    {
        word_list.push_back(current_word);
        valid_words.insert(current_word);
    }

    return true;
}

std::string get_random_word()
{
    

    const int random_index = rand() % word_list.size();

    return word_list[random_index];
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

        for (auto &c : guess)
        {
	        if (!std::isalpha(c))
	        {
                // set fail bit, so we flush and retry
                std::cin.setstate(std::ios_base::failbit);
                break;
            }
            else {
                c = toupper(c);
            }
        }


        if (std::cin.fail())
        {
            std::cout << FOREGROUND(ForegroundColor::BrightRed, "Invalid input, please only enter characters: \n");
            flush_cin();
            continue;
        }
        if (guess.length() != WORD_LENGTH)
        {
            std::cout << FOREGROUND(ForegroundColor::BrightRed, "Guess must consist of 5 characters. Please try again: \n");
            flush_cin();
            continue;
        }

        if (valid_words.count(guess) == 0) {
            std::cout << FOREGROUND(ForegroundColor::BrightRed, "Word not in word-list, try again: \n");
            flush_cin();
            continue;
        }


        break;
	}

    return guess;
}

int parse_guess(const std::string& word, std::map<char, int> word_data, const std::string& guess, std::stringstream& output)
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
            
        }
        else
        {
            output << BACKGROUND(BackgroundColor::BrightBlack, guess_char);
        }
    }

    return correct_guesses;
}