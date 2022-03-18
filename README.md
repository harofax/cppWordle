# cppWordle -- C++ assignment
console wordle made with cpp

## Data structure for words
I decided to forgo memory in favour of speed, so I have two structures for the words. One is an **unordered_set** for checking if guesses are valid words,
the other is a **vector\<string>** for grabbing a random word when you choose "play again", so we don't have to open a file every time we want to get a new word.

For the hidden word itself, I decided to create a map, with info about how many of each characters there are in the hidden word. When the player guesses, I do two sweeps,
once to check correct characters in correct place, and then another for making correct characters in wrong place yellow etc. 


