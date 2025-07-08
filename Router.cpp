#include "Router.hpp"


// DO: Match a server block based on host and port
// RETURN: the first server block that matches the port, or the first server block matches the host if no port match is found
const ServerConfig& matchServer(const Config& config, const std::string& host, int port) {
    const ServerConfig* fallback = NULL;

    for (size_t i = 0; i < config.servers.size(); ++i)
    {
        const ServerConfig& server = config.servers[i];

        // Check all listen blocks for port match
        for (size_t j = 0; j < server.listens.size(); ++j)
        {
            if (server.listens[j].listen_port == port)
            {
                // Save first match as fallback
                if (!fallback)
                    fallback = &server;

                // Now check server_name match
                for (size_t k = 0; k < server.server_name.size(); ++k)
                {
                    if (server.server_name[k] == host)
                        return server; // Exact match
                }
            }
        }
    }

    if (fallback)
        return *fallback;

    throw std::runtime_error("No server block found for that port");
}


// DO: This function matches the longest location path for a given URI in a server block.
// RETURN: the location block that matches the URI
const LocationConfig& matchLocation(const ServerConfig& server, const std::string& uri) {
    
    const LocationConfig *match = NULL;
    size_t longest = 0;

    for (size_t i = 0; i < server.locations.size(); ++i)
    {
        const LocationConfig& loc = server.locations[i];
        const std::string& path = loc.path;

        if (uri.compare(0, path.size(), path) == 0) //path.size() is the second argument, and its for str1 not str2 as well as first argument
        {
            // the root location always matches everything -> for the path
            // & checking for full match -> for both
            // Check valid path boundary (e.g. /images should not match /imageshack) if path is a part of uri we have to check if it end with /
                // with a slash else it's a full match
            /*
                there is three cases:
                1. path is exactly the same as uri (e.g. /images == /images)
                2. path is a prefix of uri and uri continues with a slash (e.g. /images == /images/...)
                    path is a prefix of uri and uri continues with another character or less character (e.g. /images == /imageshack or /image)
                3. or the path is a root location (e.g. / == /images)
            */
            if (path == "/" || uri.size() == path.size() || uri[path.size()] == '/')
            {
                if (path.size() > longest)
                {
                    match = &loc;
                    longest = path.size();
                }
            }
        }
    }

    if (!match)
        throw std::runtime_error("No matching location for URI: " + uri);

    return *match;
}


// DO: This function gives you the physical file path on disk based on the config and URI.
// RETURN: root + (uri - location.path)
std::string finalPath(const LocationConfig& location, const std::string& uri) {
    const std::string& root = location.root;
    const std::string& locPath = location.path;

    // Step 1: remove the location path from the URI
        // substr(index to start from, length of the substring)
    std::string remain = uri.substr(locPath.length());

    // Step 2: avoid double slashes
    if (root[root.size() - 1] == '/' && !remain.empty() && remain[0] == '/')
        remain = remain.substr(1);

    // Step 3: combine root + remain
    return root + remain;
}

// DO: This function routes a request based on the configuration, host, port, and URI.
// RETURN: a RoutingResult containing the matched server, location, file path, and redirection
RoutingResult requestedRout(const Config& config, const std::string& host,
                        int port, const std::string& uri)
{
    const ServerConfig& server = matchServer(config, host, port);
    const LocationConfig& location = matchLocation(server, uri);

    RoutingResult result;
    result.server = &server;
    result.location = &location;

    if (!location.redirection.empty())
    {
        result.is_redirect = true;
        result.redirect_url = location.redirection;
    }
    else
    {
        result.is_redirect = false;
        result.file_path = finalPath(location, uri);
    }

    return result;
}
