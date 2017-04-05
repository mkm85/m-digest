#ifndef MDIGEST_H
#define MDIGEST_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// WLOG, this has to be an even number for simplicity
#ifndef MDIGEST_BUCKETS
#define MDIGEST_BUCKETS 128
#endif

typedef double value_t;
typedef int64_t weight_t;

struct centroid {
    value_t centroid; // Calculated centroid for this bucket.
    weight_t count; // Current count of values in this bucket.
    weight_t target; // Target weight for this bucket. I.e. current max number of elements.
};

/**
 * There is an ordering between all the buckets. centroids[i].center < centroids[i+1].center;
 * 
 * There is a function f(i,n) which given a bucket $i$ returns the
 * number of elements the bucket can contain. This function should be
 * made such that the outer values in the observations resides in buckets with least weight.
 *
 * A good function for the upper bound of values in a single bucket
 * with the quantile q is (q(1-q)*4*n) where n is the total weight of
 * all samples.
 *
 * Too keep the implementation simple the buckets contains a weight distribution where the total number of elements is k*2*2*2^(buckets/2) starting with k = 1. The k value is a layer. each layer can contain max k*2*2*2^(buckets/2) observations. When the number of elements reaches 
 * 
 * When a bucket reaches max the upper bound is set to 2xcurrent weight and the buckets is rebalanced such that each bucket contains exactly bucketSize/2 elements.
 * 
 * The algorithm has two phases initial fillup and aggregation. In the
 * initial fillup each centroid is assigned a value. When each
 * centroid has a value the aggregation phase begins.
 * 
 * After each rebalance there is at least room for one element in each bucket.
 * 
 */

#define upper(quantile, n) ((quantile)((1-quantile)*4*n))



struct mdigest {
    struct centroid centroids[MDIGEST_BUCKETS];
    weight_t count;
    weight_t target;
};

void centroid_init(struct centroid* c);

void centroid_add(struct centroid* c, value_t value, weight_t weight);
void centroid_swap(struct centroid * lhs, struct centroid * rhs);

/**
 * initialize the mdigest structure, by setting all buckets to 0.
 */
void mdigest_init(struct mdigest* dgst);

/**
 * add value with a weight to the mdigest structure.
 */
void mdigest_add(struct mdigest* dgst, value_t value, weight_t weight);

void mdigest_initial_insert(struct mdigest* dgst, value_t value);



void mdigest_rebalance(struct mdigest* dgst, size_t bucket);

bool mdigest_rebalance_centroids(struct centroid* this, struct centroid* next);

double mdigest_get_quantile(struct mdigest* dgst, double quantile);

#endif
