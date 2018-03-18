#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // TODO (Part 2.2):
  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  double tx1, tx2, ty1, ty2, tz1, tz2;
  if (r.d[0] == 0) {
    tx1 = -INF_D;
    tx2 = INF_D;
  } else {
    tx1 = (min[0] - r.o[0])/r.d[0];
    tx2 = (max[0] - r.o[0])/r.d[0];
  }
  if (r.d[1] == 0) {
    ty1 = -INF_D;
    ty2 = INF_D;
  } else {
    ty1 = (min[1] - r.o[1])/r.d[1];
    ty2 = (max[1] - r.o[1])/r.d[1];
  }
  if (r.d[2] == 0) {
    tz1 = -INF_D;
    tz2 = INF_D;
  } else {
    tz1 = (min[2] - r.o[2])/r.d[2];
    tz2 = (max[2] - r.o[2])/r.d[2];
  }
  double min_x = std::min(tx1, tx2);
  double max_x = std::max(tx1, tx2);
  double min_y = std::min(ty1, ty2);
  double max_y = std::max(ty1, ty2);
  double min_z = std::min(tz1, tz2);
  double max_z = std::max(tz1, tz2);
  double min_t = std::max(std::max(min_x, min_y), min_z);
  double max_t = std::min(std::min(max_x, max_y), max_z);
  if (min_t > max_t || min_t > t1 || max_t < t0) {
    return false;
  } else {
    t0 = min_t;
    t1 = max_t;
    return true;
  }
}

void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
