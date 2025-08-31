#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif

uniform float dt;

uniform ivec2 texelDimensions2D;
uniform ivec3 texelDimensions3D;
uniform vec3 dimensions3D;

uniform sampler2D forwardTex;
uniform sampler2D reverseTex;
uniform sampler2D initialTex;

uniform sampler2D velocityTex;


vec2 to2DTextureCoordinates(vec3 uvw) {
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    float wStack = float(width2D)/float(width3D);
    float xIndex = float(width3D)*mod(uvw[0], 1.0);
    float yIndex = float(height3D)*mod(uvw[1], 1.0);
    float zIndex = mod(floor(float(length3D)*uvw[2]), float(length3D));
    float uIndex = mod(zIndex, wStack)*float(width3D) + xIndex; 
    float vIndex = floor(zIndex / wStack)*float(height3D) + yIndex; 
    return vec2(uIndex/float(width2D), vIndex/float(height2D));
}

vec3 to3DTextureCoordinates(vec2 uv) {
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    float wStack = float(width2D)/float(width3D);
    float hStack = float(height2D)/float(height3D);
    float u = mod(uv[0]*wStack, 1.0);
    float v = mod(uv[1]*hStack, 1.0);
    float w = (floor(uv[1]*hStack)*wStack
               + floor(uv[0]*wStack) + 0.5)/float(length3D);
    return vec3(u, v, w);
}

vec4 clamp8(vec4 val,
            vec4 c000, vec4 c100, vec4 c010, vec4 c110,
            vec4 c001, vec4 c101, vec4 c011, vec4 c111) {
    vec4 maxC = max(c000, max(c100, max(c010, max(c110, 
        max(c001, max(c101, max(c011, c111)))))));
    vec4 minC = min(c000, min(c100, min(c010, min(c110, 
        min(c001, min(c101, min(c011, c111)))))));
    return clamp(val, minC, maxC);
}

vec4 getSurroundingTexelValue(vec3 r, bvec3 offset) {
    float w = float(texelDimensions3D[0]);
    float h = float(texelDimensions3D[1]);
    float d = float(texelDimensions3D[2]);
    vec3 rOffset = vec3(
        (((offset[0])? ceil(r.x*w - 0.5): floor(r.x*w - 0.5)) + 0.5)/w,
        (((offset[1])? ceil(r.y*h - 0.5): floor(r.y*h - 0.5)) + 0.5)/h,
        (((offset[2])? ceil(r.z*d - 0.5): floor(r.z*d - 0.5)) + 0.5)/d
    );
    return texture2D(initialTex, to2DTextureCoordinates(rOffset));
}

vec4 limiter(vec4 v) {
    vec3 velocity = texture2D(velocityTex, UV).xyz;
    vec3 uvw = to3DTextureCoordinates(UV);
    vec3 r = uvw - (velocity/dimensions3D)*dt;
    // if (r.x < 0.0 || r.x > 1.0 || r.y < 0.0 || r.y > 1.0) {
    //     fragColor = vec4(0.0);
    //     return;
    // }
    vec4 v000 = getSurroundingTexelValue(r, bvec3(0, 0, 0));
    vec4 v100 = getSurroundingTexelValue(r, bvec3(1, 0, 0));
    vec4 v010 = getSurroundingTexelValue(r, bvec3(0, 1, 0));
    vec4 v110 = getSurroundingTexelValue(r, bvec3(1, 1, 0));
    vec4 v001 = getSurroundingTexelValue(r, bvec3(0, 0, 1));
    vec4 v101 = getSurroundingTexelValue(r, bvec3(1, 0, 1));
    vec4 v011 = getSurroundingTexelValue(r, bvec3(0, 1, 1));
    vec4 v111 = getSurroundingTexelValue(r, bvec3(1, 1, 1));
    return clamp8(v, v000, v100, v010, v110, v001, v101, v011, v111);
}
 
void main() {
    vec4 initial = texture2D(initialTex, UV);
    vec4 forward = texture2D(forwardTex, UV);
    vec4 reverse = texture2D(reverseTex, UV);
    fragColor = limiter(forward + (initial - reverse)/2.0);
}

