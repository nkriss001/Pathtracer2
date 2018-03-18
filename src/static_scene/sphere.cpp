#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  double a = dot(r.d, r.d);
  double b = 2.0*dot(r.o - o, r.d);
  double c = dot(r.o - o, r.o - o) - r2;
  double disc = b*b - 4.0*a*c;
  if (disc < 0) {
    return false;
  }
  t1 = (-b - sqrt(disc))/(2.0*a);
  t2 = (-b + sqrt(disc))/(2.0*a);
  if (t2 < r.min_t || t1 > r.max_t || (t1 < r.min_t && t2 > r.max_t)) {
    return false;
  }
  return true;
}

bool Sphere::intersect(const Ray& r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  bool isect = test(r, t1, t2);
  if (isect) {
    if (t1 > r.min_t) {
      r.max_t = t1;
    } else {
      r.max_t = t2;
    }
  }
  return isect;
}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  bool isect = test(r, t1, t2);
  if (isect) {
    if (t1 > r.min_t) {
      r.max_t = t1;
    } else {
      r.max_t = t2;
    }
    i->t = r.max_t;
    i->primitive = this;
    Vector3D point = r.o + (r.max_t)*r.d;
    i->n = (point - o)/(point - o).norm();
    i->bsdf = get_bsdf();
  }
  return isect;

}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
