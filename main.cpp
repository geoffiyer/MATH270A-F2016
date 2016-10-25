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
typedef double T;                         // Matrix type can be double or float
const unsigned n = 2;
typedef Eigen::Matrix<T, n, n> Matrix;    // Shorten the name of matrix class
typedef Eigen::Matrix<T, n, 1> Vector;

double det(const Matrix& A);
double swapCols(Matrix& A);

int main() {
    using std::cout;
    using std::sqrt;
    using std::abs;
    bool verbose_flag = true;
    
    // Input: 2x2 matrix F
    //    For now just use a random matriix.
    Matrix F = Matrix::Random();
    F << -1,0,0,-1;
    
    // Steps to do:
    // 1) C  = F^T F
    Matrix C;
    C.noalias() = F.transpose() * F;

    // 2) Apply eigensolver to C to get V, \sigma_1 and \sigma_2
    //       Use eigen self-adjoint solver
    Eigen::SelfAdjointEigenSolver<Matrix> esolve;
    esolve.compute(C);
    Matrix V = esolve.eigenvectors();
    
    // 3) Set A = FV  (so A = U*Sigma)
    Matrix A;
    A.noalias() = F*V;
    
    // 4) Use givens rotation on A. The cos and sin should give U
    //            U = [ c  s
    //                 -s  c ]
    //         ImplicitQRSVD.h   should have givens code
    //    Here c is givens.c and s is givens.s
    JIXIE::GivensRotation<T> givens(A(0,0),A(1,0),0,1);
    Matrix U;
    U << givens.c, givens.s, -givens.s, givens.c;
    
    // 5) Use that U.transpose()*A = Sigma to get the eigenvalues of F
    T sigma_1 = A(0,0)*givens.c - A(1,0)*givens.s;
    T sigma_2 = A(0,1)*givens.s + A(1,1)*givens.c;

    // 6) Pass through U, V, Sigma to fix sign conventions.
    // in other words, this is the cleanup step

    // This is currently not correct.
    // I haven't even looked at det(A) yet.
    // Plan: Fix det(U) and det(V) first.
    //   then check det(A) and use that to figure out the sigmaif(

    if(det(U) < 0) {
        swapCols(U)
    
    if(sigma_1 < 0 && sigma_2 < 0) {
        sigma_1 *= -1;
        sigma_2 *= -1;
        U *= -1;
        if(verbose_flag)
            cout << "Two negative singular values. Had to swap.\n";
    }
    
    if(sigma_1 <= abs(sigma_2)) {
        // sigma_1 and sigma_2
        T temp = sigma_2;
        sigma_2 = sigma_1;
        sigma_1 = temp;
            
        // switch columns of U
        swapCols(U);    
        
        // switch columns of V (same as rows of V^T)
        for(int i=0; i<2; i++) {
            temp = V(i,0);
            V(i,0) = V(i,1);
            V(i,1) = temp;
        }
        if(verbose_flag)
            cout << "Singular values in incorrect order. Swapped\n";
    }
    Sigma << sigma_1, 0, 0, sigma_2;

    if(verbose_flag) {
        cout << "Original Matrix F:\n" << F << "\n\n"
             << "C = F^T * F:\n" << C << "\n\n"
             << "Comparing evals: Eigensolver gives us eigenvalues of F^T*F\n" << esolve.eigenvalues().transpose()
             << "\nMy arithmetic gives us singular values of F\n" << sigma_1 << " " << sigma_2 << "\n\n"
             << "U and V are (respectively)\n" << U << "\n\n" << V << "\n\n"
             << "Compute U*Sigma*V^T to get\n" << U*Sigma*V.transpose() << "\n\n";                    
    }

}

double det(const Matrix& A) {
    return A(0,0)*A(1,1) - A(1,0)*A(0,1);
}

void swapCols(Matrix& A) {
    for(int i=0; i<n; i++) {
        temp = A(i,0);
        A(i,0) = A(i,1);
        A(i,1) = temp;
    }
}
