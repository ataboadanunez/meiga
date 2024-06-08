#include "Logger.h"

using namespace std;



void Logger::DisplayLogo() {
	
  	cout << R"(	
  _______ _            __  __      _                    
 |__   __| |          |  \/  |    (_)                   
    | |  | |__   ___  | \  / | ___ _  __ _  __ _        
    | |  | '_ \ / _ \ | |\/| |/ _ \ |/ _` |/ _` |       
    | |  | | | |  __/ | |  | |  __/ | (_| | (_| |       
  __|_|_ |_| |_|\___| |_|  |_|\___|_|\__, |\__,_|  _    
 |  ____|                             __/ |       | |   
 | |__ _ __ __ _ _ __ ___   _____    |___/__  _ __| | __
 |  __| '__/ _` | '_ ` _ \ / _ \ \ /\ / / _ \| '__| |/ /
 | |  | | | (_| | | | | | |  __/\ V  V / (_) | |  |   < 
 |_|  |_|  \__,_|_| |_| |_|\___| \_/\_/ \___/|_|  |_|\_\
                                                        
	© Alvaro Taboada Nunez - 2023
	https://github.com/ataboadanunez/meiga

	)" << endl;

	}

void Logger::ProgramUsage(string aName) {
	ostringstream msg;
	msg << "Example of program usage: ./"+aName+" -c "+aName+".json";
	Print(msg, LogType::INFO);
}

void Logger::CheckCommandLineParameters(int argc, char** argv, string &aCfgFile, string aContext)
{
	if(argc != 3) {
		throw invalid_argument("Invalid parameters.");
	}
	if(string(argv[1]) != "-c") {
		throw invalid_argument("Invalid command line parameter: " + string(argv[1]));
	}
	aCfgFile = argv[2];
}

void Logger::Print(const string &aMessage, LogType aType, string aContext)
{
	string typeStr = LogTypeToString(aType);
	if(aContext != "") {
		aContext = aContext + ": ";
	}
	if(aType != LogType::ERROR) {
		cout << "[" << typeStr << "] "+aContext << aMessage << endl;
	}
	else {
		cerr << "[" << typeStr << "] "+aContext << aMessage << endl;
	}
}

void Logger::Print(const char *aMessage, LogType aType, std::string aContext)
{
	Print(string(aMessage), aType, aContext);
}

void Logger::Print(const ostringstream &aMessage, LogType aType, std::string aContext)
{
	Print(aMessage.str(), aType, aContext);
}

string Logger::LogTypeToString(LogType aType)
{
	switch (aType) {
		case LogType::INFO: 
			return "INFO";
		case LogType::DEBUG:
			return "DEBUG";
		case LogType::WARNING:
			return "WARNING";
		case LogType::ERROR:
			return "ERROR";
		default:
			return " ";
	}
}