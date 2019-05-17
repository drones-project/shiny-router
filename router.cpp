#include "http/server_http.hpp"
#include "pathfinder/RayPath.hpp"
#include "utils/logging.hpp"

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
#include <iostream>
// For logging
#include <chrono>
#include <ctime>

using namespace std;
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

int main(int argc, char *argv[]) {
    // Initialize the server
    HttpServer server;
    server.config.port = 5000;

    // Initialize pathfinder
    string filename = "data/buildings.json";
    RayPath pathfinder = RayPath(filename);

    // POST response for /route endpoint
    server.resource["^/route$"]["POST"] = [&pathfinder](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        logging::log("POST /route");
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
        logging::log("GET /route");
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

    logging::log("Server started...");

    server_thread.join();
}
