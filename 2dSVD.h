/**
   Copyright (c) 2016 Theodore Gast, Chuyuan Fu, Chenfanfu Jiang, Joseph Teran

   Permission is hereby granted, free of charge, to any person obtaining a copy of
   this software and associated documentation files (the "Software"), to deal in
   the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   If the code is used in an article, the following paper shall be cited:
   @techreport{qrsvd:2016,
   title={Implicit-shifted Symmetric QR Singular Value Decomposition of 3x3 Matrices},
   author={Gast, Theodore and Fu, Chuyuan and Jiang, Chenfanfu and Teran, Joseph},
   year={2016},
   institution={University of California Los Angeles}
   }

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <cmath>
#include <iostream>
#include "Tools.h"
#include "ImplicitQRSVD.h"

// Some shortening of names of stuff here
typedef double T;          
typedef Eigen::Matrix<T, 2, 2> Matrix;    // Shorten the name of matrix class
typedef Eigen::Matrix<T, 2, 1> Vector;

double det(const Matrix& A);
void swapCols(Matrix& A);

// Inputs: Matrix F to compute the svd
//   Calculates svd via algorithm 2 discussed in class
// Outputs (by reference): Matrices U,V. Vector of singular values Sigma.
// I should probably make this templated on double or float?
void svd(const Matrix& F, Matrix& U, Vector& Sigma, Matrix& V, bool verbose_flag);

double det(const Matrix& A) {
    return A(0,0)*A(1,1) - A(1,0)*A(0,1);
}

void swapCols(Matrix& A) {
    for(int i=0; i<2; i++) {
        T temp = A(i,0);
        A(i,0) = A(i,1);
        A(i,1) = temp;
    }
}

// Inputs: Matrix F to compute the svd
//   Calculates svd via algorithm 2 discussed in class
// Outputs (by reference): Matrices U,V. Vector of singular values Sigma.
// I should probably make this templated on double or float?
void svd(const Matrix& F, Matrix& U, Vector& Sigma, Matrix& V, bool verbose_flag) {

    using std::cout;
    using std::sqrt;
    using std::abs;

    // C  = F^T F
    Matrix C;
    C.noalias() = F.transpose() * F;

    // Apply eigensolver to C to get V, \sigma_1 and \sigma_2
    //       Use eigen self-adjoint solver
    Eigen::SelfAdjointEigenSolver<Matrix> esolve;
    esolve.compute(C);
    V = esolve.eigenvectors();
    V *= 1/det(V);  // TODO: This line is scary
    
    // Set A = FV  (so A = U*Sigma)
    Matrix A;
    A.noalias() = F*V;
    
    // Use givens rotation on A. The cos and sin should give U
    //            U = [ c  s
    //                 -s  c ]
    //         ImplicitQRSVD.h   should have givens code
    //    Here c is givens.c and s is givens.s
    JIXIE::GivensRotation<T> givens(A(0,0),A(1,0),0,1);
    U << givens.c, givens.s, -givens.s, givens.c;
    
    // Use that U.transpose()*A = Sigma to get the eigenvalues of F
    T sigma_1 = A(0,0)*givens.c - A(1,0)*givens.s;
    T sigma_2 = A(0,1)*givens.s + A(1,1)*givens.c;
    
    // Pass through U, V, Sigma to fix sign conventions.
    // in other words, this is the cleanup step
    if(abs(sigma_1)<abs(sigma_2)) {
        T temp = sigma_1;
        sigma_1 = sigma_2;
        sigma_2 = temp;
        swapCols(U);
        swapCols(V);
        if(verbose_flag)
            cout << "Swapped sigma_1 and sigma_2\n";
    }

    if(sigma_1 < 0) {
        sigma_1 *= -1;
        U.col(0) *= -1;
        if(verbose_flag)
            cout << "Negated sigma_1\n";
    }
    
    if(det(F) > 0 && sigma_2 < 0) {
        sigma_2 *= -1;
        U.col(1) *= -1;
        if(verbose_flag)
            cout << "Negated sigma_2\n";
    }
    else if (det(F) < 0 && sigma_2 > 0){
        sigma_2 *= -1;
        U.col(1) *= -1;
        if(verbose_flag)
            cout << "Negated sigma_2\n";
    }

    if(det(U)<0) {
        U.col(0) *= -1;
        V.col(0) *= -1;
        if(verbose_flag)
            cout << "Fixed det of U and V\n";
    }

    // Vector of singular values
    Sigma << sigma_1, sigma_2;

    if(verbose_flag) {
        cout << "\nMy arithmetic gives us singular values of F\n" << Sigma.transpose()
             << "\nU and V are (respectively)\n" << U << "\n\n" << V
             << "\n\nError in \\ell^2 norm: " << (U*Sigma.asDiagonal()*V.transpose()-F).norm() << "\n\n";
    }
    
}
