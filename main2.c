#include "mdigest2.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv)
{
    printf("init\n");
    struct mdigest md;
    mdigest_init(&md);
    for (int i = 0; i < 10000; i++) {
        double val = rand();
        mdigest_observe(&md, val);
        print_digest(&md);
    }

    print_digest(&md);

    printf("0.001 quantile %f\n", mdigest_get_quantile(&md, 0.001));
    printf("0.01 quantile %f\n", mdigest_get_quantile(&md, 0.01));
    printf("0.1 quantile %f\n", mdigest_get_quantile(&md, 0.1));
    
}
