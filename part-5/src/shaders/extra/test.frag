precision highp float;

uniform vec3 cameraPosition;
uniform vec3 lPosition;
uniform vec3 lIntensity;

varying vec3 fPosition;
varying vec3 fNormal;

void main() {
    // TODO: Part 5.2
    float k_a = 0.0;
    float k_d = 0.8;
    float k_s = 0.5;
    float I_a = 1.0;
    float p = 28.0;
    float dist = length(lPosition - fPosition);
    vec3 h = normalize(lPosition + cameraPosition - 2.0*fPosition);
    gl_FragColor = vec4(153., 77., 0., 1.)/(100.) * (k_a*I_a + vec4(k_d * lIntensity/(dist*dist) * max(0.0, dot(normalize(lPosition - fPosition), normalize(fNormal))) + k_s * lIntensity/(dist*dist) * max(0.0, pow(dot(h, normalize(fNormal)), p)), 1.0));
}