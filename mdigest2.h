#pragma once

#include <stdint.h>

#define MDIGEST_BUCKETS 32

struct centroid {
    double mean;
    uint64_t count;
};


struct mdigest {
    struct centroid centroids[MDIGEST_BUCKETS];
    uint64_t maxCounts[MDIGEST_BUCKETS];
    uint64_t count;
};

// init mdigest
void mdigest_init(struct mdigest* mdigest);

// observe a single value
void mdigest_observe(struct mdigest* mdigest, double value);

// merge two mdigests
void mdigest_merge(struct mdigest* mdigest, struct mdigest* toBeMerged);

double mdigest_get_quantile(struct mdigest* mdigest, double quantile);

void print_digest(struct mdigest* md);

void mdigest_merge(struct mdigest* md, struct mdigest* toMerge);
