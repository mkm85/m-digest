#include "mdigest.h"

#include <stdio.h>
#include <stdlib.h>

void print_quantile(struct mdigest* dgst, double quantile)
{
    printf("%.3f: %.10e\n", quantile, mdigest_get_quantile(dgst, quantile));
}

int main() {
    int i;
    struct mdigest dgst;
    mdigest_init(&dgst);
    for (i = 0; i < 1000; i++) {
        mdigest_add(&dgst, (double)i, 1);
    }

    printf("quantiles\n");
    print_quantile(&dgst, 0.001);
    print_quantile(&dgst, 0.01);
    print_quantile(&dgst, 0.25);
    print_quantile(&dgst, 0.50);
    print_quantile(&dgst, 0.75);
    print_quantile(&dgst, 0.99);
    print_quantile(&dgst, 0.999);
}


