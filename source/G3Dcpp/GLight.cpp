/**
  @file GLight.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-11-12
  @edited  2003-11-13
*/

#include "G3D/GLight.h"

namespace G3D {

GLight::GLight() {
    position        = Vector4(0, 0, 0, 0);
    color           = Color3::WHITE;
    spotDirection   = Vector3(0, 0, -1);
    spotCutoff      = 180;
    enabled         = false;
    attenuation[0]  = 1.0;
    attenuation[1]  = 0.0;
    attenuation[2]  = 0.0;
}


GLight GLight::directional(const Vector3& toLight, const Color3& color) {
    GLight L;
    L.position = Vector4(toLight, 0);
    L.color    = color;
    return L;
}


GLight GLight::point(const Vector3& pos, const Color3& color, double constAtt, double linAtt, double quadAtt) {
    GLight L;
    L.position = Vector4(pos, 1);
    L.color    = color;
    L.attenuation[0] = constAtt;
    L.attenuation[1] = linAtt;
    L.attenuation[2] = quadAtt;
    return L;
}


GLight GLight::spot(const Vector3& pos, const Vector3& pointDirection, double cutOffAngleDegrees, const Color3& color, double constAtt, double linAtt, double quadAtt) {
    GLight L;
    L.position = Vector4(pos, 1);
    L.spotDirection = pointDirection;
    L.spotCutoff = cutOffAngleDegrees;
    L.color    = color;
    L.attenuation[0] = constAtt;
    L.attenuation[1] = linAtt;
    L.attenuation[2] = quadAtt;
    return L;
}


bool GLight::operator==(const GLight& other) {
    return memcmp(this, &other, sizeof(GLight)) == 0;
}

bool GLight::operator!=(const GLight& other) {
    return !(*this == other);
}

}
