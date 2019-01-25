#include "mdigest2.h"
#include <math.h>
#include <stdio.h>

static double mdigest_fill_rate_count(struct mdigest* md, uint64_t target);

void mdigest_init(struct mdigest* md)
{
    md->count = 0;
    for (int i = 0; i < MDIGEST_BUCKETS; i++) {
        md->centroids[i].mean = 1.0/0.0; // +inf
        md->centroids[i].count = 0;
        md->maxCounts[i] = 1;
    }
}

void print_digest(struct mdigest* md)
{
    printf("count %d\n", md->count);
    printf("i    maxCount  Count   Mean\n");
    for (int i = 0; i < MDIGEST_BUCKETS; i++) {
        printf("%d\t%d\t%d\t%f\n", i, md->maxCounts[i], md->centroids[i].count, md->centroids[i].mean);
    }
}


static void mdigest_merge_centroids(struct centroid* c1, struct centroid* c2)
{
    //printf("merging centroids\n");
    if (c1->count == 0) {
        c1->count = c2->count;
        c1->mean = c2->mean;
        return;
    }
    c1->count += c2->count;
    c1->mean += c2->count * (c2->mean - c1->mean) / c1->count;
}


static void increase_max_count(struct mdigest* md)
{
    //printf("increase max count\n");
    //print_digest(md);
    uint64_t max = md->maxCounts[MDIGEST_BUCKETS-1];
    max = ceil(max*2);
    for (int i = MDIGEST_BUCKETS - 1; i >= 0; i--) {
        uint64_t tmp = max;
        max = md->maxCounts[i];
        md->maxCounts[i] = tmp;
    }

    // merge buckets upwards
    for (int i = 0; i < MDIGEST_BUCKETS-1; i++) {
        struct centroid* current = &md->centroids[i];
        struct centroid* next = &md->centroids[i+1];
        
        if (current->count + next->count <= md->maxCounts[i]) {
            // merge them
            mdigest_merge_centroids(current, next);
            next->mean = 0.0;
            next->count = 0;
        }
    }
    //print_digest(md);
}

static void mdigest_centroid_swap(struct centroid* c1, struct centroid* c2)
{
    double tmpMean = c1->mean;
    uint64_t tmpCount = c1->count;
    c1->mean = c2->mean;
    c1->count = c2->count;
    c2->mean = tmpMean;
    c2->count = tmpCount;
}


void mdigest_observe(struct mdigest* md, double value)
{
    //printf("observing value %f\n", value);
    md->count++;
    if (md->count == 1) {
        md->centroids[0].mean = value;
        md->centroids[0].count = 1;
        return;
    }
    
    struct centroid tmp;
    tmp.mean = value;
    tmp.count = 1;

    for(int i = 0; i < MDIGEST_BUCKETS-1; i++) {
        if (tmp.mean < md->centroids[i+1].mean) {
            // try to merge else swap the centroid in
            if (md->maxCounts[i] >= (md->centroids[i].count + tmp.count)) {
                mdigest_merge_centroids(&md->centroids[i], &tmp);
                return;
            }
            if (tmp.mean < md->centroids[i].mean) {
                mdigest_centroid_swap(&tmp, &md->centroids[i]);
            }
        }
        if (tmp.count == 0) {
            break;
        }
    }

    if (tmp.count != 0) {
        if (md->maxCounts[MDIGEST_BUCKETS-1] > md->centroids[MDIGEST_BUCKETS-1].count) {
            mdigest_merge_centroids(&md->centroids[MDIGEST_BUCKETS-1], &tmp);
            return;
        } 
        if (tmp.mean < md->centroids[MDIGEST_BUCKETS-1].mean) {
            mdigest_centroid_swap(&tmp, &md->centroids[MDIGEST_BUCKETS-1]);
        }
        
        increase_max_count(md);
        mdigest_merge_centroids(&md->centroids[MDIGEST_BUCKETS-1], &tmp);
    }
}


double mdigest_get_quantile(struct mdigest* md, double quantile)
{
    uint64_t count = 0;
    uint64_t target = ceil(md->count * quantile);

    printf("get %f quantile, meaning value og the %d observation\n", quantile, target);
    for (int i = 0; i < MDIGEST_BUCKETS; i++) {
        count += md->centroids[i].count;
        if (count >= target) {
            return md->centroids[i].mean;
        }
    }
}

double mdigest_fill_rate_count(struct mdigest* md, uint64_t target)
{
    uint64_t totalRoom = 0;
    for (int i = 0; i < MDIGEST_BUCKETS; i++) {
        totalRoom += md->maxCounts[i];
    }

    if (target == 0) {
        return 0.0;
    }
    totalRoom/target;
}

/* void mdigest_merge(struct mdigest* md, struct mdigest* merge) */
/* { */
/*     uint64_t target = md->count + merge->count; */
    
/*     while (mdigest_fill_rate_count(md, target) > 0.5) { */
/*         increase_max_count(md); */
/*     } */

/*     int i = 0; */
/*     int j = 0; */

/*     while(i != MDIGEST_BUCKETS && j != MDIGEST_BUCKETS) { */

/*         if (md->centroids[i].mean < merge->centroids[j].mean) { */
            
/*         } */
        
/*     } */
    
/*     for (int i = 0; i < ) */
/* } */
