# 3D Dirac Simulation

This [simulation](https://marl0ny.github.io/FiniteDifferenceSimulations3D/Dirac/index.html) numerically solves the single particle Dirac equation in 3D with periodic boundary conditions, either in free space or with arbitrary four vector potentials. Currently only 2D slices or a rudimentary 3D vector field view have been implemented, and a proper 3D render has yet to be made.

### Method Used

The Dirac equation is numerically integrated using the third order accurate in time Split operator method with Strang splitting.

### References

Split Operator Method:

- James Schloss. [The Split-Operator Method](https://www.algorithm-archive.org/contents/split-operator_method/split-operator_method.html). In <em>The Arcane Algorithm Archive</em>.

- Wikipedia contributors. (2023, January 25). [Split-step method](https://en.wikipedia.org/wiki/Split-step_method). In <em>Wikipedia, The Free Encyclopedia</em>.

 - Bauke, H., Keitel, C. (2011). Accelerating the Fourier split operator method via graphics processing units. <em>Computer Physics Communications, 182(12)</em>, 2454-2463. [https://doi.org/10.1016/j.cpc.2011.07.003](https://doi.org/10.1016/j.cpc.2011.07.003)

Spin

- Shankar, R. (1994). Spin. In <em>Principles of Quantum Mechanics</em>, chapter 14. Springer.

 - Wikipedia contributors. (2023, March 6). [Pauli matrices](https://en.wikipedia.org/wiki/Pauli_matrices). In <em>Wikipedia, The Free Encyclopedia</em>.

 Dirac Equation:

 - Wikipedia contributors. (2023, February 6). [Dirac equation](https://en.wikipedia.org/wiki/Dirac_equation). In <em>Wikipedia, The Free Encyclopedia</em>.

 - Wikipedia contributors. (2023, February 25). [Dirac spinor](https://en.wikipedia.org/wiki/Dirac_spinor). In <em>Wikipedia, The Free Encyclopedia</em>.

 - Shankar, R. (1994). The Dirac Equation. In <em>Principles of Quantum Mechanics</em>, chapter 20. Springer.

Fast Fourier Transform:

- Press W. et al. (1992). Fast Fourier Transform.
In <em>[Numerical Recipes in Fortran 77](https://websites.pmc.ucsc.edu/~fnimmo/eart290c_17/NumericalRecipesinF77.pdf)</em>, chapter 12.

- Wikipedia contributors. (2021, October 8). [Cooley–Tukey FFT algorithm](https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm). In <em>Wikipedia, The Free Encyclopedia</em>.

- Weisstein, E. (2021). [Fast Fourier Transform](https://mathworld.wolfram.com/FastFourierTransform.html). In <em>Wolfram MathWorld</em>.
