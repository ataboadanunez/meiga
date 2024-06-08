#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>

enum LogType {
    INFO,
    WARNING,
    DEBUG,
    ERROR
};

class Logger {

public:
    static void DisplayLogo();
    static void ProgramUsage(std::string aName);
    static void CheckCommandLineParameters(int argc, char** argv, std::string &aCfgFile, std::string aContext = "");
    static void Print(const std::ostringstream& aMessage, LogType aType, std::string aContext = "");
    static void Print(const std::string& aMessage, LogType aType, std::string aContext = "");
    static void Print(const char *aMessage, LogType aType, std::string aContext = "");

private:
    static std::string LogTypeToString(LogType aType);

};

class Timer {
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}

    inline void SetContext(std::string aContext) { fContext = aContext; }
    void PrintElapsedTime() const {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        if(fContext != "") {
            std::cout << fContext+": Elapsed time: " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds" << std::endl;
        } else {
            std::cout << "Elapsed time: " << std::fixed << std::setprecision(2) << elapsed.count() << " seconds" << std::endl;
        }
        
    }

private:
    std::chrono::high_resolution_clock::time_point start;
    std::string fContext = "";
};



#endif // LOGGER_H