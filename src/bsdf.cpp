#include "bsdf.h"

#include <iostream>
#include <algorithm>
#include <utility>

using std::min;
using std::max;
using std::swap;

namespace CGL {

void make_coord_space(Matrix3x3& o2w, const Vector3D& n) {

    Vector3D z = Vector3D(n.x, n.y, n.z);
    Vector3D h = z;
    if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z)) h.x = 1.0;
    else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z)) h.y = 1.0;
    else h.z = 1.0;

    z.normalize();
    Vector3D y = cross(h, z);
    y.normalize();
    Vector3D x = cross(z, y);
    x.normalize();

    o2w[0] = x;
    o2w[1] = y;
    o2w[2] = z;
}

// Diffuse BSDF //

Spectrum DiffuseBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  // TODO (Part 3.1): 
  // This function takes in both wo and wi and returns the evaluation of
  // the BSDF for those two directions.

  return reflectance/PI;
}

Spectrum DiffuseBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO (Part 3.1): 
  // This function takes in only wo and provides pointers for wi and pdf,
  // which should be assigned by this function.
  // After sampling a value for wi, it returns the evaluation of the BSDF
  // at (wo, *wi).

  *wi = sampler.get_sample(pdf);
  return reflectance/PI;
}

// Mirror BSDF //

Spectrum MirrorBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum MirrorBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO:
  // Implement MirrorBSDF
  reflect(wo, wi);
  *pdf = 1.0;
  return reflectance / abs_cos_theta(*wi);
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D& wo, const Vector3D& wi) {
    return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D& h) {
    // TODO: proj3-2, part 2
    // Compute Beckmann normal distribution function (NDF) here.
    // You will need the roughness alpha.
    double theta = acos(h.z);
    return exp(-pow(tan(theta)/alpha, 2.0))/(PI*pow(alpha, 2.0)*pow(h.z, 4.0));
}

Spectrum MicrofacetBSDF::F(const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Compute Fresnel term for reflection on dielectric-conductor interface.
    // You will need both eta and etaK, both of which are Spectrum.
    Spectrum etaSquare = eta*eta + k*k;
    Spectrum etaCos = 2.0*eta*wi.z;
    Spectrum cosSquare = Spectrum(wi.z*wi.z, wi.z*wi.z, wi.z*wi.z);
    Spectrum Rs = (etaSquare - etaCos + cosSquare)/(etaSquare + etaCos + cosSquare);
    Spectrum Rp = (etaSquare*cosSquare - etaCos + Spectrum(1.0, 1.0, 1.0))/(etaSquare*cosSquare + etaCos + Spectrum(1.0, 1.0, 1.0));
    return (Rs + Rp)/2.0;
}

Spectrum MicrofacetBSDF::f(const Vector3D& wo, const Vector3D& wi) {
    // TODO: proj3-2, part 2
    // Implement microfacet model here.
    if (wi.z <= 0 || wo.z <= 0) {
      return Spectrum();
    }
    Vector3D h = (wo + wi)/(wo+wi).norm();
    return (F(wi) * G(wo, wi) * D(h))/(4.0*wo.z*wi.z);
}

Spectrum MicrofacetBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
    // TODO: proj3-2, part 2
    // *Importance* sample Beckmann normal distribution function (NDF) here.
    // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
    //       and return the sampled BRDF value.

    Vector2D w = sampler.get_sample();
    double r1 = w.x;
    double r2 = w.y;
    double theta = atan(sqrt(-pow(alpha, 2.0)*log(1.0 - r1)));
    double phi = 2.0*PI*r2;
    Vector3D h = Vector3D(sin(theta)*sin(phi), sin(theta)*cos(phi), cos(theta));
    *wi = 2*h*dot(h, wo) - wo;
    if (wi->z <= 0) {
      *pdf = 0;
      return Spectrum();
    }
    double pTheta = 2.0*sin(theta)/(pow(alpha, 2.0)*pow(cos(theta), 3.0))/exp(pow(tan(theta)/alpha, 2.0));
    double pPhi  = 1/(2.0*PI);
    double ph = (pTheta * pPhi)/sin(theta);
    double pwi = ph/(4.0*dot(*wi, h));
    *pdf = pwi;
    //*wi = cosineHemisphereSampler.get_sample(pdf);
    return MicrofacetBSDF::f(wo, *wi);
}

// Refraction BSDF //

Spectrum RefractionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum RefractionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {

  // TODO:
  // Implement RefractionBSDF
  return Spectrum();
}

// Glass BSDF //

Spectrum GlassBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum GlassBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  // TODO: 3-2 Part 1 Task 4
  // Compute Fresnel coefficient and either reflect or refract based on it.
  if (!refract(wo, wi, ior)) {
    reflect(wo, wi);
    *pdf = 1.0;
    return reflectance / abs_cos_theta(*wi);
  } else {
    double R0 = pow((1.0 - ior)/(1.0 + ior), 2.0);
    double R = R0 + (1.0 - R0)*pow(1.0 - abs_cos_theta(wo), 5.0);
    if (coin_flip(R)) {
      reflect(wo, wi);
      *pdf = R;
      return R * reflectance / abs_cos_theta(*wi);
    } else {
      *pdf = 1.0 - R;
      if (wo.z >= 0) {
        return (1.0 - R) * transmittance / abs_cos_theta(*wi) / pow(1.0/ior, 2.0);
      } else {
        return (1.0 - R) * transmittance / abs_cos_theta(*wi) / pow(ior, 2.0);
      }
    }
  }
}


void BSDF::reflect(const Vector3D& wo, Vector3D* wi) {
  // TODO: 3-2 Part 1 Task 1
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = Vector3D(-wo.x, -wo.y, wo.z);
}

bool BSDF::refract(const Vector3D& wo, Vector3D* wi, float ior) {
  // TODO: 3-2 Part 1 Task 3
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When wo.z is positive, then wo corresponds to a
  // ray entering the surface through vacuum.
  double eta, sign;
  if (wo.z >= 0) {
    eta = 1.0/ior;
    sign = -1.0;
  } else {
    eta = ior;
    sign = 1.0;
  }
  double dis = 1.0 - eta*eta*(1.0 - wo.z*wo.z);
  if (dis < 0) {
    return false;
  } else {
    *wi = Vector3D(-eta*wo.x, -eta*wo.y, sign*sqrt(dis));
    return true;
  }
}

// Emission BSDF //

Spectrum EmissionBSDF::f(const Vector3D& wo, const Vector3D& wi) {
  return Spectrum();
}

Spectrum EmissionBSDF::sample_f(const Vector3D& wo, Vector3D* wi, float* pdf) {
  *pdf = 1.0 / PI;
  *wi  = sampler.get_sample(pdf);
  return Spectrum();
}

} // namespace CGL
