#VERSION_NUMBER_PLACEHOLDER

precision highp float;

#if __VERSION__ >= 300
in vec4 inputData;
out vec4 COLOUR;
#define texture2D texture
#else
attribute vec4 inputData;
attribute vec4 COLOUR;
#endif

uniform float scale;
uniform vec4 rotation;
uniform float vecScale;

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

uniform sampler2D vecTex;
uniform sampler2D colTex;


vec4 quaternionMultiply(vec4 q1, vec4 q2) {
    vec4 q3;
    q3.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    q3.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y; 
    q3.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z; 
    q3.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
    return q3; 
}

vec4 quaternionConjugate(vec4 r) {
    return vec4(-r.x, -r.y, -r.z, r.w);
}

vec4 rotate(vec4 x, vec4 r) {
    vec4 xr = quaternionMultiply(x, r);
    vec4 rInv = quaternionConjugate(r);
    vec4 x2 = quaternionMultiply(rInv, xr);
    x2.w = 1.0;
    return x2; 
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
    COLOUR = texture2D(colTex, inputData.xy);
    vec3 r1 = to3DTextureCoordinates(inputData.xy) - vec3(0.5, 0.5, 0.5);
    vec3 v1 = texture2D(vecTex, inputData.xy).xyz;
    vec3 v2 = (length(v1) > 0.1)? normalize(v1): v1; 
    vec3 r2 = scale*rotate(vec4(r1 + v2*inputData.w*vecScale, 1.0),
                           rotation).xyz;
    gl_Position = vec4(r2, 1.0);

}