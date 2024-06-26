#ifndef BENCH_UTILS_H
#define BENCH_UTILS_H

/* These function will be declared by the runtime and 
 * passed to the module.
 */
void bench_start();
void bench_end();


/**
 * Call this function to prevent certain compiler-related optimizations related to knowing the value
 * of the passed variable.
 */
static void _black_box(void *x)
{
    (void)x;
}
static void (*volatile black_box)(void *x) = _black_box;

#define BLACK_BOX(X) black_box((void *)&(X))


#endif /* BENCH_UTILS_H */