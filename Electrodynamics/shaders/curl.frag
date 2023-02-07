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


// Currently only 2nd and 4th order are supported.
// If orderOfAccuracy < 4 then only use the 2nd order accurate stencil,
// else default to 4th order.
uniform int orderOfAccuracy;

uniform int sampleOffset;

uniform sampler2D tex;

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

uniform vec3 dr; // Spartial step sizes
uniform vec3 dimensions3D; // Dimensions of simulation



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
    float wIndex = mod(floor(w*float(length3D)), float(length3D));
    // if (wIndex >= float(length3D)) wIndex = 0.0;
    vec2 wPosition = vec2(mod(wIndex ,wStack)/wStack,
                          floor(wIndex/wStack)/hStack);
    return wPosition + vec2(mod(u*wRatio, wRatio),
                            mod(v*hRatio, hRatio));
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

vec4 xDiff2ndOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float xl = x + offset*dr[0]/dimensions3D[0];
    float xr = x + (offset + 1.0)*dr[0]/dimensions3D[0];
    vec4 l = texture2D(tex, to2DTextureCoordinates(vec3(xl, y, z)));
    vec4 r = texture2D(tex, to2DTextureCoordinates(vec3(xr, y, z)));
    return 0.5*(r - l)/dr[0];
}

vec4 yDiff2ndOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float yd = y + offset*dr[1]/dimensions3D[1];
    float yu = y + (offset + 1.0)*dr[1]/dimensions3D[1];
    vec4 d = texture2D(tex, to2DTextureCoordinates(vec3(x, yd, z)));
    vec4 u = texture2D(tex, to2DTextureCoordinates(vec3(x, yu, z)));
    return 0.5*(u - d)/dr[1];
}

vec4 zDiff2ndOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float zb = z + offset*dr[2]/dimensions3D[2];
    float zf = z + (offset + 1.0)*dr[2]/dimensions3D[2];
    vec4 b = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb)));
    vec4 f = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf)));
    return 0.5*(f - b)/dr[2];
}

vec4 xDiff4thOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float xl2 = x + (offset - 1.0)*dr[0]/dimensions3D[0];
    float xl1 = x + offset*dr[0]/dimensions3D[0];
    float xr1 = x + (offset + 1.0)*dr[0]/dimensions3D[0];
    float xr2 = x + (offset + 2.0)*dr[0]/dimensions3D[0];
    vec4 l2 = texture2D(tex, to2DTextureCoordinates(vec3(xl2, y, z)));
    vec4 l1 = texture2D(tex, to2DTextureCoordinates(vec3(xl1, y, z)));
    vec4 r1 = texture2D(tex, to2DTextureCoordinates(vec3(xr1, y, z)));
    vec4 r2 = texture2D(tex, to2DTextureCoordinates(vec3(xr2, y, z)));
    return (l2/24.0 - 9.0*l1/8.0 + 9.0*r1/8.0 - r2/24.0)/dr[0];
}

vec4 yDiff4thOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float yd2 = y + (offset - 1.0)*dr[1]/dimensions3D[1];
    float yd1 = y + offset*dr[1]/dimensions3D[1];
    float yu1 = y + (offset + 1.0)*dr[1]/dimensions3D[1];
    float yu2 = y + (offset + 2.0)*dr[1]/dimensions3D[1];
    vec4 d2 = texture2D(tex, to2DTextureCoordinates(vec3(x, yd2, z)));
    vec4 d1 = texture2D(tex, to2DTextureCoordinates(vec3(x, yd1, z)));
    vec4 u1 = texture2D(tex, to2DTextureCoordinates(vec3(x, yu1, z)));
    vec4 u2 = texture2D(tex, to2DTextureCoordinates(vec3(x, yu2, z)));
    return (d2/24.0 - 9.0*d1/8.0 + 9.0*u1/8.0 - u2/24.0)/dr[1];
}

vec4 zDiff4thOrder(sampler2D tex) {
    float offset = float(sampleOffset);
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float zb2 = z + (offset - 1.0)*dr[2]/dimensions3D[2];
    float zb1 = z + offset*dr[2]/dimensions3D[2];
    float zf1 = z + (offset + 1.0)*dr[2]/dimensions3D[2];
    float zf2 = z + (offset + 2.0)*dr[2]/dimensions3D[2];
    vec4 f2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf2)));
    vec4 f1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf1)));
    vec4 b1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb1)));
    vec4 b2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb2)));
    return (b2/24.0 - 9.0*b1/8.0 + 9.0*f1/8.0 - f2/24.0)/dr[2];
}

void main() {
    vec3 DxA, DyA, DzA;
    if (orderOfAccuracy >= 4) {
        DxA = xDiff4thOrder(tex).xyz;
        DyA = yDiff4thOrder(tex).xyz;
        DzA = zDiff4thOrder(tex).xyz;
    } else {
        DxA = xDiff2ndOrder(tex).xyz;
        DyA = yDiff2ndOrder(tex).xyz;
        DzA = zDiff2ndOrder(tex).xyz;
    }
    fragColor = vec4(DyA.z - DzA.y, -DxA.z + DzA.x, DxA.y - DyA.x, 1.0);
}