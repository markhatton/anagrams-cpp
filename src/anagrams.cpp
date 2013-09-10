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
void solve(const string w, const string remain, const string acc);

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

    solve("", input, "");
}

set<string> partials;

string buildPartial(string s)
{
    list<string> sorted;
    istringstream iss(s);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter<list<string> >(sorted));
    sorted.sort();

    stringstream ss;
    for (list<string>::iterator it=sorted.begin(); it!=sorted.end(); ++it)
        ss << *it << " ";
    return ss.str();
}

void solve(const string w, const string remain, const string acc)
{
    for (unsigned i=0; i < remain.length(); ++i)
    {
        char c = remain.at(i);
        string w_ = w + c;
        string remain_ = remain.substr(0, i) + remain.substr(i + 1, remain.length() - i - 1);

        if (dict.count(w_) > 0) {
            string p = buildPartial(acc + w_);
            if (partials.count(p) == 0)
            {
                partials.insert(p);
                if (remain_.empty())
                    cout << acc + w_ << endl;
                else
                    solve("", remain_, p);
            }
        }

        solve(w_, remain_, acc);
    }
}
