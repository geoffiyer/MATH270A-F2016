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
#include "2dSVD.h"

// Some shortening of names of stuff here
typedef double T;          
typedef Eigen::Matrix<T, 2, 2> Matrix;    // Shorten the name of matrix class
typedef Eigen::Matrix<T, 2, 1> Vector;

int main() {
    bool verbose_flag = true;
    
    // Input: 2x2 matrix F
    //    For now just use a random matriix.
    srand((unsigned int) time(0));
    Matrix F = Matrix::Random();
    Matrix U,V;
    Vector Sigma;

    svd(F,U,Sigma,V,verbose_flag);

    return 0;
}
