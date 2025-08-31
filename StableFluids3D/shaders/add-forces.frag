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
uniform sampler2D forceTex;
uniform sampler2D fluidVelocityTex;

uniform ivec2 texelDimensions2D;
uniform ivec3 texelDimensions3D;


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

void main() {
    vec4 forces = texture2D(forceTex, UV);
    vec4 velocity = texture2D(fluidVelocityTex, UV);
    fragColor = vec4(dt*forces.xyz + velocity.xyz, 0.0);
    // float width3D = float(texelDimensions3D[0]);
    // float height3D = float(texelDimensions3D[1]);
    // float length3D = float(texelDimensions3D[2]);
    // vec3 uvw = to3DTextureCoordinates(UV);
    // float u = uvw[0], v = uvw[1], w = uvw[2];
    // if (uvw[0] > 1.0/width3D && uvw[0] < 1.0 - 1.0/width3D &&
    //     uvw[1] > 1.0/height3D && uvw[1] < 1.0 - 1.0/height3D &&
    //     uvw[2] > 1.0/length3D && uvw[2] < 1.0 - 1.0/length3D) {
    //     fragColor = vec4(dt*forces.xyz + velocity.xyz, velocity.w);
    // } else {
    //     fragColor = vec4(0.0);
    // }
}