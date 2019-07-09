#ifdef COSMA_WITH_SCALAPACK
#include <cosma/scalapack.hpp>

grid2grid::scalapack::ordering cosma::scalapack::rank_ordering(int ctxt, int P) {
    // check whether rank grid is row-major or col-major
    auto ordering = grid2grid::scalapack::ordering::column_major;
    if (P > 1) {
        int prow, pcol;
        // check the coordinates of rank 1 to see
        // if the rank grid is row-major or col-major
        blacs::Cblacs_pcoord(ctxt, 1, &prow, &pcol);
        if (prow == 0 && pcol == 1) {
            ordering = grid2grid::scalapack::ordering::row_major;
        }
    }
    return ordering;
}

int cosma::scalapack::get_context(const int* desca, const int* descb, const int* descc) {
    int ctxt = desca[1];
    // all matrices should belong to the same context
    assert(desca[1] == descb[1]);
    assert(descb[1] == descc[1]);
    return ctxt;
}

int cosma::scalapack::leading_dimension(const int* desc) {
    return desc[8];
}
#endif