#include <url.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>

#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>

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
    std::ostringstream response;
    while (ssize_t n_bytes = read(s, buffer, sizeof(buffer)))
    {
        if (n_bytes > 0)
        {
            response << std::string(buffer, n_bytes);
        }
    }
    close(s);

    r = std::move(response.str());
    size_t start = 0;
    std::unordered_map<std::string, std::string> response_headers;
    for (auto p = r.find("\r\n", start);
              p != r.npos;
              start = p + 2, p = r.find("\r\n", start))
    {
        if (p == start)
        {
            // end of headers?
            start = p + 2;
            break;
        }
        std::string_view header{r.data() + start, p - start};
        auto colon = header.find(":");
        std::string key{header, 0, colon};
        std::string val{header, colon + 1, header.length() - colon - 1};

        std::transform(key.begin(), key.end(), key.begin(),
        [](auto c) {return std::tolower(c);});

        val.erase(val.begin(), std::find_if(val.begin(), val.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        val.erase(std::find_if(val.rbegin(), val.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), val.end());

        response_headers[key] = val;
    }

    std::string_view body{r.data() + start};
    return body.data();
}

}
