#ifndef StaticObstacle_hpp
#define StaticObstacle_hpp

#include "nlohmann/json.hpp"
#include "Vector3.hpp"

struct Obstacle {
  Vector3 position;
  Vector3 size;
  Vector3 orientation;
  float diag;
  Vector3 dz;
  Vector3 dx;
  float mu;
  Vector3 normals[4]  = {Vector3(),Vector3(),Vector3(),Vector3()};
  Vector3 verts[4] = {Vector3(),Vector3(),Vector3(),Vector3()};

  Obstacle(nlohmann::json const &j);

  nlohmann::json serialize() const;

  static Obstacle deserialize(nlohmann::json const &j);
};
#endif /* StaticObstacle_h */
