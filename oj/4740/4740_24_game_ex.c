#include <stdio.h>
#include <stdbool.h>
#include <math.h>

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

void swap(fraction_ptr_t a, fraction_ptr_t b)
{
    fraction_t tmp = *a;
    *a = *b;
    *b = tmp;
}

bool dfs(fraction_t fps[], int len, double *res) {
    if (len == 1) {
        if (fps->numerator == fps->denominator * 24 && fps->denominator) {
            *res = 24.0;
            return true;
        }
        if (fps->denominator) {
            double tmp = 1.0 * fps->numerator / fps->denominator;
            if (fabs(tmp-24.0) < fabs(*res-24.0))
                *res = tmp;
        }
        return false;
    }
    int i, j, k;
    for (i = 1; i < len; ++i) {
        fraction_t save = fps[i];
        for (j = 0; j < i; ++j) {
            swap(fps, fps+j);
            for (k = 0; k < sizeof(fraction_ops)/sizeof(fraction_op_t); ++k) {
                fraction_ops[k](&save, fps, fps+i);
                if (dfs(fps+1, len-1, res))
                    return true;
            }
            swap(fps, fps+j);
        }
        fps[i] = save;
    }
    return false;
}

double Game24Points(int a, int b, int c, int d)
{
    fraction_t fractions[4];
    init_fraction(fractions + 0, a, 1);
    init_fraction(fractions + 1, b, 1);
    init_fraction(fractions + 2, c, 1);
    init_fraction(fractions + 3, d, 1);
    double ret = 100000;
    dfs(fractions, 4, &ret);
    return ret;
}

double Game24PointsEx(char *str)
{
    int a, b, c, d;
    sscanf(str, "%d %d %d %d", &a, &b, &c, &d);
    return Game24Points(a, b, c, d);
}

int main(void)
{
    printf("%f\n", Game24Points(7, 2, 1, 10));
    printf("%f\n", Game24Points(7, 2, 2, 10));
    printf("%f\n\n", Game24PointsEx("7  2 2 10"));

    printf("%f\n", Game24PointsEx("1 5 5 5"));
    printf("%f\n", Game24PointsEx("1 7 6 7"));
    printf("%f\n", Game24PointsEx("3 3 8 8"));
    printf("%f\n", Game24PointsEx("9 9 7 5"));
    printf("%f\n", Game24PointsEx("1 9 1 7"));
    return 0;
}
