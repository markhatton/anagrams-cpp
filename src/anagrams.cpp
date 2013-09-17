/*
 * anagrams.cpp
 */

#include "../include/dict.h"

#include <iostream>
#include <chrono>
#include <sstream>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <climits>

using namespace std;


void solve(int solutionLimit, unsigned long timeoutAtMillis);

string sortAndFilterChars(const string s);

inline stringstream insertionSort(const string &xs, const string &x);

struct frontier_element {
    long priority;
    const trie_t* t;
    long char_mask;
    const string* acc;

    frontier_element(long _priority, const trie_t* _t, long _char_mask, const string* _acc):
        priority(_priority),
        t(_t),
        char_mask(_char_mask),
        acc(_acc)
        {}
};

struct frontier_element_comparator : binary_function <frontier_element,frontier_element,bool> {
  inline bool operator() (const frontier_element& a, const frontier_element& b) const {return a.priority<b.priority;}
};

string chars;

long complete;

priority_queue<frontier_element, vector<frontier_element>, frontier_element_comparator> frontier;

set<string> visited;


void usage(const string &message = "")
{
    if (!message.empty())
        cerr << message << endl;

    cerr
            << "Usage: anagrams INPUT...\n"
                "Options:\n"
                "  -u file                    unigrams CSV file to use as dictionary, defaults to ./unigrams\n"
                "  -l n                       limit number of solutions to n, defaults to unlimited (0)\n"
                "  -t seconds                 timeout execution afer seconds, defaults to no timeout (0)\n";
}

int main(int argc, char* argv[])
{
    string input = "";
    string unigramsfile = "./unigrams";
    unsigned long solutionsLimit = ULONG_MAX;
    int timeoutSeconds = 0;

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
        else if (arg == "-l")
        {
            if (i + 1 >= argc)
            {
                usage("missing value for option -- " + arg);
                return 1;
            }
            solutionsLimit = atoi(argv[++i]);
        }
        else if (arg == "-t")
        {
            if (i + 1 >= argc)
            {
                usage("missing value for option -- " + arg);
                return 1;
            }
            timeoutSeconds = atoi(argv[++i]);
        }
        else if (!arg.empty()) {
            if (arg.at(0) == '-') {
                usage("unexpected option -- " + arg);
                return 1;
            }
            input += arg; 
        }
    }

    chars = sortAndFilterChars(input);
    complete = LONG_MAX >> (sizeof(complete) * 8 - chars.length() - 1);

    if (chars.empty()) {
        usage();
        return 1;
    }
    else if (chars.length() > 64)
    {
        usage("input longer than 64 characters is not supported");
        return 1;
    }

    loadDictionary(unigramsfile);

    const pair<set<string>::iterator, bool> solution = visited.insert(string());
    frontier.emplace(1 << 26, &dict, 0L, &*solution.first);

    unsigned long timeoutAtMillis = ULONG_MAX;
    if (timeoutSeconds)
    {
        unsigned long now = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
        timeoutAtMillis = now + timeoutSeconds * 1000;
    }

    solve(solutionsLimit, timeoutAtMillis);
}

string sortAndFilterChars(string s)
{
    list<char> cs;
    for ( string::iterator it=s.begin(); it!=s.end(); ++it)
    {
        char c = tolower(*it);
        if (c >= 'a' && c <= 'z')
            cs.push_back(c);
    }

    cs.sort();

    stringstream ss;
    for ( list<char>::iterator it=cs.begin(); it!=cs.end(); ++it)
        ss << *it;

    return ss.str();
}

inline stringstream insertionSort(const string &xs, const string &x)
{
    stringstream sorted;
    stringstream in(xs);
    string word;
    bool inserted = false;
    bool first = true;

    while (getline(in, word, ' '))
    {
        if (!inserted && x < word)
        {
            if (!first) sorted << ' ';
            sorted << x;
            first = false;
            inserted = true;
        }
        if (!first) sorted << ' ';
        sorted << word;
        first = false;
    }
    if (!inserted) {
        if (!first) sorted << ' ';
        sorted << x;
    }

    return sorted;
}

void solve(int solutionLimit, unsigned long timeoutAtMillis)
{
    unsigned long solutions = 0;

    while (true) {

        if (frontier.empty() || chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1) > timeoutAtMillis)
            break;

        const frontier_element f = frontier.top();
        frontier.pop();
    
        char lastc = '\0';
        for (unsigned i=0; i < chars.length(); ++i)
        {
            char c = chars.at(i);
            if (f.char_mask & (1 << i) || c == lastc)
                continue;
            lastc = c;

            const trie_t* t = f.t->getChild(c);
    
            if (t)
            {
                long char_mask(f.char_mask);
                char_mask |= (1 << i);
    
                bool eof = (char_mask & complete) == complete;
                if (!eof)
                    frontier.emplace(f.priority, t, char_mask, f.acc);

                pair<const string, long>* kv = t->getValue();
                if (kv)
                {
                    // we have found a dictionary word (not just a prefix)

                    const pair<set<string>::iterator, bool> solution =
                        visited.emplace(insertionSort(*f.acc, kv->first).str());

                    if (solution.second)
                    {
                        if (eof)
                        {
                            cout << *solution.first << endl;
                            solutions++;
                            if (solutions >= solutionLimit)
                                return;
                        } else {
                            long priority = -(solution.first->size() << 26) + kv->second;
                            if (f.priority < priority) priority = f.priority;
                            frontier.emplace(priority, &dict, char_mask, &*solution.first);
                        }
                    }
                }
            }
    
        }
    }
}
