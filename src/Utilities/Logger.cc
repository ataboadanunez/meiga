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
                                                        
	)" << endl;
	cout << "Version: " << MEIGA_VERSION << " (2024)" << endl; 
	cout << "Author: Alvaro Taboada Nunez" << endl;
	cout << "GitHub: https://github.com/ataboadanunez/meiga" << endl;
	cout << "\n";
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

void Logger::PrintVector(const std::vector<std::string> &aVector, const std::string &aMessage, LogType aType, std::string aContext)
{
	if(aContext != "") {
		aContext = aContext + ": ";
	}
	ostringstream outputStream;
	outputStream << "[" << LogTypeToString(aType) << "] " + aContext << aMessage << std::endl;
	for (const auto& prop : aVector) {
		outputStream << "  " << prop << std::endl;
	}
	cout << outputStream.str();
}

void Logger::PrintVector(const vector<string> &aVector, const ostringstream &aMessage, LogType aType, string aContext)
{
	PrintVector(aVector, aMessage.str(), aType, aContext);
}

void Logger::PrintVector(const std::vector<std::string> &aVector, const char *aMessage, LogType aType, std::string aContext)
{
	PrintVector(aVector, string(aMessage), aType, aContext);
}

void Logger::PrintVector(const std::vector<double> &aVector, const char *aMessage, LogType aType, std::string aContext)
{
	vector<string> strVector;
	strVector.reserve(aVector.size());
	for(const double& val : aVector) {
		strVector.push_back(std::to_string(val));
	}
	PrintVector(strVector, aMessage, aType, aContext);
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