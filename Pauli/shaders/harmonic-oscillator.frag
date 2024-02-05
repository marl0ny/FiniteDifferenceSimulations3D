#VERSION_NUMBER_PLACEHOLDER

precision highp float;

#if __VERSION__ >= 300
in vec2 UV;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor gl_FragColor
varying highp vec2 UV;
#endif

uniform float omega;
uniform float t;
uniform vec3 r0;
uniform vec3 kr;
uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;


vec3 to3DTextureCoordinates(vec2 uv) {
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    float wStack = float(width2D)/float(width3D);
    float hStack = float(height2D)/float(height3D);
    float wIndex = floor(uv[1]*hStack)*wStack + floor(uv[0]*wStack);
    return vec3(mod(uv[0]*wStack, 1.0), mod(uv[1]*hStack, 1.0),
                (wIndex + 0.5)/float(length3D));
}

void main() {
    vec3 uvw1 = to3DTextureCoordinates(UV);
    vec3 uvw2 = vec3(uvw1[0] - r0[0], uvw1[1] - r0[1], uvw1[2] - r0[2]);
    mat3 rMat;
    rMat[0] = vec3(cos(omega*t), sin(omega*t), 0.0);
    rMat[1] = vec3(sin(omega*t), -cos(omega*t), 0.0);
    rMat[2] = vec3(0.0, 0.0, 1.0);
    mat3 kMat1;
    kMat1[0] = vec3(kr[0], 0.0, 0.0);
    kMat1[1] = vec3(0.0, kr[1], 0.0);
    kMat1[2] = vec3(0.0, 0.0, kr[2]);
    mat3 kMat2 = transpose(rMat) * kMat1 * rMat;
    float pot = dot((uvw2 * kMat2), uvw2);
    fragColor = vec4(0.0, 0.0, 0.0, pot);

}