#include "http/server_http.hpp"

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

/* STUB PATHFINDER */
class StubPathfinder {
public:
    string getRoute(string);
};

string StubPathfinder::getRoute(string data) {
    return "{\"waypoints\": {\"x\": 0, \"y\": 0, \"z\": 0}}";
}

// replace this line with actual pathfinder
StubPathfinder pathfinder = StubPathfinder();

int main() {
    // initialize the server
    HttpServer server;
    server.config.port = 8080;

    // POST response for /route endpoint
    server.resource["^/route$"]["POST"] = [](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        string data = request->content.string();
        // create response header
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "application/json");
        // get route and send as response
        string route = pathfinder.getRoute(data);
        response->write(route, header);
    };

    // GET method for /route endpoint
    server.resource["^/route$"]["GET"] = [](shared_ptr<HttpServer::Response>response, shared_ptr<HttpServer::Request> request) {
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "application/json");
        response->write("{\"success\": true}", header);
    };

    // 404 on any other endpoint
    server.default_resource["GET"] = [](shared_ptr<HttpServer::Response>response, shared_ptr<HttpServer::Request>request) {
        response->write(SimpleWeb::StatusCode::client_error_not_found, "Content not found");
    };

    // start the server and run forever
    thread server_thread([&server]() {
        server.start();
    });
    
    server_thread.join();
}
