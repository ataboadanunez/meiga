#ifndef CONFIGFILEPATH_H
#define CONFIGFILEPATH_H
#include <string>

#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH ""
#endif

inline const std::string& GetConfigFilePath() {
    static const std::string path = CONFIG_FILE_PATH;
    return path;
}

#endif // CONFIGFILEPATH_H
