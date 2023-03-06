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


uniform sampler2D uTex;
uniform sampler2D vTex;
uniform sampler2D potentialTex;
uniform float dt;
uniform float m;
uniform float c;
uniform float hbar;

uniform int spinorIndex;

const int TOP = 0;
const int BOTTOM = 1;

#define complex vec2
#define complex2 vec4

const float SQRT_2 = 1.4142135623730951;

complex mult(complex z1, complex z2) {
    return complex(z1.x*z2.x - z1.y*z2.y, 
                   z1.x*z2.y + z1.y*z2.x);
}

complex mul(complex z1, complex z2) {
    return mult(z1, z2);
}

complex2 c1C2(complex z, complex2 z2) {
    complex a = complex(z2[0], z2[1]);
    complex b = complex(z2[2], z2[3]);
    return complex2(complex(z.x*a.x - z.y*a.y, z.x*a.y + z.y*a.x),
                    complex(z.x*b.x - z.y*b.y, z.x*b.y + z.y*b.x));
}

complex conj(complex z) {
    return complex(z.x, -z.y);
}

complex innerProd(complex2 z1, complex2 z2) {
    return mul(conj(z1.rg), z2.rg) + mul(conj(z1.ba), z2.ba);
}

complex frac(complex z1, complex z2) {
    complex invZ2 = conj(z2)/(z2.x*z2.x + z2.y*z2.y);
    return mul(z1, invZ2);
}

complex complexExp(complex z) {
    return complex(exp(z.x)*cos(z.y), exp(z.x)*sin(z.y));
}

/*
Matrix([[(-n + nz)/((nx + I*ny)*sqrt((-n + nz)**2/(nx**2 + ny**2) + 1.0)),
         (n + nz)/((nx + I*ny)*sqrt((n + nz)**2/(nx**2 + ny**2) + 1.0))],
        [1.0/sqrt((-n + nz)**2/(nx**2 + ny**2) + 1.0),
         1.0/sqrt((n + nz)**2/(nx**2 + ny**2) + 1.0)]])
*/

complex2 getNegativeSpinEigenstate(vec3 orientation, float len) {
    float n = len;
    float nx = orientation.x, ny = orientation.y, nz = orientation.z;
    complex az = complex(0.0, 0.0);
    complex bz = complex(1.0, 0.0);
    complex a = frac(complex(-n + nz, 0.0),
                     complex(nx, ny)*sqrt((nz - n)*(nz - n)/(nx*nx + ny*ny)
                                          + 1.0));
    complex b = complex(1.0/sqrt((nz - n)*(nz - n)/(nx*nx + ny*ny) + 1.0),
                        0.0);
    if ((nx*nx + ny*ny) == 0.0) return complex2(az, bz);
    return complex2(a, b);
}

complex2 getPositiveSpinEigenstate(vec3 orientation, float len) {
    float n = len;
    float nx = orientation.x, ny = orientation.y, nz = orientation.z;
    complex az = complex(1.0, 0.0);
    complex bz = complex(0.0, 0.0);
    complex a = frac(complex(n + nz, 0.0),
                     complex(nx, ny)*sqrt((nz + n)*(nz + n)/(nx*nx + ny*ny)
                                          + 1.0));
    complex b = complex(1.0/sqrt((nz + n)*(nz + n)/(nx*nx + ny*ny) + 1.0),
                        0.0);
    if ((nx*nx + ny*ny) == 0.0) return complex2(az, bz);
    return complex2(a, b);
}

void main() {
    // Potential
    vec4 potential = texture2D(potentialTex, UV);
    float arg = -0.5*c*potential.w*dt/hbar;
    complex expV = complex(cos(arg), sin(arg));
    // 3 vector potential
    vec3 vecPot = potential.xyz;
    float vx = vecPot.x, vy = vecPot.y, vz = vecPot.z;
    float v2 = vx*vx + vy*vy + vz*vz;
    float v = sqrt(v2);
    // Wave function
    complex2 s01 = texture2D(uTex, UV);
    complex2 s23 = texture2D(vTex, UV);

    complex2 posEig = getPositiveSpinEigenstate(vecPot, v);
    complex2 negEig = getNegativeSpinEigenstate(vecPot, v);

    complex c0 = (innerProd(posEig, s01) + innerProd(posEig, s23))/SQRT_2;
    complex c1 = (innerProd(negEig, s01) - innerProd(negEig, s23))/SQRT_2;
    complex c2 = (innerProd(negEig, s01) + innerProd(negEig, s23))/SQRT_2;
    complex c3 = (innerProd(posEig, s01) - innerProd(posEig, s23))/SQRT_2;
    complex eP = complex(cos(c*v*dt/hbar), sin(c*v*dt/hbar)); 
    complex eN = complex(cos(c*v*dt/hbar), -sin(c*v*dt/hbar));
    complex e0 = mult(c0, eP);
    complex e1 = mult(c1, eP);
    complex e2 = mult(c2, eN);
    complex e3 = mult(c3, eN);
    if (v != 0.0) {
        s01 = c1C2(e0, posEig/SQRT_2) + c1C2(e1, negEig/SQRT_2)
                + c1C2(e2, negEig/SQRT_2) + c1C2(e3, posEig/SQRT_2);
        s23 = c1C2(e0, posEig/SQRT_2) + c1C2(e1, -negEig/SQRT_2)
                + c1C2(e2, negEig/SQRT_2) + c1C2(e3, -posEig/SQRT_2);
    }
    fragColor = (spinorIndex == TOP)? c1C2(expV, s01): c1C2(expV, s23);
}
