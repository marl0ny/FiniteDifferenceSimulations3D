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

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D laplacianTex1;

uniform float c;
uniform float dt;

void main() {
    vec4 wave0 = texture2D(tex0, UV);
    vec4 wave1 = texture2D(tex1, UV);
    vec4 laplacianWave1 = texture2D(laplacianTex1, UV);
    fragColor = c*c*dt*dt*laplacianWave1 + 2.0*wave1 - wave0;
}


