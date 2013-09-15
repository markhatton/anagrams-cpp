/*
 * anagrams.cpp
 */

#include "../include/dict.h"

#include <iostream>
#include <sstream>
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <memory>

using namespace std;


inline void solve();

string sortAndFilterChars(const string s);

inline list<string> insertionSort(const list<string> &xs, const string &x);

inline string makeString(const list<string> &xs);

struct frontier_element {
    long priority;
    const trie_t* t;
    long char_mask;
    const list<string>* acc;

    frontier_element(long _priority, const trie_t* _t, long _char_mask, const list<string>* _acc):
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

set<list<string> > visited;


void usage(const string &message = "")
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

    const pair<set<list<string> >::iterator, bool>& solution = visited.insert(list<string>());
    frontier.emplace(1 << 26, &dict, 0L, &*solution.first);

    solve();
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

inline void solve()
{
    while (true) {

        if (frontier.empty())
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
    
                pair<const string, long>* kv = t->getValue();
                if (kv) {

                    // we have found a whole word (not just a prefix)

                    const pair<set<list<string> >::iterator, bool>& solution =
                        visited.emplace(insertionSort(*f.acc, kv->first));

                    if (solution.second)
                    {
                        if ((char_mask & complete) == complete) {
                            const string p = makeString(*solution.first);
                            cout << p << endl;
                        } else {
                            long priority = -(solution.first->size() << 26) + kv->second;
                            if (f.priority < priority) priority = f.priority;
                            frontier.emplace(priority, &dict, char_mask, &*solution.first);
                        }
                    }
                }
    
                frontier.emplace(f.priority, t, char_mask, f.acc);
            }
    
        }
    }
}
