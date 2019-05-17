#include <stdio.h>
#include "Obstacle.hpp"
#include "nlohmann/json.hpp"

nlohmann::json Obstacle::serialize() const {
  using json = nlohmann::json;
  json j = {
    {"position", position.serialize()},
    {"size", size.serialize()},
    {"orientation", orientation.serialize()},
    {"diag", diag},
    {"dz", dz.serialize()},
    {"dx", dx.serialize()},
    {"mu", mu},
    {"normals", json::array({
      normals[0].serialize(),
      normals[1].serialize(),
      normals[2].serialize(),
      normals[3].serialize()
    })},
    {"verts", json::array({
      verts[0].serialize(),
      verts[1].serialize(),
      verts[2].serialize(),
      verts[3].serialize()
    })},
  };
  return j;
}

Obstacle::Obstacle(nlohmann::json const &j) :
  position(Vector3(j["position"])),
  size(Vector3(j["size"])),
  orientation(Vector3(j["orientation"])),
  diag(j["diag"]),
  dz(Vector3(j["dz"])),
  dx(Vector3(j["dx"])),
  mu(j["mu"]) {
    for (int i = 0; i < 4; i++) {
      normals[i] = Vector3(j["normals"][i]);
      verts[i] = Vector3(j["verts"][i]);
    }
}

Obstacle Obstacle::deserialize(nlohmann::json const &j) {
  return Obstacle(j);
}
