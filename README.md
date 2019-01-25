## m-digest

WIP, NOT PRODUCTION READY

The m-digest is a fast streaming quantile calculation algorithm
especially targeted for resource constrained devices. The name and
algorithm is inspired by t-digest/q-digest.
[https://github.com/tdunning/t-digest].

### Differences with t-digest

  * It uses a static array of centroids, this avoids malloc.
  * All the algotihmic aspects which has to do with removing and
    inserting centroids into a set of centroids.

## TODO

  * Implement averaging between centroids when calculating quantiles.


## Merging

Take the mdigest with most elements and merge the other mdigest into
this, one bucket at a time. Start with the smallest bucket. At most a
few increases is needed since we are basing the merge on the largest
of the mdigests.

## Proof that the structure is filled properly.

Given a mdigest, after `increase_max_count` is called the the fillrate
is >= 1/6 implying that the fill rate was >= 1/3 before
increase_max_count as it at most doubles the capacity.

lets assume that if two buckets can be merged to a lower bucket they
are merged into this bucket. Such that the structure is optimally
filled given the merge down algorithm.

bucket[i].count + bucket[i+1].count > bucket[i].maxCount, else they would
be merged together into bucket[i].

if we assume bucket[i+1].maxCount = bucket[i].maxCount * 2 we get that
bucket[i].maxCount + bucket[i+1].maxCount = 3*bucket[i].maxCount

this implies that the fillrate for bucket[i] and bucket[i+1] >
bucket[i].maxCount / 3*bucket[i].maxCount = 1/3

this implies that the fill rate for a structure is atleast 1/3. We are
not going to call `increase_max_buckets` before the structure is full,
hence the structure is atleast 1/6 filled after a call to
`increase_max_count`.

After a call to `increase_max_count` we still have the invariant that
the fillrate for bucket i an i+1 is > 1/3 except for the last two
buckets.



## Fast insert
Since the last buckets are the largest, new insert search from the
back until the best bucket for a fit is found. This way a binary
search is not needed an the average number og searched buckets is
log_2(number of buckets). which is as fast as a binary search for the
desired bucket.


## difference between two mdigests

lets say we have two mdigests one for 24hr and one for 24hr and 5
minutes we want to know the quantiles for the 5 minute period. But
since the 5 minute quantile data could all be around the 24hr median
value and about zero data is store regarding the actual quantiles for
these 5 minutes.

Instead collect several mdigests and merge them together for larger
timespans.
