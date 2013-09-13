/*
 * anagrams.cpp
 */

#include <iostream>
#include <sstream>
#include <set>
#include <list>
#include <algorithm>
#include <iterator>

using namespace std;


set<string> dict;

void solve(const string w, const string remain, const list<string> acc);

inline list<string> insertionSort(const list<string> xs, const string x);

inline string makeString(list<string> xs);


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

    dict.insert("a");
    dict.insert("b");
    dict.insert("c");
    dict.insert("ab");
    dict.insert("bc");

    solve("", input, list<string>());
}

set<string> partials;

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

void solve(const string w, const string remain, const list<string> acc)
{
    for (unsigned i=0; i < remain.length(); ++i)
    {
        char c = remain.at(i);
        string w_ = w + c;
        string remain_ = remain.substr(0, i) + remain.substr(i + 1, remain.length() - i - 1);

        if (dict.count(w_) > 0)
        {
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
