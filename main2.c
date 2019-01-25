#include "mdigest2.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv)
{
    printf("init\n");
    struct mdigest md;
    mdigest_init(&md);
    for (int i = 10000; i > 0; i--) {
        double val = rand();
        mdigest_observe(&md, i);
        //print_digest(&md);
    }

    print_digest(&md);
    
    struct mdigest md2;
    mdigest_init(&md2);
    for (int i = 0; i < 10000; i++) {
        double val = rand();
        mdigest_observe(&md2, i);
        //print_digest(&md);
    }

    print_digest(&md2);

    struct mdigest md3;
    mdigest_init(&md3);
    for (int i = 0; i < 10000; i++) {
        double val = rand();
        mdigest_observe(&md3, val);
        //print_digest(&md);
    }

    print_digest(&md3);


    /* struct mdigest md4; */
    /* mdigest_init(&md4); */

    /* mdigest_merge(&md4, &md3); */
    /* print_digest(&md4); */
    /* mdigest_merge(&md4, &md2); */
    /* print_digest(&md4); */
    
}
