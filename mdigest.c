#include "mdigest.h"
#include <math.h>

#define MDIGEST_MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define MDIGEST_SWAP(x,y) do {   \
   typeof(x) _x = x;             \
   typeof(y) _y = y;             \
   x = _y;                       \
   y = _x;                       \
 } while(0)

#ifndef MDIGEST_ASSERT
#include <stdio.h>
#include <stdlib.h>
#define MDIGEST_ASSERT(expr) do { if (!(expr)) { printf("assert failed %s:%d\n", __FILE__, __LINE__); exit(1); } } while(0)
#endif
       
void centroid_init(struct centroid* c)
{
    c->centroid = 0;
    c->count = 0;
    c->target = 1;
}

void centroid_add(struct centroid* c, value_t value, weight_t weight)
{
    c->count += weight;
    if (c->count != 0) {
        c->centroid += weight * (value - c->centroid) / c->count;
    }
}


void mdigest_init(struct mdigest* dgst)
{
    unsigned int i;
    for (i = 0; i < MDIGEST_BUCKETS; i++) {
        centroid_init(&dgst->centroids[i]);
    }

    dgst->count = 0;
    dgst->target = MDIGEST_BUCKETS;
    
}


size_t mdigest_find_bucket(struct mdigest* dgst, value_t value)
{
    // binary search through all buckets.
    // if the weight of a bucket is 0 then return it
    size_t i;
    
    // if this is for the first bucket
    if (value < dgst->centroids[0].centroid) {
        return 0;
    }
    
    for (i = 0; i < MDIGEST_BUCKETS-1; i++) {
        if ((dgst->centroids[i].centroid >= value) && (dgst->centroids[i+1].centroid < value)) {
            // the value is between center of centroid i and i+1
            // return the nearest bucket.
            double dist1 = fabs(dgst->centroids[i].centroid - value);
            double dist2 = fabs(value - dgst->centroids[i+1].centroid);
            if (dist1 < dist2) {
                return i;
            } else {
                return i+1;
            }
        }
    }
    // if we end here, the value must be for the last bucket
    return MDIGEST_BUCKETS-1;
}


/**
 * return the value for the given quantile
 */
double mdigest_get_quantile(struct mdigest* dgst, double quantile)
{
    value_t acc = 0;
    size_t i;
    for (i = 0; i < MDIGEST_BUCKETS-1; i++) {
        double thisQuantile = (acc + dgst->centroids[i].count/2) / dgst->count;
        double nextQuantile = (acc + dgst->centroids[i+1].count/2) / dgst->count;

        if (quantile < thisQuantile) {
            return dgst->centroids[i].centroid;
        }

        if (quantile >= thisQuantile && quantile <= nextQuantile) {
            // todo
            return dgst->centroids[i].centroid;
        }
        acc += dgst->centroids[i].count;
    }
    return dgst->centroids[MDIGEST_BUCKETS-1].centroid;
}

void mdigest_add(struct mdigest* dgst, value_t value, weight_t weight)
{
    if (weight < 1) {
        return;
    }
    
    if (dgst->count == 0) {
        size_t i;
        
        // special case for the first value
        dgst->min = value;
        dgst->max = value;

        for (i = 0; i < MDIGEST_BUCKETS; i++) {
            dgst->centroids[i].centroid = value;
        }
        //dgst->count = 1;
    }

    if (value < dgst->min) {
        dgst->min = value;
    }

    if (value > dgst->max) {
        dgst->max = value;
    }
    
    while (weight > 0) {
        // find a bucket where the value can be
        size_t bucket = mdigest_find_bucket(dgst, value);
        struct centroid* c = &dgst->centroids[bucket];
        // how much of the weight to insert into the fund bucket
        weight_t weightToInsert = MDIGEST_MIN(weight, (c->target - c->count));
        centroid_add(c, value, weightToInsert);
        weight -= weightToInsert;
        dgst->count += weightToInsert;
        if (weight > 0) {
            mdigest_rebalance(dgst, bucket);
        }
    }
}

void centroid_swap(struct centroid * lhs, struct centroid * rhs)
{
    MDIGEST_SWAP(*lhs, *rhs);
}
void mdigest_rebalance(struct mdigest* dgst, size_t bucket)
{
    size_t i;
    dgst->target = dgst->count * 2;
    
    // assume a symmetric flat distribution
    // TODO make a interesting distribution.
    for (i = 0; i < MDIGEST_BUCKETS; i++) {
        // ceil
        weight_t newTarget = (dgst->target-1) / MDIGEST_BUCKETS + 1;
        if (newTarget < 2) {
            newTarget = 2;
        }

        dgst->centroids[i].target = newTarget;
    }

    
    weight_t extraLhs = 0;
    weight_t extraRhs = 0;
    
    for (i = 0; i < bucket; i++) {
        extraLhs += dgst->centroids[i].target - dgst->centroids[i].count;
    }

    for (i = MDIGEST_BUCKETS-1; i > bucket; i--) {
        extraRhs += dgst->centroids[i].target - dgst->centroids[i].count;
    }

    if (extraLhs > extraRhs) {
        // merge to the left
        for (i = bucket; i > 0; i--) {
            struct centroid* this = &dgst->centroids[i];
            struct centroid* next = &dgst->centroids[i-1];
            if (!mdigest_rebalance_centroids(this, next)) {
                break;
            }
        }
    } else {
        // merge to the right
        for (i = bucket; i < MDIGEST_BUCKETS-1; i++) {
            struct centroid* this = &dgst->centroids[i];
            struct centroid* next = &dgst->centroids[i+1];
            if (!mdigest_rebalance_centroids(this, next)) {
                break;
            }
        }
    }

    for (i = 0; i < MDIGEST_BUCKETS; i++) {
        MDIGEST_ASSERT((dgst->centroids[i].target - dgst->centroids[i].count) >= 0);
    }
    
}

bool mdigest_rebalance_centroids(struct centroid* this, struct centroid* next)
{
    if (this->count > (this->target -1)/2 +1) {
        // there is too much in this bucket
        value_t tooMuch = this->count - ((this->target -1) / 2 + 1);
        centroid_add(this, this->centroid, -tooMuch);
        centroid_add(next, this->centroid, tooMuch);
        if (next->count > next->target) {
            return true;
        }
    }
    return false;
}
