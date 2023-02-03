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

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

uniform float amplitude0;
uniform float amplitude1;
uniform vec3 r0;
uniform vec3 r1;
uniform vec3 sigma0;
uniform vec3 sigma1;
uniform vec3 wavenumber0;
uniform vec3 wavenumber1;

uniform sampler2D boundaryMaskTex;

const float PI = 3.141592653589793;

vec2 to2DTextureCoordinates(vec3 position) {
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    float wStack = float(width2D)/float(width3D);
    float hStack = float(height2D)/float(height3D);
    float u = position.x;
    float v = position.y;
    float w = position.z;
    float wRatio = 1.0/wStack;
    float hRatio = 1.0/hStack;
    float wIndex = floor(w*float(length3D));
    // if (wIndex >= float(length3D)) wIndex = 0.0;
    vec2 wPosition = vec2(mod(wIndex ,wStack)/wStack,
                          floor(wIndex/wStack)/hStack);
    return wPosition + vec2(u*wRatio, v*hRatio);
}

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
    vec3 uvw = to3DTextureCoordinates(UV);
    float gaussian0 = exp(-0.5*pow((uvw[0] - r0[0])/sigma0[0], 2.0)
                          -0.5*pow((uvw[1] - r0[1])/sigma0[1], 2.0)
                          -0.5*pow((uvw[2] - r0[2])/sigma0[2], 2.0));
    float gaussian1 = exp(-0.5*pow((uvw[0] - r1[0])/sigma1[0], 2.0)
                          -0.5*pow((uvw[1] - r1[1])/sigma1[1], 2.0)
                          -0.5*pow((uvw[2] - r1[2])/sigma1[2], 2.0));
    vec2 phase0 = vec2(cos(2.0*PI*dot(wavenumber0, uvw)),
                       sin(2.0*PI*dot(wavenumber0, uvw)));
    vec2 phase1 = vec2(cos(2.0*PI*dot(wavenumber1, uvw)),
                       sin(2.0*PI*dot(wavenumber1, uvw)));
    float bVal = texture2D(boundaryMaskTex, to2DTextureCoordinates(uvw))[0];
    fragColor = bVal*vec4(amplitude0*gaussian0*phase0,
                          amplitude1*gaussian1*phase1);

}
