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

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;


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

vec3 revBitSort2(vec3 uvw) {
    vec3 uvw2 = vec3(0.0, 0.0, 0.0);
    int indexU = int(floor(uvw[0]*float(texelDimensions3D[0])));
    int indexV = int(floor(uvw[1]*float(texelDimensions3D[1])));
    int indexW = int(floor(uvw[2]*float(texelDimensions3D[2])));
    // u
    int rev = int(0), i = indexU;
    for (int asc = 1,
         des = texelDimensions3D[0]/2; des > 0; des /= 2, asc *= 2) {
        if (i/des > 0) {
            rev += asc;
            i -= des;
        }
    }
    uvw2[0] = (float(rev) + 0.5)/float(texelDimensions3D[0]);
    // v
    rev = 0, i = indexV;
    for (int asc = 1,
         des = texelDimensions3D[1]/2; des > 0; des /= 2, asc *= 2) {
        if (i/des > 0) {
            rev += asc;
            i -= des;
        }
    }
    uvw2[1] = (float(rev) + 0.5)/float(texelDimensions3D[1]);
    // w
    rev = 0, i = indexW;
    for (int asc = 1,
         des = texelDimensions3D[2]/2; des > 0; des /= 2, asc *= 2) {
        if (i/des > 0) {
            rev += asc;
            i -= des;
        }
    }
    uvw2[2] = (float(rev) + 0.5)/float(texelDimensions3D[2]);
    return uvw2;
}


void main() {
    vec3 uvw = to3DTextureCoordinates(UV);
    fragColor = texture2D(tex, to2DTextureCoordinates(revBitSort2(uvw)));
}