#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c);
  } else {
    draw(node->l, c);
    draw(node->r, c);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c);
  } else {
    drawOutline(node->l, c);
    drawOutline(node->r, c);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // TODO (Part 2.1):
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.

  BBox centroid_box, bbox;

  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c);
  }

  BVHNode *node = new BVHNode(bbox);
  if (prims.size() <= max_leaf_size) {
    node->prims = new vector<Primitive *>(prims);
    node->l = NULL;
    node->r = NULL;
    return node;
  } else {
    vector<Primitive *> *prims1 = new vector<Primitive *>();
    vector<Primitive *> *prims2 = new vector<Primitive *>();
    while (prims1->size() == 0 || prims2->size() == 0) {
      double x = bbox.extent[0], y = bbox.extent[1], z = bbox.extent[2];
      Vector3D split = Vector3D(0.5*(bbox.min[0] + bbox.max[0]), 0.5*(bbox.min[1] + bbox.max[1]), 0.5*(bbox.min[2] + bbox.max[2]));
      int axis;
      if (x > y && x > z) {
        axis = 0;
      } else if (y > x && y > z) {
        axis = 1;
      } else {
        axis = 2;
      }
      prims1 = new vector<Primitive *>();
      prims2 = new vector<Primitive *>();
      for (Primitive *p : prims) {
        if (p->get_bbox().centroid()[axis] <= split[axis]) {
          prims1->push_back(p);
        } else {
          prims2->push_back(p);
        }
      }
      if (prims1->size() == 0) {
        bbox.min[axis] = split[axis]; 
      } else if (prims2->size() == 0) {
        bbox.max[axis] = split[axis];
      }
    }
    node->l = construct_bvh(*prims1, max_leaf_size);
    node->r = construct_bvh(*prims2, max_leaf_size);
    return node;
  }
}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.
  double t0 = ray.min_t, t1 = ray.max_t;
  bool isect = node->bb.intersect(ray, t0, t1);
  if (!isect) {
    return false;
  }
  if (node->l == NULL && node->r == NULL) {
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray)) {
        return true;
      }
    }
    return false;
  }
  return (intersect(ray, node->l) || intersect(ray, node->r));
}

bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {
  // TODO (Part 2.3):
  // Fill in the intersect function.
  double t0 = ray.min_t, t1 = ray.max_t;
  bool isect = node->bb.intersect(ray, t0, t1);
  if (!isect) {
    return false;
  }
  if (node->l == NULL && node->r == NULL) {
    bool isects = false;
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray, i)) {
        isects = true;
      }
    }
    return isects;
  }
  bool hit1 = intersect(ray, i, node->l);
  bool hit2 = intersect(ray, i, node->r);
  return (hit1 || hit2);
}

}  // namespace StaticScene
}  // namespace CGL
