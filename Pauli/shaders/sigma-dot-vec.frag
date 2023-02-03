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

/* The type hmat2 is supposed to represent a two by two
Hermitian matrix. The first two components are the diagonals.
The last two components store the complex off-diagonal element
that's in the first row.*/
#define hmat2 vec4

uniform sampler2D tex;
uniform hmat2 sigmaX;
uniform hmat2 sigmaY;
uniform hmat2 sigmaZ;



void main() {
    vec3 v = texture2D(tex, UV).xyz;
    fragColor = v.x*sigmaX + v.y*sigmaY + v.z*sigmaZ;
}