#VERSION_NUMBER_PLACEHOLDER

precision highp float;

#if __VERSION__ >= 300
in vec2 UV;
out vec4 fragColor;
#else
#define fragColor gl_FragColor
varying highp vec2 UV;
#endif


uniform int type;

#define NONE 0
#define DIRICHLET_0 1

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

uniform vec3 dimensions3D;
uniform vec3 dr;


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
    if (type == DIRICHLET_0) {
        if (uvw[0] < dr[0]/dimensions3D[0] ||
            uvw[1] < dr[1]/dimensions3D[1] ||
            uvw[2] < dr[2]/dimensions3D[2] ||
            uvw[0] > (1.0 - dr[0]/dimensions3D[0]) ||
            uvw[1] > (1.0 - dr[1]/dimensions3D[1]) ||
            uvw[2] > (1.0 - dr[2]/dimensions3D[2])) {
                fragColor = vec4(0.0, 0.0, 0.0, 0.0);
        } else {
            fragColor = vec4(1.0, 1.0, 1.0, 1.0);
        }
    } else {
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}