/*
 * dict.h
 */

#ifndef DICT_H_
#define DICT_H_

#include "trie_map.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>

using namespace std;

typedef trie_map<string, long, 32> trie_t;

trie_t dict;

string short_words_arr[] = {
    "a",
    "i",
    "ab",
    "ad",
    "ah",
    "am",
    "an",
    "as",
    "at",
    "be",
    "bi",
    "bo",
    "by",
    "co",
    "da",
    "de",
    "do",
    "ed",
    "eh",
    "el",
    "em",
    "er",
    "ex",
    "go",
    "ha",
    "he",
    "hi",
    "ho",
    "id",
    "if",
    "in",
    "is",
    "it",
    "kg",
    "km",
    "la",
    "lb",
    "lo",
    "ma",
    "me",
    "ml",
    "mm",
    "ms",
    "my",
    "na",
    "no",
    "of",
    "oh",
    "oi",
    "ol",
    "on",
    "op",
    "or",
    "ox",
    "pa",
    "pi",
    "pm",
    "po",
    "pi",
    "pm",
    "po",
    "re",
    "so",
    "st",
    "ta",
    "to",
    "uh",
    "um",
    "un",
    "up",
    "us",
    "vs",
    "we",
    "ya",
    "ye",
    "yo"
};
set<string> short_words(short_words_arr, short_words_arr + 76);

void loadDictionary(const string &unigramsFile)
{
    string line;
    string word;
    string freq;
    ifstream infile;
    infile.open(unigramsFile.c_str(), ifstream::in);

    while (!infile.eof())
    {
        getline(infile, line);

        stringstream ss(line);
        getline(ss, word, '\t');
        if (!getline(ss, freq, '\t'))
            freq = "1";

        if (word.length() > 0 && (word.size() > 2 || short_words.count(word) > 0)) {
            // cout << "adding " << word << " to dict, with len " << word.length() << endl;
            dict[word] = atol(freq.c_str());
        }
    }

    infile.close();
}


#endif /* DICT_H_ */
