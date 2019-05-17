#ifndef RayPath_hpp
#define RayPath_hpp

#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include "Pathfinder.hpp"
#include "Vector3.hpp"
#include "Obstacle.hpp"

typedef std::priority_queue<Obstacle*, std::vector<Obstacle*>, std::function<bool(Obstacle*, Obstacle*)>> BuildingHeap;
class RayPath : public Pathfinder {
private:
  /* Data */
  float const _maxAlt = 250;
  float const _minAlt = 60;
  float const _altDiv = 10;
  float const _buildingDiv = 30;
  float const _Ra = 200;
  float const _Rd = 3;
  float const _epsilon = 0.001;
  float const _hubAlt[2] = { 500, 510 };
  int const _altSize = (int)((_maxAlt - _minAlt)/_altDiv + 1);
  float* _altitudes = nullptr;
  int* _assigned = nullptr;
  int _droneCount = 0;
  Vector3 _origin;
  Vector3 _destination;
  std::vector<std::vector<Obstacle*>*>* sortedBuildings = nullptr;

  /* Functions */
  static void _rotationY(float degree, Vector3& v);
  static Vector3 _getPerp(Vector3& v);

  void _getBuildings();
  int _countAt(int i, std::vector<Vector3> &dronePos) const;

  BuildingHeap _blockers(Vector3 const &start, Vector3 const &end,
                               float alt) const;

  int _findIntersect(Obstacle &obs, Vector3 const &start,
                     Vector3 const &end, int i[2]) const;

  Vector3 _findAlternate(Obstacle &obs,
                         Vector3 const &start, Vector3 const &exclude) const;
  bool _isContained(Obstacle &obs, Vector3 const &point) const;

  Vector3 _findWaypoint(Obstacle &obs, Vector3 const &start,
                        Vector3 const &end, int i[2]) const;

  std::vector<Vector3> _route(bool hubReturn);
  std::vector<Vector3> _navigate(Vector3 start, Vector3 end, float altitude) const;

  void _getAltitudes();
  int _chooseAltitude(Vector3 const &origin, Vector3 const &dest) const;

public:
  RayPath(std::string const & filename);
  virtual ~RayPath();

  std::string getRoute(std::string const &raw) override;
  void updateDroneCount(std::vector<Vector3> &dronePos);
};

#endif /* RayPath_hpp */
