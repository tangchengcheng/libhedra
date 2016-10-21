// This file is part of libhedra, a library for polyhedral mesh processing
//
// Copyright (C) 2016 Amir Vaxman <avaxman@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef HEDRA_EIGEN_SOLVER_WRAPPER_H
#define HEDRA_EIGEN_SOLVER_WRAPPER_H
#include <igl/igl_inline.h>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <string>
#include <vector>
#include <cstdio>

namespace hedra {
    namespace optimization
    {
        //a templated wrapper to all sparse solvers by Eigen. Not doing much and not entirely efficient since the matrix has to be initialized twice, but not too bad.
        
        //TODO: perhaps better to invalidate the analysis stage and do it all in the factorization.
        template<class EigenSparseSolver>
        class EigenSolverWrapper{
        public:
            EigenSparseSolver solver;
            Eigen::SparseMatrix<double> A;
            Eigen::VectorXi rows, cols;
            
            bool analyze(const Eigen::VectorXi& _rows,
                         const Eigen::VectorXi& _cols){
                rows=_rows;
                cols=_cols;
                A.resize(rows.maxCoeff()+1, cols.maxCoeff()+1);
                std::vector<Eigen::Triplet<double> > triplets;
                for (int i=0;i<rows.size();i++)
                    triplets.push_back(Eigen::Triplet<double> (rows(i), cols(i), 1.0));  //it's just a pattern
                A.setFromTriplets(triplets.begin(), triplets.end());
                solver.analyzePattern(A);
                return true;
            }
            
            bool factorize(const Eigen::VectorXd& values){
                std::vector<Eigen::Triplet<double> > triplets;
                for (int i=0;i<rows.size();i++)
                    triplets.push_back(Eigen::Triplet<double> (rows(i), cols(i), values(i)));
                A.setFromTriplets(triplets.begin(), triplets.end());
                solver.factorize(A);
                return true;  //TODO: to check if factorization went ok.
                
            }
            
            bool solve(const Eigen::MatrixXd& rhs,
                       Eigen::VectorXd& x){
                
                x = solver.solve(rhs);
                return true;
            }
        };
        
        //a simple SPD linear solution solver
        template<class EigenSparseSolver>
        Eigen::VectorXd EigenSingleSolveWrapper(Eigen::SparseMatrix<double> A,Eigen::VectorXd b)
        {
            using namespace Eigen;
            VectorXi I;
            VectorXi J;
            VectorXd S;
            int Counter=0;
            
            for (int k=0; k<A.outerSize(); ++k)
                for (SparseMatrix<double>::InnerIterator it(A,k); it; ++it)
                {
                    if (it.row()>it.col())
                        continue;
                    
                    Counter++;
                }
            
            int NumNonZero=Counter;
            I.resize(NumNonZero);
            J.resize(NumNonZero);
            S.resize(NumNonZero);
            Counter=0;
            for (int k=0; k<A.outerSize(); ++k)
                for (SparseMatrix<double>::InnerIterator it(A,k); it; ++it)
                {
                    if (it.row()>it.col())
                        continue;
                    
                    S(Counter)=it.value();
                    I(Counter)=it.row();   // row index
                    J(Counter++)=it.col();   // col index (here it is equal to k
                    
                }
            
            EigenSolverWrapper<EigenSparseSolver> ls;
            ls.analyze_pattern(I,J);
            ls.factorize();
            VectorXd x;
            ps.solve(b,x);
            return x;
        }
      
    }
}


#endif
