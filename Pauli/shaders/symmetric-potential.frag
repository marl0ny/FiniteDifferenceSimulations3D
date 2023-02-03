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

uniform float q;
uniform float hbar;
uniform float m;
uniform float c;
uniform float k;
uniform vec3 direction;
uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;
uniform vec3 dimensions3D;


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
    float x = (uvw[0] - 0.5)*dimensions3D[0];
    float y = (uvw[1] - 0.5)*dimensions3D[1];
    float z = (uvw[2] - 0.5)*dimensions3D[2];
    vec3 d = direction;
    vec3 vecPot = k*(d[0]*vec3(0.0, -z, y)
                     + d[1]*vec3(z, 0.0, -x)
                     + d[2]*vec3(-y, x, 0.0));
    fragColor = vec4(vecPot, (q*q)*dot(vecPot, vecPot)/(2.0*m*c*c));

}