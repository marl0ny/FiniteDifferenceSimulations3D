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

uniform sampler2D tex;
uniform float blockSize;
uniform int orientation;
uniform float angleSign;
uniform float size;
uniform float scale;

uniform ivec2 texelDimensions2D;
uniform ivec3 texelDimensions3D;

const float TAU = 6.283185307179586;

const int ORIENTATION_0 = 0;
const int ORIENTATION_1 = 1;
const int ORIENTATION_2 = 2;


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
    float wIndex = w*float(length3D) - 0.5;
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

vec4 getOdd1(vec3 uvw) {
    if (orientation == ORIENTATION_0) {
        vec3 uvw2 = vec3(uvw[0] + 0.5*blockSize, uvw[1], uvw[2]);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    } else if (orientation == ORIENTATION_1) {
        vec3 uvw2 = vec3(uvw[0], uvw[1] + 0.5*blockSize, uvw[2]);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    } else if (orientation == ORIENTATION_2) {
        vec3 uvw2 = vec3(uvw[0], uvw[1], uvw[2] + 0.5*blockSize);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    }
}

vec4 getEven2(vec3 uvw) {
    if (orientation == ORIENTATION_0) {
        vec3 uvw2 = vec3(uvw[0] - 0.5*blockSize, uvw[1], uvw[2]);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    } else if (orientation == ORIENTATION_1) {
        vec3 uvw2 = vec3(uvw[0], uvw[1] - 0.5*blockSize, uvw[2]);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    } else if (orientation == ORIENTATION_2) {
        vec3 uvw2 = vec3(uvw[0], uvw[1], uvw[2] - 0.5*blockSize);
        return texture2D(tex, to2DTextureCoordinates(uvw2));
    }
}

void main() {
    vec3 uvw = to3DTextureCoordinates(UV);
    float val = 0.0;
    if (orientation == ORIENTATION_0) {
        val = mod(uvw[0], blockSize);
    } else if (orientation == ORIENTATION_1) {
        val = mod(uvw[1], blockSize);
    } else if (orientation == ORIENTATION_2) {
        val = mod(uvw[2], blockSize);
    }

    // Even lower half
    vec4 even1 = texture2D(tex, UV);
    vec4 odd1 = getOdd1(uvw);
    float phi1 = angleSign*TAU*(val - 0.5/size)/(blockSize);
    float cosVal1 = cos(phi1);
    float sinVal1 = sin(phi1);
    vec4 expOdd1 = vec4(odd1.r*cosVal1 - odd1.g*sinVal1,
                        odd1.r*sinVal1 + odd1.g*cosVal1,
                        odd1.b*cosVal1 - odd1.a*sinVal1,
                        odd1.b*sinVal1 + odd1.a*cosVal1);
    vec4 out1 = scale*(even1 + expOdd1);

    // Odd upper half
    vec4 even2 = getEven2(uvw);
    vec4 odd2 = texture2D(tex, UV);
    float phi2 = angleSign*TAU*((val - 0.5/size) - blockSize/2.0)/(blockSize);
    float cosVal2 = cos(phi2);
    float sinVal2 = sin(phi2);
    vec4 expOdd2 = vec4(odd2.r*cosVal2 - odd2.g*sinVal2,
                        odd2.r*sinVal2 + odd2.g*cosVal2,
                        odd2.b*cosVal2 - odd2.a*sinVal2,
                        odd2.b*sinVal2 + odd2.a*cosVal2);
    vec4 out2 = scale*(even2 - expOdd2);

    fragColor = (val <= blockSize/2.0)? out1: out2;
}