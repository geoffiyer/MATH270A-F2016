#include <iostream>
#include "ImplicitQRSVD.h"
#include "3dPolar.h"

int main() {
    using std::cout;

    Eigen::Matrix<double, 3, 3>A;

    A << 1,2,3,4,5,6,7,8,9;

    cout << "Does this compile?\n";
    cout << A << "\n\n";
    cout << "Symmetric error in A is " << symError(A) << "\n";
    return 0;
}
