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


uniform vec3 sigma;
uniform vec3 location;
uniform vec4 amplitude;
uniform sampler2D tex;

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

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

float gauss1D(float x, float x0, float sigma) {
    return exp(-0.5*(x - x0)*(x - x0)/(sigma*sigma));
}

void main() {
    vec3 xyz = to3DTextureCoordinates(UV);
    float x = xyz[0], y = xyz[1], z = xyz[2];
    float x0 = location[0], y0 = location[1], z0 = location[2];
    float width3D = float(texelDimensions3D[0]);
    float height3D = float(texelDimensions3D[1]);
    float length3D = float(texelDimensions3D[2]);
    float sx = sigma[0], sy = sigma[1], sz = sigma[2];
    float g = gauss1D(x, x0, sx)*gauss1D(y, y0, sy)*gauss1D(z, z0, sz);
    fragColor = vec4(amplitude.xyz*g + texture2D(tex, UV).xyz, 0.0);
    // if (x > 1.0/width3D && x < 1.0 - 1.0/width3D &&
    //     y > 1.0/height3D && y < 1.0 - 1.0/height3D &&
    //     z > 1.0/length3D && z < 1.0 - 1.0/length3D) {
    //     fragColor = amplitude*g + texture2D(tex, UV);
    // } else {
    //     fragColor = vec4(0.0);
    // }
}