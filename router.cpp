#include "http/server_http.hpp"
#include "pathfinder/RayPath.hpp"
#include "utils/logging.hpp"
#include "utils/cxxopts.hpp"

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Added for the default_resource example
#include <algorithm>
#include <boost/filesystem.hpp>
#include <iostream>
#include <string>
#include <vector>
// For logging
#include <chrono>
#include <ctime>

using namespace std;
using namespace boost::property_tree;

using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;

int main(int argc, char *argv[]) {
    logging::log("Starting routing server...");

    // Parsing arguments
    cxxopts::Options options("ShinyRouter", "Raycasting pathfinder for DroNeS");
    options.add_options()
    ("d,debug", "Enable debugging")
    ("b,buildings", "Buildings file", cxxopts::value<string>());
    auto result = options.parse(argc, argv);

    bool debugging = result["debug"].as<bool>();
    if (debugging) logging::log("Debugging is on");

    // Initialize the server
    HttpServer server;
    server.config.port = 5000;

    // Initialize pathfinder
    string filename = "data/buildings.json";
    try {
        filename = result["buildings"].as<string>();
        logging::log("Using " + filename + " as buildings file");
    }
    catch (domain_error &e) {
        logging::log("No buildings file provided, defaulting to " + filename);
    }
    RayPath pathfinder = RayPath(filename);

    // POST response for /route endpoint
    server.resource["^/route$"]["POST"] = [&pathfinder, &debugging](shared_ptr<HttpServer::Response> response, shared_ptr<HttpServer::Request> request) {
        logging::log("POST /route");
        //get the string response
        string data = request->content.string();
        if (debugging) {
            ptree pt;
            istringstream is(data);
            read_json(is, pt);
            // create message string
            string message = "ORIGIN: ";
            for (auto i: pt.get_child("origin"))
                message += i.second.data() + "   ";
            message += "DESTINATION: ";
            for (auto i: pt.get_child("destination"))
                message += i.second.data() + "   ";
            logging::log(message);
        }
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
    logging::log("Server running...");
    server_thread.join();
}
