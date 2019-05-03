#pragma once

#define DELTA_T 0.016
#define SOLVER_ITERATIONS 5

#define KERNEL_H 0.25
#define REST_DENSITY 450000

#define POLY_6 (315 / (64 * PI * pow(KERNEL_H, 9)))
#define SPIKY_GRADIENT (-45 / (PI * pow(KERNEL_H, 6)))

#define CFM_EPSILON 0.1
#define DRAG_COEFF 1

#define TENSILE_DELTA_Q (0.2 * KERNEL_H)
#define TENSILE_K 0.2
#define TENSILE_N 4

#define VISCOSITY_C 0.01
#define VORTICITY_EPSILON 0.0006

#define SPIKY_DAMPING 1