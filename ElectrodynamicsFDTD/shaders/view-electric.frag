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

uniform int component;
uniform sampler2D tex;

void main() {
    float e = texture2D(tex, UV)[component];
    fragColor = vec4(e, 0.0, -e, 1.0);
}
