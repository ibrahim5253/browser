#pragma once

#include <string_view>
#include <string>

namespace browser
{

class URL
{
public:
    URL(std::string_view s);
    std::string request();

private:
    std::string host;
    std::string path;
};

}
