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

        if (word.length() > 0) {
            // cout << "adding " << word << " to dict, with freq. " << freq << endl;
            dict[word] = atol(freq.c_str());
        }
    }

    infile.close();
}


#endif /* DICT_H_ */
