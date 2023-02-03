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
uniform mat4 matrix;


void main() {
    /* In C, all matrix operations are expressed in row order.
    Now consider the matrix equation b = M a. In glsl this is converted
    to column order so that b.T = a.T M.T*/
    fragColor = texture2D(tex, UV)*matrix;
}