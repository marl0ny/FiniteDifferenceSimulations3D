#VERSION_NUMBER_PLACEHOLDER

precision highp float;

#if __VERSION__ >= 300
in vec4 COLOUR;
out vec4 fragColor;
#define texture2D texture
#else
#define fragColor gl_FragColor
varying highp vec4 COLOUR;
#endif


void main() {
    if (COLOUR.a <= 0.01) discard;
    fragColor = COLOUR;
}