#ifndef Vector3_hpp
#define Vector3_hpp

#include <iostream>
#include <cmath>
#include <iomanip>
#include "nlohmann/json.hpp"

struct Vector3 {
  Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
  Vector3(float n[3]) : x(n[0]), y(n[1]), z(n[2]) {}
  Vector3() : x(0), y(0), z(0) {}
  Vector3(nlohmann::json const &j) : x(j["x"]), y(j["y"]), z(j["z"]) {}
  float x;
  float y;
  float z;

  Vector3 normalized() const;
  float magnitude() const;
  float sqrMagnitude() const;
  nlohmann::json serialize() const;

  Vector3 operator+(Vector3 const &v) const;
  Vector3 operator-(Vector3 const &v) const;
  float operator*(Vector3 const &v) const;
  Vector3 operator*(float const &c) const;
  Vector3 operator/(float const &c) const;
  std::string toString() const;

  static Vector3 normalize(Vector3 &v);
  static Vector3 one();
  static Vector3 up();
  static Vector3 down();
  static Vector3 right();
  static Vector3 forward();
  static Vector3 deserialize(nlohmann::json const &j);
  static float distance(Vector3 const &a, Vector3 const &b);
};

Vector3 operator*(float const &c, Vector3 const &v);

std::ostream& operator<<(std::ostream& os, Vector3 const &v);


#endif /* Vector3_hpp */
