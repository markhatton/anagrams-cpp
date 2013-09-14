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
    ifstream infile;
    infile.open(unigramsFile.c_str(), ifstream::in);

    while (!infile.eof())
    {
        getline(infile, line);

        stringstream ss(line);
        getline(ss, word, '\t');

        if (word.length() > 0) {
            // cout << "adding " << word << " to dict, with len " << word.length() << endl;
            dict[word] = 1;
        }
    }

    infile.close();
}


#endif /* DICT_H_ */
