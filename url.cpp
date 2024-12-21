#include <url.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include <sstream>

namespace
{
}

namespace browser
{

URL::URL(std::string_view s)
{
    if (auto p = s.find("/"); p == s.npos)
    {
        host = s;
        path = "/";
    }
    else
    {
        host = s.substr(0, p);
        path = s.substr(p + 1);
    }
}

std::string URL::request()
{
    int s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        return "";
    }
    struct addrinfo *res0;
    if (getaddrinfo(host.c_str(), "80", nullptr, &res0) == 0)
    {
        for (auto res = res0; res; res = res->ai_next)
        {
            if (connect(s, res->ai_addr, res->ai_addrlen) == 0)
            {
                break;
            }
        }
    }
    freeaddrinfo(res0);

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.0\r\n";
    request << "Host: " << host << "\r\n";
    request << "\r\n";

    auto r = std::move(request.str());
    send(s, r.c_str(), r.length(), 0);

    char buffer[1024];
    std::stringstream response;
    while (ssize_t n_bytes = read(s, buffer, sizeof(buffer)))
    {
        if (n_bytes > 0)
        {
            response << std::string(buffer, n_bytes);
        }
    }

    return std::move(response.str());
}

}
