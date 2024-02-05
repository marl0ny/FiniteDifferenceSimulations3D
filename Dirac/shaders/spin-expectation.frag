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

uniform sampler2D tex;
uniform hmat2 sigmaX;
uniform hmat2 sigmaY;
uniform hmat2 sigmaZ;

complex conj(complex z) {
    return complex(z[0], -z[1]);
}

complex multiply(complex w, complex z) {
    return complex(w[0]*z[0] - w[1]*z[1], w[0]*z[1] + w[1]*z[0]);
}

complex firstComponent(complex2 z) {
    return complex(z[0], z[1]);
}

complex secondComponent(complex2 z) {
    return complex(z[2], z[3]);
}

complex2 matrixMultiply(hmat2 m, complex2 v) {
    complex m00 = complex(m[0], 0.0);
    complex m11 = complex(m[1], 0.0);
    complex m01 = complex(m[2], m[3]);
    complex m10 = conj(m01);
    complex v0 = firstComponent(v);
    complex v1 = secondComponent(v);
    return complex2(multiply(m00, v0) + multiply(m01, v1),
                    multiply(m10, v0) + multiply(m11, v1));
}

complex innerProduct(complex2 w, complex2 z) {
    return multiply(conj(firstComponent(w)), firstComponent(z))
            + multiply(conj(secondComponent(w)), secondComponent(z));
}

float expectationValue(hmat2 operator, complex2 state) {
    return innerProduct(state, matrixMultiply(operator, state))[0];
}

void main() {
    complex2 state = texture2D(tex, UV);
    fragColor = vec4(expectationValue(sigmaX, state), 
                     expectationValue(sigmaY, state),
                     expectationValue(sigmaZ, state), 1.0);
}



