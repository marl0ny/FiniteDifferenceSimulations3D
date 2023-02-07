# Numerical solutions to the Pauli equation in 3D

This simulation shows a particle interacting with a uniform magnetic field. Currently only 2D slices are displayed, as a full 3D render has yet to be implemented. Use the up and down key to change which slice to display.

## Method used

 The Pauli equation is numerically integrated explicitly by using a second order accurate centred difference approximation in time and a fourth order centred difference for each of the spatial derivatives.

## References

Numerical method (centred second order difference in time)

 - Visscher, P. (1991). A fast explicit algorithm for the time‐dependent Schrödinger equation. <em>Computers in Physics, 5</em>, 596-598. [https://doi.org/10.1063/1.168415](https://doi.org/10.1063/1.168415)

- Ira Moxley III, F. (2013). [Generalized finite-difference time-domain schemes for solving nonlinear Schrödinger equations](https://digitalcommons.latech.edu/cgi/viewcontent.cgi?article=1284&context=dissertations). <em>Dissertation</em>, 290. 

Pauli equation

 - Wikipedia contributors. (2022, August 5). [Pauli equation](https://en.wikipedia.org/wiki/Pauli_equation). In <em>Wikipedia, The Free Encyclopedia</em>.

 - Shankar, R. (1994). Spin. In <em>Principles of Quantum Mechanics</em>, chapter 14. Springer.

 - Shankar, R. (1994). The Dirac Equation. In <em>Principles of Quantum Mechanics</em>, chapter 20. Springer.

Finite difference stencils

 - Fornberg, B. (1988). Generation of Finite Difference Formulas on Arbitrarily Spaced Grids. <em>Mathematics of Computation, 51(184)</em>, 699-706. [https://doi.org/10.1090/S0025-5718-1988-0935077-0 ](https://doi.org/10.1090/S0025-5718-1988-0935077-0 )
