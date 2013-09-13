/*
 * dict.h
 */

#ifndef DICT_H_
#define DICT_H_


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>

using namespace std;

set<string> dict;

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

        dict.insert(word);
    }

    infile.close();
}


#endif /* DICT_H_ */
