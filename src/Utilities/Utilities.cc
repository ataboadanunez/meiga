#include "Utilities.h"
#include <iostream>
#include <string>
#include <boost/filesystem.hpp>

string 
Utilities::ConcatenatePaths(const string &aPath1, const string aPath2)
{
    bool endsWithSeparator = (!aPath1.empty() && aPath1.back() == '/');
    bool startsWithSeparator = (!aPath2.empty() && aPath2.front() == '/');
    if (endsWithSeparator && startsWithSeparator) {
        return aPath1 + aPath2.substr(1);
    }
    else if (!endsWithSeparator && !startsWithSeparator) {
        return aPath1 + "/" + aPath2;
    }
    else {
        return aPath1 + aPath2;
    }
}

string 
Utilities::ResolveRelativePath(const string &aBasePath, const string &aRelPath)
{
    boost::filesystem::path baseDir(aBasePath);
    boost::filesystem::path relPath(aRelPath);
    boost::filesystem::path absolutePath = baseDir / relPath;

    // Return the absolute path as a string
    return absolutePath.string();
}

void Utilities::SplitPath(const string &aFullPath, string &aDirectory, string &aFileName)
{
    // Create a boost::filesystem::path object from the full path
    boost::filesystem::path fullPathObj(aFullPath);

    // Get the directory path component
    aDirectory = fullPathObj.parent_path().string();

    // Get the filename component
    aFileName = fullPathObj.filename().string();
}

string 
Utilities::ReplacePlaceHolder(const string &aJsonString, const string &aPlaceHolder, 
                            const string &aReplacement)
{
    std::string result = aJsonString;
    size_t pos = result.find(aPlaceHolder);
    while (pos != std::string::npos) {
        result.replace(pos, aPlaceHolder.length(), aReplacement);
        pos = result.find(aPlaceHolder, pos + aReplacement.length());
    }
    return result;
    
}
