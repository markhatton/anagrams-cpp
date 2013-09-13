/*
 * anagrams.cpp
 */

#include "../include/dict.h"

#include <iostream>
#include <sstream>
#include <set>
#include <list>

using namespace std;


inline void solve(const string w, const string remain, const list<string> acc);

string sortChars(const string s);

inline list<string> insertionSort(const list<string> xs, const string x);

inline string makeString(list<string> xs);

set<string> partials;


void usage(string message = "")
{
    if (!message.empty())
        cout << message << endl;

    cout
            << "Usage: anagrams INPUT...\n"
                "Options:\n"
                "  -u file                    unigrams CSV file to use as dictionary (defaults to ./unigrams)\n";
}

int main(int argc, char* argv[])
{
    string input = "";
    string unigramsfile = "./unigrams";

    bool no_more_opts = false;

    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (no_more_opts) {
            input += arg;
        }
        else if (arg == "--") {
            no_more_opts = true;
        }
        else if (arg == "-u")
        {
            if (i + 1 >= argc)
            {
                usage("missing filename for option -- " + arg);
                return 1;
            }
            unigramsfile = argv[++i];
        }
        else if (!arg.empty()) {
            if (*arg.begin() == '-') {
                usage("unexpected option -- " + arg);
                return 1;
            }
            input += arg; 
        }
    }

    if (input.empty()) {
        usage();
        return 1;
    }

    loadDictionary(unigramsfile);

    solve("", sortChars(input), list<string>());
}

string sortChars(string s)
{
    list<char> cs;
    for ( string::iterator it=s.begin(); it!=s.end(); ++it)
        cs.push_back(*it);

    cs.sort();

    stringstream ss;
    for ( list<char>::iterator it=cs.begin(); it!=cs.end(); ++it)
        ss << *it;

    return ss.str();
}

inline list<string> insertionSort(const list<string> xs, const string x)
{
    list<string> sorted = xs;
    for (list<string>::iterator it=sorted.begin(); it!=sorted.end(); ++it)
    {
        if (x < *it)
        {
            sorted.insert(it, x);
            break;
        }
    }
    if (xs.size() == sorted.size())
        sorted.push_back(x);
    return sorted;
}

inline string makeString(list<string> xs)
{
    stringstream ss;
    for (list<string>::iterator it=xs.begin(); it!=xs.end(); ++it)
        if (it == xs.begin())
            ss << *it;
        else
            ss << ' ' << *it;
    return ss.str();
}

inline void solve(const string w, const string remain, const list<string> acc)
{
    char lastc = '\0';
    for (unsigned i=0; i < remain.length(); ++i)
    {
        char c = remain.at(i);
        if (c == lastc)
            continue;
        lastc = c;

        string w_ = w + c;
        string remain_ = remain.substr(0, i) + remain.substr(i + 1, remain.length() - i - 1);

        TRIE_T::iterator it = dict.find(w_);

        if (it != dict.end())
        {
            if (it.hasValue()) {
                // we have found a whole word (not just a prefix)
                list<string> partial = insertionSort(acc, w_);
                string p = makeString(partial);
                if (partials.count(p) == 0)
                {
                    partials.insert(p);
                    if (remain_.empty())
                        cout << p << endl;
                    else
                        solve("", remain_, partial);
                }
            }

            solve(w_, remain_, acc);
        }

    }
}
