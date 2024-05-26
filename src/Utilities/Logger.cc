#include "Logger.h"
#include <iostream>

void DisplayLogo() {
	
    std::cout << R"(	
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

	)" << std::endl;

}

void ProgramUsage() {
		std::cerr << " Program Usage: " << std::endl;
		std::cerr << " ./G4ExSimulator [ -c ConfigFile.json ] " << std::endl;
}