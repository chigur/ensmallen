/**
 * @file atoms.hpp
 * @author Chenzhe Diao
 *
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_FW_ATOMS_HPP
#define MLPACK_CORE_OPTIMIZERS_FW_ATOMS_HPP

#include <mlpack/prereqs.hpp>
#include "func_sq.hpp"

namespace mlpack {
namespace optimization {

class Atoms
{
 public:
  Atoms(){}
  
  //! Add atom into the solution space.
  void AddAtom(const arma::mat& v, const double c = 0)
  {
    if (currentAtoms.is_empty())
    {
      CurrentAtoms() = v;
      CurrentCoeffs() = c;
    }
    else
    {
      currentAtoms.insert_cols(0, v);
      arma::vec cVec = {c};
      currentCoeffs.insert_rows(0, cVec);
    }
  }
  
  
  //! Recover the solution coordinate from the coefficients of current atoms.
  void RecoverVector(arma::mat& x)
  {
    x = currentAtoms * currentCoeffs;
  }

  /** 
   * Prune the support, delete previous atoms if they don't contribute much.
   * See Algorithm 2 of paper:
   * @code
   * @article{RaoShaWri:2015Forward--backward,
   *    Author = {Rao, Nikhil and Shah, Parikshit and Wright, Stephen},
   *    Journal = {IEEE Transactions on Signal Processing},
   *    Number = {21},
   *    Pages = {5798--5811},
   *    Publisher = {IEEE},
   *    Title = {Forward--backward greedy algorithms for atomic norm regularization},
   *    Volume = {63},
   *    Year = {2015}
   * }
   * @endcode
   *
   * @param F thresholding number.
   * @param function function to be optimized.
   */
  void PruneSupport(const double F, FuncSq& function)
  {
    arma::mat atomSqTerm = function.MatrixA() * currentAtoms;
    atomSqTerm = sum(square(atomSqTerm), 0);
    atomSqTerm = 0.5 * atomSqTerm.t() % square(currentCoeffs);

    while (true)
    {
      // Solve for current gradient.
      arma::mat x;
      RecoverVector(x);
      arma::mat gradient(size(x));
      function.Gradient(x, gradient);

      // Find possible atom to be deleted.
      arma::vec gap = atomSqTerm - currentCoeffs % trans(gradient.t() * currentAtoms);
      arma::uword ind;
      gap.min(ind);

      // Try deleting the atom.
      arma::mat newAtoms = currentAtoms;
      newAtoms.shed_col(ind);
      // Recalculate the coefficients.
      arma::vec newCoeffs = solve(function.MatrixA() * newAtoms, function.Vectorb());
      // Evaluate the function again.
      double Fnew = function.Evaluate(newAtoms * newCoeffs);
      
      if (Fnew > F)
        // Should not delete the atom.
        break;
      else {
        // Delete the atom from current atoms.
        currentAtoms = newAtoms;
        currentCoeffs = newCoeffs;
        atomSqTerm.shed_row(ind);
      } // else
    } // while
  }



  template<typename FunctionType>
  void ProjectedGradientEnhancement(FunctionType& function,
                                    double tau,
                                    double stepSize,
                                    size_t maxIteration = 100,
                                    double tolerance = 1e-3)
  {
    // Gradient Descent.
    arma::mat x;
    RecoverVector(x);
    double value = function.Evaluate(x);
    
    for (size_t iter = 1; iter<maxIteration; iter++)
    {
      arma::mat g;
      function.Gradient(x, g);
      g = currentAtoms.t() * g;
      currentCoeffs = currentCoeffs - stepSize * g;

      // Projection to l1 ball with norm tau.
      ProjectionToL1(tau);

      RecoverVector(x);
      double valueNew = function.Evaluate(x);

      if ((value - valueNew) < tolerance)
        break;

      value = valueNew;
    }
  }

  
  //! Get the current atom coefficients.
  const arma::vec& CurrentCoeffs() const { return currentCoeffs; }
  //! Modify the current atom coefficients.
  arma::vec& CurrentCoeffs() { return currentCoeffs; }
  
  //! Get the current atoms.
  const arma::mat& CurrentAtoms() const { return currentAtoms; }
  //! Modify the current atoms.
  arma::mat& CurrentAtoms() { return currentAtoms; }

 private:
  //! Coefficients of current atoms.
  arma::vec currentCoeffs;

  //! Current atoms in the solution space.
  arma::mat currentAtoms;


  /**
   * Projection of currentCoeffs to L1 ball with norm tau.
   */
  void ProjectionToL1(const double tau)
  {
    arma::vec simplexSol = arma::abs(currentCoeffs);
    
    // Already with atom norm <= tau.
    if (arma::accu(simplexSol) <= tau)
      return;

    simplexSol = arma::sort(simplexSol, "descend");
    arma::vec simplexSum = arma::cumsum(simplexSol);

    double nu = 0;
    size_t rho;
    for (size_t j = 1; j <= simplexSol.n_rows; j++)
    {
      rho = simplexSol.n_rows - j;
      nu = simplexSol(rho) - (simplexSum(rho) - tau)/(rho + 1);
      if (nu > 0)
        break;
    }
    double theta = (simplexSum(rho) - tau)/rho;
    
    // Threshold on absolute value of currentCoeffs with theta.
    for (arma::uword j = 0; j< simplexSol.n_rows; j++)
    {
      if (currentCoeffs(j) >=0.0)
        currentCoeffs(j) = std::max(currentCoeffs(j)-theta, 0.0);
      else
        currentCoeffs(j) = std::min(currentCoeffs(j)+theta, 0.0);
    }
  }
  

}; // class Atoms
}  // namespace optimization
}  // namespace mlpack


#endif
