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

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;
uniform vec3 dimensions3D;

uniform sampler2D uTex;
uniform sampler2D vTex;
uniform float dt;
uniform float m;
uniform float c;
uniform float hbar;

uniform int spinorIndex;
const int TOP = 0;
const int BOTTOM = 1;

#define complex vec2
#define complex2 vec4

const float PI = 3.141592653589793;

vec3 to3DTextureCoordinates(vec2 uv) {
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    float wStack = float(width2D)/float(width3D);
    float hStack = float(height2D)/float(height3D);
    float wIndex = floor(uv[1]*hStack)*wStack + floor(uv[0]*wStack);
    return vec3(mod(uv[0]*wStack, 1.0), mod(uv[1]*hStack, 1.0),
                (wIndex + 0.5)/float(length3D));
}

complex mul(complex z1, complex z2) {
    return complex(z1.x*z2.x - z1.y*z2.y, 
                   z1.x*z2.y + z1.y*z2.x);
}

complex conj(complex z) {
    return vec2(z.x, -z.y);
}

vec3 getMomentum() {
    vec3 uvw = to3DTextureCoordinates(UV);
    float u = uvw[0], v = uvw[1], w = uvw[2];
    float freqU = ((u < 0.5)? u: -1.0 + u)*float(texelDimensions3D[0]) - 0.5;
    float freqV = ((v < 0.5)? v: -1.0 + v)*float(texelDimensions3D[1]) - 0.5;
    float freqW = ((w < 0.5)? w: -1.0 + w)*float(texelDimensions3D[2]) - 0.5;
    float width = dimensions3D[0];
    float height = dimensions3D[1];
    float length_ = dimensions3D[2];
    if (freqU == 0.0) freqU += 5e-8;
    if (freqV == 0.0) freqV += 5e-8;
    if (freqW == 0.0) freqW += 5e-8;
    return vec3(2.0*PI*freqU/width, 2.0*PI*freqV/height, 
                2.0*PI*freqW/length_);
}

void main() {

    vec3 pVector = getMomentum();
    float px = pVector.x, py = pVector.y, pz = pVector.z;
    float p2 = px*px + py*py + pz*pz;
    float p = sqrt(p2);
    float mc = m*c;
    float omega = sqrt(mc*mc + p2);
    float den1 = p*sqrt((mc - omega)*(mc - omega) + p2);
    float den2 = p*sqrt((mc + omega)*(mc + omega) + p2);

    // The matrix U for the momentum step, where U e^{E} U^{\dagger}.
    // This is found by diagonalizing the matrix involving the mass
    // and momentum terms using a computer algebra system like Sympy,
    // which can be expressed as  U E inv(U), where E is the diagonal
    // matrix of eigenvalues and U is the matrix of eigenvectors. 
    // This is following what is similarly done in II.3 of this
    // article by Bauke and Keitel:
    // https://arxiv.org/abs/1012.3911

    float   matUDag00 = pz*(mc - omega)/den1;
    complex matUDag01 = complex(px, -py)*(mc - omega)/den1;
    float   matUDag02 = p2/den1;
    float   matUDag03 = 0.0;
    complex matUDag10 = complex(px, py)*(mc - omega)/den1;
    float   matUDag11 = -pz*(mc - omega)/den1;
    float   matUDag12 = 0.0;
    float   matUDag13 = p2/den1;
    float   matUDag20 = pz*(mc + omega)/den2;
    complex matUDag21 = complex(px, -py)*(mc + omega)/den2;
    float   matUDag22 = p2/den2;
    float   matUDag23 = 0.0;
    complex matUDag30 = complex(px, py)*(mc + omega)/den2;
    float   matUDag31 = -pz*(mc + omega)/den2;
    float   matUDag32 = 0.0;
    float   matUDag33 = p2/den2;

    float   matU00 = matUDag00;
    complex matU01 = conj(matUDag10);
    float   matU02 = matUDag20;
    complex matU03 = conj(matUDag30);
    complex matU10 = conj(matUDag01);
    float   matU11 = matUDag11;
    complex matU12 = conj(matUDag21);
    float   matU13 = matUDag31;
    float   matU20 = matUDag02;
    float   matU21 = matUDag12;
    float   matU22 = matUDag22;
    float   matU23 = matUDag32;
    float   matU30 = matUDag03;
    float   matU31 = matUDag13;
    float   matU32 = matUDag23;
    float   matU33 = matUDag33;
    
    complex2 u = texture2D(uTex, UV);
    complex2 v = texture2D(vTex, UV);
    complex psi0 = u.xy;
    complex psi1 = u.zw;
    complex psi2 = v.xy;
    complex psi3 = v.zw;

    float cos_val = cos(omega*c*dt/hbar);
    float sin_val = sin(omega*c*dt/hbar); 
    complex e1 = complex(cos_val, sin_val); 
    complex e2 = complex(cos_val, -sin_val);

    complex phi0 = matUDag00*psi0 + mul(matUDag01, psi1)
                     + matUDag02*psi2 + matUDag03*psi3;
    complex phi1 = mul(matUDag10, psi0) + matUDag11*psi1
                     + matUDag12*psi2 + matUDag13*psi3;
    complex phi2 = matUDag20*psi0 + mul(matUDag21, psi1)
                     + matUDag22*psi2 + matUDag23*psi3;
    complex phi3 = mul(matUDag30, psi0) + matUDag31*psi1
                     + matUDag32*psi2 + matUDag33*psi3;
    
    complex e1Phi0 = mul(e1, phi0);
    complex e1Phi1 = mul(e1, phi1);
    complex e2Phi2 = mul(e2, phi2);
    complex e2Phi3 = mul(e2, phi3);

    psi0 = matU00*e1Phi0 + mul(matU01, e1Phi1)
             + matU02*e2Phi2 + mul(matU03, e2Phi3);
    psi1 = mul(matU10, e1Phi0) + matU11*e1Phi1
             + mul(matU12, e2Phi2) + matU13*e2Phi3;
    psi2 = matU20*e1Phi0 + matU21*e1Phi1
             + matU22*e2Phi2 + matU23*e2Phi3;
    psi3 = matU30*e1Phi0 + matU31*e1Phi1
             + matU32*e2Phi2 + matU33*e2Phi3;
    
    complex2 psi01 = complex2(psi0, psi1);
    complex2 psi23 = complex2(psi2, psi3);

    if (spinorIndex == TOP) {
        // float angle = dt*p2/(2.0*m*hbar);
        // fragColor = complex2(mul(u.xy, complex(cos(angle), -sin(angle))),
        //                       mul(u.zw, complex(cos(angle), -sin(angle))));
        fragColor = psi01;
    } else if (spinorIndex == BOTTOM) {
        // float angle = dt*p2/(2.0*m*hbar);
        // fragColor = complex2(mul(v.xy, complex(cos(angle), -sin(angle))),
        //                      mul(v.zw, complex(cos(angle), -sin(angle))));
        fragColor = psi23;
    }

}