#include <iostream>
#include "ImplicitQRSVD.h"
#include "3dPolar.h"

int main() {
    using std::cout;
    
    bool verbose_flag = true;
    Eigen::Matrix<double, 3, 3>A, R, S;
    A << 1,1,1,2,2,2,3,4,5;

    // srand((unsigned int) time(0));
    // A = Eigen::Matrix<double,3,3>::Random();

    polarDecomposition(A,R,S,verbose_flag);
    //cout << symError(A) << "\n";
    
    return 0;
}
