#include "variables.h"

// Define the global instance
// GlobalVariables* globals;
std::shared_ptr<GlobalVariables> globals = std::make_shared<GlobalVariables>();
