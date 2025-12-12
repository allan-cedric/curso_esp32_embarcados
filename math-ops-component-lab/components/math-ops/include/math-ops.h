#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <stdbool.h>

typedef struct Math_Ops_t {
    int a;
    int b;
} Math_Ops_t;

// initialize Math_Ops_t struct
void math_ops_init(Math_Ops_t *mops, int a, int b);

// sum two numbers: a + b
int sum(Math_Ops_t *mops);

// subtract two numbers: a - b (ab == true) or b - a (ab == false)
int sub(Math_Ops_t *mops, bool ab);

// multiply two numbers: a * b
int mult(Math_Ops_t *mops);

// divide two numbers: a / b (ab == true) or b / a (ab == false)
int divi(Math_Ops_t *mops, bool ab);

#endif