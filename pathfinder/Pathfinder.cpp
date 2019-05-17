#include "Pathfinder.hpp"
#include "Obstacle.hpp"
#include <fstream>

using namespace std;
using json = nlohmann::json;

Pathfinder::Pathfinder(string const &filename) {
  loadBuildings(filename);
}

void Pathfinder::parseBuildingData(string const &data) {
  buildings = new vector<Obstacle*>();
  json jso = json::parse(data);

  for (auto i = jso["buildings"].begin(); i != jso["buildings"].end(); i++) {
    buildings->push_back(new Obstacle(*i));
  }
}

void Pathfinder::loadBuildings(string const &filename) {
  ifstream datafile(filename);
  string data;
  datafile.seekg(0, ios::end);
  data.reserve(datafile.tellg());
  datafile.seekg(0, ios::beg);
  data.assign(istreambuf_iterator<char>(datafile), istreambuf_iterator<char>());
  datafile.close();

  parseBuildingData(data);
}

Pathfinder::~Pathfinder() {
  if (buildings) {
    for (auto i = buildings->begin(); i != buildings->end(); i++) {
      delete *i;
    }
    delete buildings;
  }
  if (noFlyZones) delete noFlyZones;
}
