## m-digest

WIP, NOT PRODUCTION READY

The name is inspired by t-digest and the implementation is likewise
inspired by t-digest. For resource constrained devices the t-digest
has a few problems. 

1. It requires memory management, this could be overcome with a static
memory pool for the tree, then one has essentially implemented their
own malloc/free
2. A fast implementation is a bit complex in terms of lines of code.

The m-digest tries to solve these problem by using a constant size
datastructure consisting of centroids.





