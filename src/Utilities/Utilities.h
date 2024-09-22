#ifndef Utilities_h
#define Utilities_h 1

#include <iostream>

using namespace std;

class Utilities
{
    public:
        Utilities() { }
        ~Utilities() { }

        static string ConcatenatePaths(const string &aPath1, const string aPath2);
        static string ResolveRelativePath(const string &aBasePath, const string &aRelPath);
        static void SplitPath(const string &aFullPath, string &aDirectory, string &aFileName);
        static string ReplacePlaceHolder(const string &aJsonString, const string &aPlaceHolder, const string &aReplacement);
        static bool IsMultiThread();
};

#endif