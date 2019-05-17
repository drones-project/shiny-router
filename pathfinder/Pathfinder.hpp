#ifndef PathFinder_hpp
#define PathFinder_hpp

#include <vector>
#include <string>
#include "nlohmann/json.hpp"

class Obstacle;

class Pathfinder {
public:
  Pathfinder(std::string const &filename);
  virtual ~Pathfinder();
protected:
  std::vector<Obstacle*>* buildings = nullptr;
  std::vector<Obstacle>* noFlyZones = nullptr;

  virtual std::string getRoute(std::string const &data) = 0;
private:
  void loadBuildings(std::string const &filename);
  void parseBuildingData(std::string const &data);
};

#endif /* PathFinder_hpp */
