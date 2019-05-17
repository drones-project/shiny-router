#include "Vector3.hpp"
#include <sstream>
using namespace std;

Vector3 Vector3::operator*(float const &c) const {
  return Vector3(c * x, c * y, c * z);
}

float Vector3::operator*(Vector3 const &v) const {
  return v.x * x + v.y * y + v.z * z;
}

Vector3 operator*(float const &c, Vector3 const &v) {
  return v * c;
}

Vector3 Vector3::operator/(float const &c) const {
  return *this * (1 / c);
}

Vector3 Vector3::operator+(Vector3 const &v) const {
  return Vector3(v.x + x, v.y + y, v.z + z);
}

Vector3 Vector3::operator-(Vector3 const &v) const {
  return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::normalized() const {
  return *this/magnitude();
}

float Vector3::sqrMagnitude() const {
  return x*x + y*y + z*z;
}

float Vector3::magnitude() const {
  return sqrt(sqrMagnitude());
}

string Vector3::toString() const {
  stringstream stream;
  stream << fixed << setprecision(2) << x << ", " << y << ", " << z;
  return stream.str();
}

Vector3 Vector3::normalize(Vector3 &v) {
  v = v.normalized();
  return v;
}

nlohmann::json Vector3::serialize() const{
  return nlohmann::json::object({
    {"x", x},
    {"y", y},
    {"z", z}});
}

Vector3 Vector3::one() {
  return Vector3(1, 1, 1);
}
Vector3 Vector3::up() {
  return Vector3(0, 1, 0);
}
Vector3 Vector3::down() {
  return Vector3(0, -1, 0);
}

Vector3 Vector3::right() {
  return Vector3(1, 0, 0);
}

Vector3 Vector3::forward() {
  return Vector3(0, 0, 1);
}

float Vector3::distance(Vector3 const &a, Vector3 const &b) {
  return (b - a).magnitude();
}

Vector3 Vector3::deserialize(nlohmann::json const &j) {
  return Vector3(j["x"], j["y"], j["z"]);
}

std::ostream& operator<<(std::ostream& os, Vector3 const &v) {
  os << "(" << v.toString() << ")";
  return os;
}


