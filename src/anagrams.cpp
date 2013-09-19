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

inline list<string> insertionSort(const list<string> &xs, const string &x);

inline string makeString(const list<string> &xs);

struct frontier_element {
    long priority;
    const trie_t* t;
    uint64_t char_mask;
    const list<string>* acc;

    frontier_element(long _priority, const trie_t* _t, uint64_t _char_mask, const list<string>* _acc):
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

uint64_t complete;

priority_queue<frontier_element, vector<frontier_element>, frontier_element_comparator> frontier;

set<list<string> > visited;


void usage(const string &message = "")
{
    if (!message.empty())
        cerr << message << endl;

    cout
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
    uint64_t solutionsLimit = ULONG_MAX;
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
    complete = UINT64_MAX >> (64 - chars.length());

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

    const pair<set<list<string> >::iterator, bool>& solution = visited.insert(list<string>());
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

inline list<string> insertionSort(const list<string> &xs, const string &x)
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

inline string makeString(const list<string> &xs)
{
    stringstream ss;
    for (list<string>::const_iterator it=xs.cbegin(); it!=xs.cend(); ++it)
        if (it == xs.cbegin())
            ss << *it;
        else
            ss << ' ' << *it;
    return ss.str();
}

void solve(int solutionLimit, unsigned long timeoutAtMillis)
{
    unsigned long solutions = 0;
    unsigned long iterations = 0;

    while (true) {

	iterations++;

        if (frontier.empty() || (iterations % 8192 == 0 && chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1) > timeoutAtMillis))
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
                uint64_t char_mask(f.char_mask);
                char_mask |= (1 << i);
    
                pair<const string, long>* kv = t->getValue();
                bool eof = (char_mask & complete) == complete;

                if (kv)
                {
                    // we have found a dictionary word (not just a prefix)

                    const pair<set<list<string> >::iterator, bool>& solution =
                        visited.insert(insertionSort(*f.acc, kv->first));

                    if (solution.second)
                    {
                        if (eof)
                        {
                            const string p = makeString(*solution.first);
                            cout << p << endl;
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
    
                if (!eof)
                    frontier.emplace(f.priority, t, char_mask, f.acc);
            }
    
        }
    }
}
