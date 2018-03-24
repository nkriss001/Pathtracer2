precision highp float;

uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.1
    float k_d = 1.0;
    float dist = length(lPosition - fPosition);
    gl_FragColor = k_d*vec4(lIntensity/(dist*dist) * max(0.0, dot((lPosition - fPosition)/dist, fNormal/length(fNormal))), 1.0);
}