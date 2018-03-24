attribute vec3 position;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform sampler2D textureDisplacement;
uniform vec2 textureDimension;

varying vec3 fPosition;
varying vec3 fNormal;

uniform float time;

void main() {
	float t = time/1000.;
    vec3 offset = position;

    // TODO: Compute displaced vertices
    float heightScaling = 2.;

    // TODO: Compute displaced normals
    float normalScaling = 2.5;

    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    float h_uv = texture2D(textureDisplacement, uv).r;
    float dU = heightScaling * normalScaling * (texture2D(textureDisplacement, vec2(uv.x + 1.0/textureDimension.x, uv.y)).r - h_uv);
    float dV = heightScaling * normalScaling * (texture2D(textureDisplacement, vec2(uv.x, uv.y + 1.0/textureDimension.y)).r - h_uv);

    vec3 lNormal = vec3(-dU, -dV, 1.0);
    fPosition = offset + vec3(sin(t)*7., sin(t)*4., cos(t)*7.);
    fNormal = TBN * lNormal;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(fPosition, 1.0);
}