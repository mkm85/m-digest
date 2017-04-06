## m-digest

WIP, NOT PRODUCTION READY

The m-digest is a fast streaming quantile calculation algorithm
especially targeted for resource constrained devices. The name and
algorithm is inspired by t-digest
[https://github.com/tdunning/t-digest].

### Differences with t-digest

  * It uses a static array of centroids, this avoids malloc.
  * All the algotihmic aspects which has to do with removing and
    inserting centroids into a set of centroids.






