#pragma once

#include "constants.h"
#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

struct Ray;
void rk4step(Ray& ray, double dλ, double rs);


struct Ray {
    vec2 pos;
    vec2 dir;
    double r; double phi;
    double dr; double dphi;
    double d2r; double d2phi;
    double E, L; // conservede quantities
    vector<vec2> trail;

    Ray(vec2 p, vec2 d, double r_s) : pos(p), dir(d) {
        r = hypot(pos.x, pos.y);
        phi = atan(pos.y, pos.x);
        dr = c * cos(phi) + dir.y * cos(phi) / r;
        dphi = (-c * sin(phi) + dir.y * cos(phi)) / r;
        d2r = 0.0;
        d2phi = 0.0;

        L = r*r*dphi;
        double f = 1.0 - r_s / r;
        double dt_dλ = sqrt((dr*dr)/(f*f) + (r*r*dphi*dphi)/f);
        E = f * dt_dλ;
    };

    void step(double r_s, double dλ) {        
        if (r < r_s) return;

        rk4step(*this, dλ, r_s);

        pos.x = cos(phi) * r;
        pos.y = sin(phi) * r;

        trail.push_back(pos);
    }
};

void geodesic(Ray& ray, double rhs[4], double r_s) {
    double r    = ray.r;
    double dr   = ray.dr;
    double dphi = ray.dphi;
    double E    = ray.E;

    double f = 1.0 - r_s / r;

    rhs[0] = dr;
    rhs[1] = dphi;

    double dt_dλ = E / f;
    rhs[2] = 
        - (r_s/(2*r*r)) * f * (dt_dλ*dt_dλ)
        + (r_s/(2*r*r*f)) * (dr*dr)
        + (r - r_s) * (dphi*dphi);

    rhs[3] = -2.0 * dr * dphi / r;
}

void addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++) {
        out[i] = a[i] + b[i] * factor;
    }
}

void rk4step(Ray& ray, double dλ, double rs) {
    double y0[4] = {ray.r, ray.phi, ray.dr, ray.dphi};
    double k1[4], k2[4], k3[4], k4[4], temp[4];

    geodesic(ray, k1, rs);
    addState(y0, k1, dλ/2.0, temp);
    Ray r2 = ray; r2.r=temp[0]; r2.phi=temp[1]; r2.dr=temp[2]; r2.dphi=temp[3];
    geodesic(r2, k2, rs);

    addState(y0, k2, dλ/2.0, temp);
    Ray r3 = ray; r3.r=temp[0]; r3.phi=temp[1]; r3.dr=temp[2]; r3.dphi=temp[3];
    geodesic(r3, k3, rs);

    addState(y0, k3, dλ/2.0, temp);
    Ray r4 = ray; r4.r=temp[0]; r4.phi=temp[1]; r4.dr=temp[2]; r4.dphi=temp[3];
    geodesic(r4, k4, rs);

    ray.r    += (dλ/6.0)*(k1[0] + 2*k2[0] + 2*k3[0] + k4[0]);
    ray.phi  += (dλ/6.0)*(k1[1] + 2*k2[1] + 2*k3[1] + k4[1]);
    ray.dr   += (dλ/6.0)*(k1[2] + 2*k2[2] + 2*k3[2] + k4[2]);
    ray.dphi += (dλ/6.0)*(k1[3] + 2*k2[3] + 2*k3[3] + k4[3]);
}