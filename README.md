anagrams-cpp
============

Anagram solver command-line utility implemented in C++

### Features

- Prioritisation:-
  - Uses an input file containing term frequencies to prioritise processing of solutions
  - Alternatively, works just fine with a regular word list, e.g. `/usr/share/dict/words`
- Performance:-
  - A trie structure holds the dictionary for efficient traversals
  - A 64-bit bitmask is used to store the state of each frontier element
  - Keeps a list of visited tree nodes such that the separate subtrees of e.g.: "for now" and "now for" will only be searched once
  - Avoids unnecessary processing when input contains mutliple instances of the same letter
  - A priority queue is used to prioritise the frontier, by default solutions with fewer separate words are prioritised
  - Uses C++11 techniques to avoid memory copy operations

### Usage

    ./anagrams Hello World

Will output all anagrams of the string "Hello World" using `/usr/share/dict/words` as a dictionary.

    ./anagrams -u unigrams.csv Hello World -t 10 -l 100

Will output the first 100 anagrams of the input string "Hello World" and will terminate the search after 10 seconds if 100 solutions are not reached by this time.  Will use the file `unigrams.csv` to prioritise solutions that contain commonly used words, see below.

### Prioritised anagram solving via unigrams dataset

This project isn't designed to simply return an exhaustive set of anagrams, but instead to find the "best" anagrams.

To this aim, `anagrams` may take as input a tab-separated CSV file containing word frequency counts.  See my alternate project for generating this file from the Google n-grams corpus: https://github.com/markhatton/google-ngrams

When given such a file as input, partial solutions containing common words will be solved first, leaving partial solutions with less frequently used words to be solved later (or never solved at all).

For example the first 3 solutions of the input "Mark Hatton" using unproritised searching are:
    am thank tor
    arm thank to
    am rot thank

But given unigram frequencies as input, the first 3 solutions are:
    mark on that
    mark no that
    mark than to

As you can see the latter solutions contain more commonly used terms.
