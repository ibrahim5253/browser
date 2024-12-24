#include <url.h>

#include <iostream>

void show(auto&& body)
{
    std::cout << body;
}

int main(int argc, char** argv)
{
    using namespace browser;
    URL url("example.org");
    show(url.request());
}
