#pragma once

#include <string>
#include <map>
#include <functional>

namespace DSAA {

    std::map<std::string, std::function<void()>> tasks;
}