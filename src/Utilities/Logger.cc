#include "Logger.h"
#include <iostream>

using namespace std;

void DisplayLogo() {
	
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

void ProgramUsage(string aName) {
	cerr << " [INFO] Example of program usage: " << endl;
	cerr << " ./"+aName+" -c "+aName+".json" << endl;
}