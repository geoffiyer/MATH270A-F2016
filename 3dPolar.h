#include<algorithm>
#include<iostream>

template <class T>
void polarDecomposition(const Eigen::Matrix<T,3,3>& F,
    Eigen::Matrix<T, 3, 3>& R,
    Eigen::Matrix<T, 3, 3>& S_Sym);

template <class T>
double symError(const Eigen::Matrix<T,3,3>& F);

template <class T>
void polarDecomposition(const Eigen::Matrix<T,3,3>& F,
    Eigen::Matrix<T, 3, 3>& R,
    Eigen::Matrix<T, 3, 3>& S_Sym)
{

    // Stop-loop conditions
    const long max_it = 1000;
    const T tol = 1024 * std::numeric_limits<T>::epsilon();
    
    // Set up iteration. R = I,  S = F, it = 0
    typedef Eigen::Matrix<T, 3, 3> Matrix3;
    R = Matrix3::Identity();
    S_Sym = F;
    long it = 0;


    // Iterate
    while(max_it < 1000 && symError(F) > tol) {
        for(int i=0; i<2; i++) {
            for(int j=0; j<2; j++) {
                JIXIE::GivensRotation<T> g( S(i,i)+S(j,j), S(j,i)-S(i,j), i, j);
                // TODO: I ended here because I didn't understand the
                //   homework pdf. What is b_ij?
                //   I can guess what G_ij is though
            }
        }
    }
}

template <class T>
double symError(const Eigen::Matrix<T,3,3>& F) {
    return std::max(std::max(F(1,0)-F(0,1), F(2,0)-F(0,2)), F(2,1)-F(1,2));
}
