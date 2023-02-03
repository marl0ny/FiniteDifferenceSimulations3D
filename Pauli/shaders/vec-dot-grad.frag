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

#define complex2 vec4

uniform complex2 scale;
uniform sampler2D tex1;
uniform sampler2D tex2;

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

vec4 centredXDiff4thOrder(sampler2D tex) {
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float xl2 = x - 2.0*dr[0]/dimensions3D[0];
    float xl1 = x - dr[0]/dimensions3D[0];
    float xr1 = x + dr[0]/dimensions3D[0];
    float xr2 = x + 2.0*dr[0]/dimensions3D[0];
    vec4 l2 = texture2D(tex, to2DTextureCoordinates(vec3(xl2, y, z)));
    vec4 l1 = texture2D(tex, to2DTextureCoordinates(vec3(xl1, y, z)));
    vec4 c0 = texture2D(tex, UV);
    vec4 r1 = texture2D(tex, to2DTextureCoordinates(vec3(xr1, y, z)));
    vec4 r2 = texture2D(tex, to2DTextureCoordinates(vec3(xr2, y, z)));
    return (l2/12.0 - 2.0*l1/3.0 + 2.0*r1/3.0 - r2/12.0)/dr[0];
}

vec4 centredYDiff4thOrder(sampler2D tex) {
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float yd2 = y - 2.0*dr[1]/dimensions3D[1];
    float yd1 = y - dr[1]/dimensions3D[1];
    float yu1 = y + dr[1]/dimensions3D[1];
    float yu2 = y + 2.0*dr[1]/dimensions3D[1];
    vec4 d2 = texture2D(tex, to2DTextureCoordinates(vec3(x, yd2, z)));
    vec4 d1 = texture2D(tex, to2DTextureCoordinates(vec3(x, yd1, z)));
    vec4 u1 = texture2D(tex, to2DTextureCoordinates(vec3(x, yu1, z)));
    vec4 u2 = texture2D(tex, to2DTextureCoordinates(vec3(x, yu2, z)));
    return (d2/12.0 - 2.0*d1/3.0 + 2.0*u1/3.0 - u2/12.0)/dr[1];
}

vec4 centredZDiff4thOrder(sampler2D tex) {
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float zf1 = z + dr[2]/dimensions3D[2];
    float zf2 = z + 2.0*dr[2]/dimensions3D[2];
    float zb1 = z - dr[2]/dimensions3D[2];
    float zb2 = z - 2.0*dr[2]/dimensions3D[2];
    vec4 f2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf2)));
    vec4 f1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf1)));
    vec4 b1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb1)));
    vec4 b2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb2)));
    return (b2/12.0 - 2.0*b1/3.0 + 2.0*f1/3.0 - f2/12.0)/dr[2];
}

complex2 multiply(complex2 w, complex2 z) {
    return complex2(w[0]*z[0] - w[1]*z[1], w[0]*z[1] + w[1]*z[0],
                    w[2]*z[2] - w[3]*z[3], w[2]*z[3] + w[3]*z[2]);
}

void main() {
    vec3 v = texture2D(tex1, UV).xyz;
    complex2 gradXPsi = centredXDiff4thOrder(tex2);
    complex2 gradYPsi = centredYDiff4thOrder(tex2);
    complex2 gradZPsi = centredZDiff4thOrder(tex2);
    fragColor = multiply(scale, v.x*gradXPsi + v.y*gradYPsi + v.z*gradZPsi);
}