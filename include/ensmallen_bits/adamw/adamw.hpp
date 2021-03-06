/**
 * @file adamw.hpp
 * @author Niteya Shah
 *
 * Declaration of the AdamW optimizer.
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef ENSMALLEN_ADAM_ADAMW_HPP
#define ENSMALLEN_ADAM_ADAMW_HPP

#include <ensmallen_bits/sgd/sgd.hpp>
#include "adamw_update.hpp"

namespace ens {

/**
 * AdamW is an variation of Adam with decoupled weight decay which is designed to
 * improve convergence. This update policy performs an update similar to L2
 * regularization with better result due to its decoupled strategy.
 *
 * For more information, see the following.
 *
 * @code
 * @article{Kingma2014,
 *   author  = {Diederik P. Kingma and Jimmy Ba},
 *   title   = {Adam: {A} Method for Stochastic Optimization},
 *   journal = {CoRR},
 *   year    = {2014},
 *   url     = {http://arxiv.org/abs/1412.6980}
 * }
 *
 * @article{
 *   title   = {Decoupled Weight Decay Regularization},
 *   author  = {Loschilov, I. and Hutter, F.},
 *   journal = {ArXiv e-prints},
 *   url     = {https://arxiv.org/pdf/1711.05101.pdf}
 *   year    = {2019}
 * }
 * @endcode
 *
 * AdamW can optimize differentiable separable functions.  For more details,
 * see the documentation on function types included with this distribution
 * or on the ensmallen website.
 */
class AdamW
{
 public:
  /**
   * Construct the AdamW optimizer with the given function and parameters. Same
   * as with with Adam, AdamW's defaults  are not necessarily good for the given
   * problem, so it is suggested that the values used be tailored to the task at
   * hand. The maximum number of iterations refers to the maximum number of
   * points that are processed (i.e., one iteration equals one point; one
   * iteration does not equal one pass over the dataset).
   *
   * @param stepSize Step size for each iteration.
   * @param batchSize Number of points to process in a single step.
   * @param weightDecay The rate at which the update regularises the iterate.
   * @param beta1 Exponential decay rate for the first moment estimates.
   * @param beta2 Exponential decay rate for the weighted infinity norm
            estimates.
   * @param epsilon Value used to initialise the mean squared gradient parameter.
   * @param maxIterations Maximum number of iterations allowed (0 means no
   *        limit).
   * @param tolerance Maximum absolute tolerance to terminate algorithm.
   * @param shuffle If true, the function order is shuffled; otherwise, each
   *        function is visited in linear order.
   * @param resetPolicy If true, parameters are reset before every Optimize
   *        call; otherwise, their values are retained.
   */
  AdamW(const double stepSize = 0.001,
        const size_t batchSize = 32,
        const double weightDecay = 0.0005,
        const double beta1 = 0.9,
        const double beta2 = 0.999,
        const double epsilon = 1e-8,
        const size_t maxIterations = 100000,
        const double tolerance = 1e-5,
        const bool shuffle = true,
        const bool resetPolicy = true);

  /**
   * Optimize the given function using AdamW. The given starting point will be
   * modified to store the finishing point of the algorithm, and the final
   * objective value is returned.
   *
   * @tparam DecomposableFunctionType Type of the function to optimize.
   * @param function Function to optimize.
   * @param iterate Starting point (will be modified).
   * @return Objective value of the final point.
   */
  template<typename DecomposableFunctionType>
  double Optimize(DecomposableFunctionType& function, arma::mat& iterate)
  {
    return optimizer.Optimize(function, iterate);
  }

  //! Get the step size.
  double StepSize() const { return optimizer.StepSize(); }
  //! Modify the step size.
  double& StepSize() { return optimizer.StepSize(); }

  //! Get the batch size.
  size_t BatchSize() const { return optimizer.BatchSize(); }
  //! Modify the batch size.
  size_t& BatchSize() { return optimizer.BatchSize(); }

  //! Get the smoothing parameter.
  double Beta1() const { return optimizer.UpdatePolicy().Beta1(); }
  //! Modify the smoothing parameter.
  double& Beta1() { return optimizer.UpdatePolicy().Beta1(); }

  //! Get the second moment coefficient.
  double Beta2() const { return optimizer.UpdatePolicy().Beta2(); }
  //! Modify the second moment coefficient.
  double& Beta2() { return optimizer.UpdatePolicy().Beta2(); }

  //! Get the value used to initialise the mean squared gradient parameter.
  double Epsilon() const { return optimizer.UpdatePolicy().Epsilon(); }
  //! Modify the value used to initialise the mean squared gradient parameter.
  double& Epsilon() { return optimizer.UpdatePolicy().Epsilon(); }

  //! Get the maximum number of iterations (0 indicates no limit).
  size_t MaxIterations() const { return optimizer.MaxIterations(); }
  //! Modify the maximum number of iterations (0 indicates no limit).
  size_t& MaxIterations() { return optimizer.MaxIterations(); }

  //! Get the tolerance for termination.
  double Tolerance() const { return optimizer.Tolerance(); }
  //! Modify the tolerance for termination.
  double& Tolerance() { return optimizer.Tolerance(); }

  //! Get whether or not the individual functions are shuffled.
  bool Shuffle() const { return optimizer.Shuffle(); }
  //! Modify whether or not the individual functions are shuffled.
  bool& Shuffle() { return optimizer.Shuffle(); }

  //! Get whether or not the update policy parameters
  //! are reset before Optimize call.
  bool ResetPolicy() const { return optimizer.ResetPolicy(); }
  //! Modify whether or not the update policy parameters
  //! are reset before Optimize call.
  bool& ResetPolicy() { return optimizer.ResetPolicy(); }

  //! Get the weight decay parameter.
  double WeightDecay() const { return optimizer.UpdatePolicy().WeightDecay(); }
  //! Modify the weight decay parameter.
  double& WeightDecay() { return optimizer.UpdatePolicy().WeightDecay(); }

 private:
  //! The Stochastic Gradient Descent object with AdamW policy.
  SGD<AdamWUpdate> optimizer;
};

} // namespace ens

// Include implementation.
#include "adamw_impl.hpp"

#endif
