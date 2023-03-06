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

#define complex vec2
#define complex2 vec4

uniform sampler2D tex1;
uniform sampler2D tex2;


complex conj(complex z) {
    return complex(z[0], -z[1]);
}

complex multiply(complex w, complex z) {
    return complex(w[0]*z[0] - w[1]*z[1], w[0]*z[1] + w[1]*z[0]);
}

void main() {
    hmat2 matrix = texture2D(tex1, UV);
    complex matrix00 = complex(matrix[0], 0.0);
    complex matrix11 = complex(matrix[1], 0.0);
    complex matrix01 = complex(matrix[2], matrix[3]);
    complex matrix10 = conj(matrix01);
    complex2 v = texture2D(tex2, UV);
    complex v0 = complex(v[0], v[1]);
    complex v1 = complex(v[2], v[3]);
    fragColor = complex2(multiply(matrix00, v0) + multiply(matrix01, v1),
                         multiply(matrix10, v0) + multiply(matrix11, v1));
}