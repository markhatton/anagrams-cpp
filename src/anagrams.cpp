/*
 * anagrams.cpp
 */

#include "../include/dict.h"

#include <iostream>
#include <sstream>
#include <set>
#include <list>
#include <queue>

using namespace std;


inline void solve();

string sortAndFilterChars(const string s);

inline list<string> insertionSort(const list<string> &xs, const string &x);

inline string makeString(list<string> &xs);

struct frontier_element {
    long priority;
    trie_t* t;
    const string* remain;
    const list<string>* acc;

    inline frontier_element(long _priority, trie_t* _t, const string* _remain, const list<string>* _acc):
        priority(_priority),
        t(_t),
        remain(_remain),
        acc(_acc){}
};

struct frontier_element_comparator : binary_function <frontier_element,frontier_element,bool> {
  inline bool operator() (const frontier_element& a, const frontier_element& b) const {return a.priority<b.priority;}
};

priority_queue<frontier_element, vector<frontier_element>, frontier_element_comparator> frontier;

set<list<string> > partials;

set<string> remains;


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

    if (input.empty()) {
        usage();
        return 1;
    }

    loadDictionary(unigramsfile);

    string sortedInput = sortAndFilterChars(input);
    pair<set<list<string> >::iterator, bool> partial = partials.insert(list<string>());
    pair<set<string>::iterator, bool> remain = remains.insert(sortedInput);
    frontier.emplace(0L, &dict, &*remain.first, &*partial.first);

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

inline string makeString(list<string> &xs)
{
    stringstream ss;
    for (list<string>::iterator it=xs.begin(); it!=xs.end(); ++it)
        if (it == xs.begin())
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

        frontier_element f = frontier.top();
        frontier.pop();
        string remain = *(f.remain);
    
        char lastc = '\0';
        for (unsigned i=0; i < remain.length(); ++i)
        {
            char c = remain.at(i);
            if (c == lastc)
                continue;
            lastc = c;
    
            trie_t* t_ = f.t->getChild(c);
    
            if (t_)
            {
                pair<set<string>::iterator, bool> remain_ =
                    remains.emplace(remain.substr(0, i) + remain.substr(i + 1, remain.length() - i - 1));
    
                pair<const string, long>* kv = t_->getValue();
                if (kv) {
                    // we have found a whole word (not just a prefix)

                    pair<set<list<string> >::iterator, bool> partial =
                        partials.emplace(insertionSort(*(f.acc), kv->first));

                    if (partial.second)
                    {
                        if ((*remain_.first).empty()) {
                            list<string> solution = *partial.first;
                            string p = makeString(solution);
                            cout << p << endl;
                        } else {
                            long priority = -((*partial.first).size() << 26) + kv->second;
                            if (f.priority < priority) priority = f.priority;
                            frontier.emplace(priority, &dict, &*remain_.first, &*partial.first);
                        }
                    }
                }
    
                frontier.emplace(f.priority, t_, &*remain_.first, f.acc);
            }
    
        }
    }
}
