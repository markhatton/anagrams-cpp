/*
 * anagrams.cpp
 */

#include <iostream>

using namespace std;

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

    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-u")
        {
            if (i + 1 >= argc)
            {
                usage("missing filename for option -- " + string(argv[i]));
                return 1;
            }
            unigramsfile = argv[++i];
        }
        else
        {
            input += argv[i];
        }
    }

    cout << input;
}
