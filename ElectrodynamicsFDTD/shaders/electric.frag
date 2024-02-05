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

/*
The names of variables follow the convention used on page
68 - 69 of the Computational Electrodynamics book by
Taflove A. and Hagness S. The Finite difference discretization of 
Maxwell's equation is based on eq. 3.7 and 3.8 of the same book (pg. 69),
and a basic introduction to the Yee algorithm is found on pg 78 to 79.
*/

uniform float dt; // timestep
uniform float epsilon; // electrical permittivity
uniform float sigma; // electric conductivity
uniform sampler2D texCurlHField;
uniform sampler2D texEField; // E field at previous step
uniform sampler2D texJ; // Current


void main() {
    vec3 curlH = texture2D(texCurlHField, UV).xyz;
    vec3 J = texture2D(texJ, UV).xyz;
    vec3 E = texture2D(texEField, UV).xyz;
    float eps = epsilon;
    fragColor = vec4((dt*curlH/eps - dt*J/eps + (1.0 - 0.5*sigma*dt/eps)*E)/
                     (1.0 + 0.5*sigma*dt/eps), 1.0);
}