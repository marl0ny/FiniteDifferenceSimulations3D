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


uniform ivec2 outDimensions;
uniform ivec2 texDimensions;
uniform vec2 offset;
uniform sampler2D tex;

void main() {
    float mx = float(outDimensions[0])/float(texDimensions[0]);
    float my = float(outDimensions[1])/float(texDimensions[1]);
    vec2 uv2 = vec2(mx*UV[0] + offset[0],
                    my*UV[1] + offset[1]);
    fragColor = texture2D(tex, uv2);
}