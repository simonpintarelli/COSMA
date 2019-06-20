#pragma once
#include <cosma/communicator.hpp>
#include <cosma/one_sided_communicator.hpp>
#include <cosma/two_sided_communicator.hpp>

namespace cosma {

// NOTE: The vtable is emitted in every translation units, it's better to move
// the method implementation to a cpp file (i.e. out of line).
//
class hybrid_communicator : public communicator {
  public:
    hybrid_communicator() = default;
    hybrid_communicator(const Strategy *strategy, MPI_Comm comm)
        : communicator::communicator(strategy, comm) {}

    // use two sided communication backend for collective communication
    // that happens before the last step
    void copy(Interval &P,
              double *in,
              double *out,
              double *reshuffle_buffer,
              std::vector<std::vector<int>> &size_before,
              std::vector<int> &total_before,
              int total_after,
              int step) override {
        MPI_Comm comm = active_comm(step);
        two_sided_communicator::copy(comm,
                                     rank(),
                                     strategy_->divisor(step),
                                     P,
                                     in,
                                     out,
                                     reshuffle_buffer,
                                     size_before,
                                     total_before,
                                     total_after);
    }

    void reduce(Interval &P,
                double *LC,
                double *C,
                double *reshuffle_buffer,
                double *reduce_buffer,
                std::vector<std::vector<int>> &c_current,
                std::vector<int> &c_total_current,
                std::vector<std::vector<int>> &c_expanded,
                std::vector<int> &c_total_expanded,
                int beta,
                int step) override {
        MPI_Comm comm = active_comm(step);
        two_sided_communicator::reduce(comm,
                                       rank(),
                                       strategy_->divisor(step),
                                       P,
                                       LC,
                                       C,
                                       reshuffle_buffer,
                                       reduce_buffer,
                                       c_current,
                                       c_total_current,
                                       c_expanded,
                                       c_total_expanded,
                                       beta);
    }

    // in the last step, when the communication is overlapped with the
    // computation use the one sided communication backend as it may be more
    // efficient in this case
    void overlap_comm_and_comp(context &ctx,
                               CosmaMatrix<double> &matrixA,
                               CosmaMatrix<double> &matrixB,
                               CosmaMatrix<double> &matrixC,
                               Interval &m,
                               Interval &n,
                               Interval &k,
                               Interval &P,
                               size_t step,
                               double beta) override {
        MPI_Comm comm = active_comm(step);
        one_sided_communicator::overlap_comm_and_comp(ctx,
                                                      comm,
                                                      rank(),
                                                      strategy_,
                                                      matrixA,
                                                      matrixB,
                                                      matrixC,
                                                      m,
                                                      n,
                                                      k,
                                                      P,
                                                      step,
                                                      beta);
    }
};
} // namespace cosma
