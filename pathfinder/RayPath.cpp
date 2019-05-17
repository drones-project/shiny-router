#include "RayPath.hpp"
#include <cmath>
#include <set>
#include <exception>
using namespace std;

RayPath::RayPath(string const &filename) : Pathfinder(filename) {
  _getAltitudes();
  _getBuildings();
}

RayPath::~RayPath() {
  if (_altitudes) delete _altitudes;
  if (_assigned) delete _assigned;
  if (sortedBuildings) {
    for (auto i = sortedBuildings->begin(); i != sortedBuildings->end();
         i++) {
      delete *i;
    }
    delete sortedBuildings;
  }
}

void RayPath::_rotationY(float degree, Vector3& v) {
  float rad = degree * M_PI / 180;
  float cos_t = cos(rad);
  float sin_t = sin(rad);
  
  v.x = v.x * cos_t + v.z * sin_t;
  v.z = v.x * -sin_t + v.z * cos_t;
}

Vector3 RayPath::_getPerp(Vector3 &v) {
  auto out = v;
  _rotationY(90, out);
  return out;
}

void RayPath::_getAltitudes() {
  if (!_altitudes) {
    _altitudes = new float[_altSize];
    _assigned = new int[_altSize];
    for (int i = 0; i < _altSize; i++) {
      _altitudes[i] = _minAlt + i * _altDiv;
      _assigned[i] = 0;
    }
  }
}

void RayPath::_getBuildings() {
  if (sortedBuildings) return;
  
  int added = 0;
  int i = 0;
  float lower, upper;
  sortedBuildings = new vector<vector<Obstacle*>*>();
  
  while (added < buildings->size()) {
    sortedBuildings->push_back(new vector<Obstacle*>());
    for (auto j = buildings->begin(); j != buildings->end(); j++) {
      lower = i * _buildingDiv;
      upper = (i + 1) * _buildingDiv;
      if ((*j)->size.y < upper && (*j)->size.y >= lower) {
        (*sortedBuildings)[i]->push_back(*j);
        added++;
      }
    }
    i++;
  }
}

int RayPath::_countAt(int i, vector<Vector3> &dronePos) const {
  int count = 0;
  for (int i = 0; i < dronePos.size(); i++) {
    if (_altitudes[i] - _altDiv / 2 < dronePos[i].y &&
        _altitudes[i] + _altDiv / 2 > dronePos[i].y)
      count++;
  }
  return count;
}

void RayPath::updateDroneCount(vector<Vector3> &dronePos) {
  _droneCount = (int)dronePos.size();
  
  for (int i = 0; i < _altSize; i++) {
    _assigned[i] = _countAt(i, dronePos);
  }
}

int RayPath::_chooseAltitude(Vector3 const &origin, Vector3 const &dest) const {
  int max = 0;
  int start = (dest - origin).z > 0 ? 0 : 1;
  int maxIndex = _altSize - 1;
  for (int i = start; i < _altSize; i += 2) {
    auto tmp = _altitudes[i] / _maxAlt / (_assigned[i] / _droneCount + 1);
    if (tmp > max) {
      max = tmp;
      maxIndex = i;
    }
  }
  _assigned[maxIndex]++;
  return maxIndex;
}

vector<Vector3> RayPath::_route(bool hubReturn) {
  float alt;
  if (hubReturn)
    alt = (_destination - _origin).z > 0 ? _hubAlt[0] : _hubAlt[1];
  else
    alt = _altitudes[_chooseAltitude(_origin, _destination)];
  
  vector<Vector3> waypts = _navigate(_origin, _destination, alt);
  for (auto i = waypts.begin(); i != waypts.end(); i++)
    i->y = alt;
  
  return waypts;
}

BuildingHeap RayPath::_blockers(Vector3 const &start, Vector3 const &end,
                                      float alt) const {
  auto dir = end - start;
  auto obstacles = BuildingHeap([](Obstacle* l, Obstacle* r) {
    return (l->mu >= r->mu);
  });
  
  auto perp = _getPerp(dir).normalized() * _Ra;
  
  for (int i = (int)(alt / _buildingDiv); i < sortedBuildings->size(); i++) {
    for (int j = 0; j < (*sortedBuildings)[i]->size(); j++) {
      auto obs = (*(*sortedBuildings)[i])[j];
      if (obs->size.y > alt - _altDiv / 2) {
        auto mu = ((obs->position - start) * dir) / dir.sqrMagnitude();
        auto nu = ((start - obs->position) * perp) / perp.sqrMagnitude();
        if (nu <= 1 && nu >= -1 && mu <= 1 + _Ra/dir.magnitude() && mu >= 0) {
          obs->mu = mu;
          obstacles.push(obs);
        }
      }
    }
  }
  return obstacles;
}

int RayPath::_findIntersect(Obstacle &obs, Vector3 const &start,
                            Vector3 const &end, int i[2]) const {
  auto dir = end - start;
  auto _dir = dir.normalized();
  auto path = [&, dir, start](float m) {return start + (m * dir);};
  int num = 0;
  
  float mu[] = { 0, 0, 0, 0 };
  i[0] = -1; i[1] = -1;
  
  for (int j = 0; j < 4; j++) {
    if (abs(_dir * obs.normals[j]) > _epsilon) {
      mu[j] = ((obs.verts[j] - start) * obs.normals[j]) / (dir * obs.normals[j]);
      auto tmp = path(mu[j]);
      if (Vector3::distance(tmp, obs.position) < obs.diag / 2 &&
          mu[j] > 0 && mu[j] <= 1) {
        i[num++] = j;
      }
    }
  }
  
  if (num > 1 && mu[i[1]] < mu[i[0]]) {
    swap<int>(i[0],i[1]);
  }
  return num;
}

Vector3 RayPath::_findAlternate(Obstacle &obs, Vector3 const &start,
                                Vector3 const &exclude) const {
  for (auto i : obs.verts) {
    auto point = i + _Rd * (i - obs.position).normalized();
    if (Vector3::distance(point, start) > _epsilon &&
        Vector3::distance(point, exclude) > _epsilon &&
        Vector3::distance(point, exclude) < obs.diag)
      return point;
  }
  return start;
}

bool RayPath::_isContained(Obstacle &obs, Vector3 const &point) const {
  for (int i = 0; i < 4; i++) {
    if ((point - obs.verts[i]) * obs.normals[i] > 0) return false;
  }
  return true;
}

Vector3 RayPath::_findWaypoint(Obstacle &obs, Vector3 const &start,
                               Vector3 const &end, int i[2]) const {
  auto _dir = (end - start).normalized();
  Vector3 wp;
  
  if (i[1] == -1) {
    int j[2];
    int n = _findIntersect(obs, start, end, j);
    if (n > 0) return _findWaypoint(obs, start, end + (obs.diag *_dir), j);
  }
  
  Vector3 a;
  Vector3 b;
  if (abs(i[1] - i[0]) == 1 || abs(i[1] - i[0]) == 3) {
    int j;
    if (abs(i[1] - i[0]) == 1 ) j = i[1] < i[0] ? i[1] : i[0];
    else j = 3;
    a = obs.verts[j] + _Rd * (obs.verts[j] - obs.position).normalized();
    b = obs.verts[(j + 1) % 4] + _Rd * (obs.verts[(j + 1) % 4] - obs.position).normalized();
    wp = ((a - start).magnitude() > _epsilon) ? a : b;
  } else {
    a = obs.verts[i[0]] + _Rd * (obs.verts[i[0]] - obs.position).normalized();
    b = obs.verts[(i[1] + 1) % 4] + _Rd * (obs.verts[(i[1] + 1) % 4] -
                                           obs.position).normalized();
    if ((a - start).magnitude() > _epsilon &&
        (b - start).magnitude() > _epsilon) {
      wp = abs((a - start).normalized() * _dir) >
      abs((b - start).normalized() * _dir) ? a : b;
    } else {
      wp = ((a - start).magnitude() > _epsilon) ? a : b;
    }
  }
  for (auto i = noFlyZones->begin(); i != noFlyZones->end(); i++) {
    if (_isContained(*i, wp)) wp = _findAlternate(obs, start, wp);
  }
  return wp;
}

std::vector<Vector3> RayPath::_navigate(Vector3 start, Vector3 end,
                                        float altitude) const {
  static int stack = 0;
  stack++;
  if (stack > 1000) throw overflow_error("Too Many Stacks!");
  vector<Vector3> waypoints({start});
  auto dir = end - start;
  if (dir.magnitude() < _epsilon) return waypoints;
  auto buildings = _blockers(start, end, altitude);
  
  static auto cmp = [&, start, dir](Vector3 a, Vector3 b) {
    auto mua = ((a - start) * dir) / dir.sqrMagnitude();
    auto mub = ((b - start) * dir) / dir.sqrMagnitude();
    return (mua > mub);
  };
  
  priority_queue<Vector3, vector<Vector3>, decltype(cmp)> poss(cmp);
  set<string> errors;
  
  bool intersected = false;
  int indices[2];
  for (auto i = noFlyZones->begin(); i != noFlyZones->end(); i++) {
    if (_findIntersect(*i, start, end, indices)) {
      intersected = true;
      auto v = _findWaypoint(*i, start, end, indices);
      poss.push(v);
      if (indices[1] == -1) errors.insert(v.toString());
    }
  }
  
  int k = 0;
  while (!buildings.empty() && k < 5) {
    auto obs = buildings.top();
    buildings.pop();
    if (_findIntersect(*obs, start, end, indices) > 0) {
      k++;
      intersected = true;
      auto v = _findWaypoint(*obs, start, end, indices);
      poss.push(v);
      if (indices[1] == -1) errors.insert(v.toString());
    }
  }
  if (intersected) {
    auto nxt = poss.top();
    while (!poss.empty()) poss.pop();
    while (!buildings.empty()) buildings.pop();
    auto lst = _navigate(start, nxt, altitude);
    for (auto i = lst.begin() + 1; i != lst.end(); i++) {
      waypoints.push_back(*i);
    }
    if (errors.size() > 0 && errors.count(nxt.toString()) > 0) {
      end = _destination;
    }
    lst = _navigate(*lst.rbegin(), end, altitude);
    for (auto i = lst.begin() + 1; i != lst.end(); i++) {
      waypoints.push_back(*i);
    }
  } else {
    waypoints.push_back(end);
  }
  stack--;
  return waypoints;
}

using json = nlohmann::json;
string RayPath::getRoute(string const &raw) {
  if (noFlyZones) delete noFlyZones;
  
  noFlyZones = new vector<Obstacle>;
  json data = json::parse(raw);
  for (auto i = data["noFlyZones"].begin();
       i != data["noFlyZones"].end(); i++) {
    noFlyZones->push_back(Obstacle(*i));
  }
  
  vector<Vector3> dronePos;
  for (auto i = data["dronePositions"].begin();
       i != data["dronePositions"].end(); i++) {
    dronePos.push_back(Vector3(*i));
  }
  
  updateDroneCount(dronePos);
  
  _origin = Vector3(data["origin"]);
  _destination = Vector3(data["destination"]);
  _origin.y = 0;
  _destination.y = 0;
  
  vector<Vector3> route;
  try {
    route = _route(!data["onJob"]);
  } catch (overflow_error) {
    route = { _origin, _destination };
  }
  
  if (route.rbegin()->y > 505) {
    auto v = *route.rbegin();
    v.y = 500;
    route.push_back(v);
  }
  
  json out;
  for (auto i = route.begin(); i != route.end(); i++) {
    out["waypoints"].push_back(i->serialize());
  }
  
  out["frequentRequest"] = false;
  
  return out.dump();
}
