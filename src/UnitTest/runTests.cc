#include <iostream>
#include "Ut_ConfigManager.h"


int main(int argc, char** argv)
{
    Ut_ConfigManager::test_ReadConfigurationFile();
    Ut_ConfigManager::test_ReadDetectorList();

    return 0;
}