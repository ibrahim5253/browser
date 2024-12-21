#include <url.h>

#include <iostream>

int main(int argc, char** argv)
{
    using namespace browser;
    URL url("example.org");
    auto response = url.request();
    std::cout << response;
}
