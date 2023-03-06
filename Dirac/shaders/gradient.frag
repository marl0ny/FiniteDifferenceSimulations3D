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
uniform sampler2D boundaryMaskTex;

uniform int index;

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

vec4 centredXDiff4thOrder(sampler2D tex) {
    int width2D = texelDimensions2D[0];
    int width3D = texelDimensions3D[0];
    float wStack = float(width2D)/float(width3D);
    vec2 dl2 = -2.0*vec2((1.0/wStack)*dr[0]/dimensions3D[0], 0.0);
    vec2 dl1 = -vec2((1.0/wStack)*dr[0]/dimensions3D[0], 0.0);
    vec2 dr1 = vec2((1.0/wStack)*dr[0]/dimensions3D[0], 0.0);
    vec2 dr2 = 2.0*vec2((1.0/wStack)*dr[0]/dimensions3D[0], 0.0);
    float bc = texture2D(boundaryMaskTex, UV)[0];
    float bl2 = texture2D(boundaryMaskTex, UV + dl2)[0];
    float bl1 = texture2D(boundaryMaskTex, UV + dl1)[0];
    float br1 = texture2D(boundaryMaskTex, UV + dr1)[0];
    float br2 = texture2D(boundaryMaskTex, UV + dr2)[0];
    vec4 l2 = bl2*texture2D(tex, UV + dl2);
    vec4 l1 = bl1*texture2D(tex, UV + dl1);
    vec4 r1 = br1*texture2D(tex, UV + dr1);
    vec4 r2 = br2*texture2D(tex, UV + dr2);
    return bc*(l2/12.0 - l1/3.0 + r1/3.0 - r2/12.0)/dr[0];
}

vec4 centredYDiff4thOrder(sampler2D tex) {
    int height2D = texelDimensions2D[1];
    int height3D = texelDimensions3D[1];
    float hStack = float(height2D)/float(height3D);
    vec2 dd2 = -2.0*vec2(0.0, (1.0/hStack)*dr[1]/dimensions3D[1]);
    vec2 dd1 = -vec2(0.0, (1.0/hStack)*dr[1]/dimensions3D[1]);
    vec2 du1 = vec2(0.0, (1.0/hStack)*dr[1]/dimensions3D[1]);
    vec2 du2 = 2.0*vec2(0.0, (1.0/hStack)*dr[1]/dimensions3D[1]);
    float bc = texture2D(boundaryMaskTex, UV)[0];
    float bd2 = texture2D(boundaryMaskTex, UV + dd2)[0];
    float bd1 = texture2D(boundaryMaskTex, UV + dd1)[0];
    float bu1 = texture2D(boundaryMaskTex, UV + du1)[0];
    float bu2 = texture2D(boundaryMaskTex, UV + du2)[0];
    vec4 d2 = bd2*texture2D(tex, UV + dd2);
    vec4 d1 = bd1*texture2D(tex, UV + dd1);
    vec4 u1 = bu1*texture2D(tex, UV + du1);
    vec4 u2 = bu2*texture2D(tex, UV + du2);
    return bc*(d2/12.0 - d1/3.0 + u1/3.0 - u2/12.0)/dr[1];
}

vec4 centredZDiff4thOrder(sampler2D tex) {
    vec3 xyz = to3DTextureCoordinates(UV).xyz;
    float x = xyz.x, y = xyz.y, z = xyz.z;
    float zf1 = z + dr[2]/dimensions3D[2];
    float zf2 = z + 2.0*dr[2]/dimensions3D[2];
    float zb1 = z - dr[2]/dimensions3D[2];
    float zb2 = z - 2.0*dr[2]/dimensions3D[2];
    float bc = texture2D(boundaryMaskTex, UV)[0];
    float bf2 = texture2D(boundaryMaskTex,
                          to2DTextureCoordinates(vec3(x, y, zf2)))[0];
    float bf1 = texture2D(boundaryMaskTex,
                          to2DTextureCoordinates(vec3(x, y, zf2)))[0];
    float bb1 = texture2D(boundaryMaskTex,
                          to2DTextureCoordinates(vec3(x, y, zb1)))[0];
    float bb2 = texture2D(boundaryMaskTex,
                          to2DTextureCoordinates(vec3(x, y, zb2)))[0];
    vec4 f2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf2)));
    vec4 f1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zf1)));
    vec4 b1 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb1)));
    vec4 b2 = texture2D(tex, to2DTextureCoordinates(vec3(x, y, zb2)));
    return bc*(b2/12.0 - b1/3.0 + f1/3.0 - f2/12.0)/dr[2];
}

void main() {
    vec4 dTexdx = centredXDiff4thOrder(tex);
    vec4 dTexdy = centredYDiff4thOrder(tex);
    vec4 dTexdz = centredZDiff4thOrder(tex);
    fragColor = vec4(dTexdx[index], dTexdy[index], dTexdz[index], 1.0); 
}