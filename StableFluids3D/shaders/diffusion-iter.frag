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

uniform ivec2 texelDimensions2D;
uniform ivec3 texelDimensions3D;
uniform vec3 dimensions3D;
uniform float diffusionCoeff;
uniform sampler2D toSolveTex;
uniform sampler2D lastIterTex;
uniform float dt;

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

void main() {
    vec3 du = vec3(1.0/float(texelDimensions3D[0]), 0.0, 0.0);
    vec3 dv = vec3(0.0, 1.0/float(texelDimensions3D[1]), 0.0);
    vec3 dw = vec3(0.0, 0.0, 1.0/float(texelDimensions3D[2]));
    float dx = dimensions3D[0]/float(texelDimensions3D[0]);
    float dy = dimensions3D[1]/float(texelDimensions3D[1]);
    float dz = dimensions3D[2]/float(texelDimensions3D[2]);
    vec3 duvw = vec3(du[0], dv[1], dw[2]);
    vec3 dxyz = vec3(dx, dy, dz);
    vec3 uvw = to3DTextureCoordinates(UV);
    vec4 toSolveVal = texture2D(toSolveTex, UV);
    vec4 vL, vR, vU, vD, vF, vB;
    vL = texture2D(lastIterTex, to2DTextureCoordinates(uvw - du));
    vR = texture2D(lastIterTex, to2DTextureCoordinates(uvw + du));
    vU = texture2D(lastIterTex, to2DTextureCoordinates(uvw + dv));
    vD = texture2D(lastIterTex, to2DTextureCoordinates(uvw - dv));
    vF = texture2D(lastIterTex, to2DTextureCoordinates(uvw + dw));
    vB = texture2D(lastIterTex, to2DTextureCoordinates(uvw - dw));
    vec4 num = toSolveVal + dt*diffusionCoeff*(
        (vL + vR)/(dx*dx) + (vU + vD)/(dy*dy) + (vF + vB)/(dz*dz));
    float den = 1.0 
        + 2.0*dt*diffusionCoeff*(1.0/(dx*dx) + 1.0/(dy*dy) + 1.0/(dz*dz));
    fragColor = num/den;

}