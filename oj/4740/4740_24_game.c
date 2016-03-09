#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int numerator;
    int denominator;
} fraction_t;
typedef fraction_t *fraction_ptr_t;

void init_fraction(fraction_ptr_t fp, int n, int d)
{
    fp->numerator = n;
    fp->denominator = d;
}

void add_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    res->numerator = lhs->numerator * rhs->denominator + lhs->denominator * rhs->numerator;
    res->denominator = lhs->denominator * rhs->denominator;
}

void sub_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    res->numerator = lhs->numerator * rhs->denominator - lhs->denominator * rhs->numerator;
    res->denominator = lhs->denominator *rhs->denominator;
}

void mul_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    res->numerator = lhs->numerator * rhs->numerator;
    res->denominator = lhs->denominator * rhs->denominator;
}

void div_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    res->numerator = lhs->numerator * rhs->denominator;
    res->denominator = lhs->denominator * rhs->numerator;
}

void rsub_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    sub_fraction(rhs, lhs, res);
}

void rdiv_fraction(const fraction_ptr_t lhs, const fraction_ptr_t rhs, fraction_ptr_t res)
{
    div_fraction(rhs, lhs, res);
}

typedef void (*fraction_op_t)(const fraction_ptr_t, const fraction_ptr_t, fraction_ptr_t);
fraction_op_t fraction_ops[] = {
    add_fraction,
    sub_fraction,
    mul_fraction,
    div_fraction,
    rsub_fraction,
    rdiv_fraction
};

bool dfs(fraction_t fps[], int len) {
    if (len == 1) {
        return fps->numerator == fps->denominator * 24 && fps->denominator;
    }
    int i, j, k;
    for (i = 1; i < len; ++i) {
        fraction_t save = fps[i];
        for (j = 0; j < i; ++j) {
            for (k = 0; k < sizeof(fraction_ops)/sizeof(fraction_op_t); ++k) {
                fraction_ops[k](&save, fps+j, fps+i);
                if (dfs(fps+j+1, len-1))
                    return true;
            }
        }
        fps[i] = save;
    }
    return false;
}

bool Game24Points(int a, int b, int c, int d)
{
    fraction_t fractions[4];
    init_fraction(fractions + 0, a, 1);
    init_fraction(fractions + 1, b, 1);
    init_fraction(fractions + 2, c, 1);
    init_fraction(fractions + 3, d, 1);
    return dfs(fractions, 4);
}

int main(void)
{
    printf("%d\n", Game24Points(7, 2, 1, 10));
    printf("%d\n", Game24Points(7, 2, 2, 10));
    return 0;
}
