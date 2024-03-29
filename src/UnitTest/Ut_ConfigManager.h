#ifndef Ut_ConfigManager
#define Ut_ConfigManager_h 1
#include <string>

using namespace std;

class Ut_ConfigManager
{

    public:
        Ut_ConfigManager() { }
        ~Ut_ConfigManager() { }

        static bool test_ReadConfigurationFile();
        static bool test_ReadDetectorList();
};

#endif