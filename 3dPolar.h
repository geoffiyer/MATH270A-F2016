#include<algorithm>
#include<iostream>
#include<cmath>

template <class T>
void polarDecomposition(const Eigen::Matrix<T,3,3>& F,
    Eigen::Matrix<T, 3, 3>& R,
    Eigen::Matrix<T, 3, 3>& S);

template <class T>
T symError(const Eigen::Matrix<T,3,3>& F);

template <class T>
void polarDecomposition(const Eigen::Matrix<T,3,3>& F,
    Eigen::Matrix<T, 3, 3>& R,
    Eigen::Matrix<T, 3, 3>& S,
    bool verbose_flag)
{
    using std::sqrt;
    using std::cout;

    // Stop-loop conditions
    const long max_it = 100000;
    const T tol = 1024 * std::numeric_limits<T>::epsilon();
    
    // Set up iteration. R = I,  S = F, it = 0
    typedef Eigen::Matrix<T, 3, 3> Matrix3;
    R = Matrix3::Identity();
    S = F;
    long it = 0;


    // Iterate
    while(it < max_it && symError(S) > tol) {
        for(int i=0; i<3; i++) {
            for(int j=i+1; j<3; j++) {
                JIXIE::GivensRotation<T> g( S(i,i)+S(j,j), S(j,i)-S(i,j), i, j);
                g.columnRotation(R);
                g.rowRotation(S);
            }
        }
        it++;
    }

    if(verbose_flag) {
        cout << "Original Matrix F:\n" << F
             << "\n\nRotation R:\n" << R
             << "\n\nSymmetric S:\n" << S
             << "\nSymmetric Error in S: " << symError(S)
             << "\n\nR*S:\n" << R*S
             << "\n\n\\ell^2 error of F-RS: " << (F-R*S).norm()
             << "\nNumber of iterations: " << it << "\n";
    }
}

template <class T>
T symError(const Eigen::Matrix<T,3,3>& F) {
    using std::fabs;
    return std::max(std::max(fabs(F(1,0)-F(0,1)), fabs(F(2,0)-F(0,2))), fabs(F(2,1)-F(1,2)));
}
