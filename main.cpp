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
#include "Tools.h"
#include "ImplicitQRSVD.h"

template <class T>
void testAccuracy(const std::vector<Eigen::Matrix<T, 3, 3> >& AA,
    const std::vector<Eigen::Matrix<T, 3, 3> >& UU,
    const std::vector<Eigen::Matrix<T, 3, 1> >& SS,
    const std::vector<Eigen::Matrix<T, 3, 3> >& VV)
{
    T max_UUt_error = 0, max_VVt_error = 0, max_detU_error = 0, max_detV_error = 0, max_reconstruction_error = 0;
    T ave_UUt_error = 0, ave_VVt_error = 0, ave_detU_error = 0, ave_detV_error = 0, ave_reconstruction_error = 0;
    for (size_t i = 0; i < AA.size(); i++) {
        Eigen::Matrix<T, 3, 3> M = AA[i];
        Eigen::Matrix<T, 3, 1> S = SS[i];
        Eigen::Matrix<T, 3, 3> U = UU[i];
        Eigen::Matrix<T, 3, 3> V = VV[i];
        T error;
        error = (U * U.transpose() - Eigen::Matrix<T, 3, 3>::Identity()).array().abs().maxCoeff();
        max_UUt_error = (error > max_UUt_error) ? error : max_UUt_error;
        ave_UUt_error += fabs(error);
        error = (V * V.transpose() - Eigen::Matrix<T, 3, 3>::Identity()).array().abs().maxCoeff();
        max_VVt_error = (error > max_VVt_error) ? error : max_VVt_error;
        ave_VVt_error += fabs(error);
        error = fabs(fabs(U.determinant()) - (T)1);
        max_detU_error = (error > max_detU_error) ? error : max_detU_error;
        ave_detU_error += fabs(error);
        error = fabs(fabs(V.determinant()) - (T)1);
        max_detV_error = (error > max_detV_error) ? error : max_detV_error;
        ave_detV_error += fabs(error);
        error = (U * S.asDiagonal() * V.transpose() - M).array().abs().maxCoeff();
        max_reconstruction_error = (error > max_reconstruction_error) ? error : max_reconstruction_error;
        ave_reconstruction_error += fabs(error);
    }
    ave_UUt_error /= (T)(AA.size());
    ave_VVt_error /= (T)(AA.size());
    ave_detU_error /= (T)(AA.size());
    ave_detV_error /= (T)(AA.size());
    ave_reconstruction_error /= (T)(AA.size());
    std::cout << std::setprecision(10) << " UUt max error: " << max_UUt_error
              << " VVt max error: " << max_VVt_error
              << " detU max error:" << max_detU_error
              << " detV max error:" << max_detV_error
              << " recons max error:" << max_reconstruction_error << std::endl;
    std::cout << std::setprecision(10) << " UUt ave error: " << ave_UUt_error
              << " VVt ave error: " << ave_VVt_error
              << " detU ave error:" << ave_detU_error
              << " detV ave error:" << ave_detV_error
              << " recons ave error:" << ave_reconstruction_error << std::endl;
}

template <class T>
void runImplicitQRSVD(const int repeat, const std::vector<Eigen::Matrix<T, 3, 3> >& tests, const bool accuracy_test)
{
    using namespace JIXIE;
    std::vector<Eigen::Matrix<T, 3, 3> > UU, VV;
    std::vector<Eigen::Matrix<T, 3, 1> > SS;
    JIXIE::Timer timer;
    timer.start();
    double total_time = 0;
    for (int test_iter = 0; test_iter < repeat; test_iter++) {
        timer.click();
        for (size_t i = 0; i < tests.size(); i++) {
            Eigen::Matrix<T, 3, 3> M = tests[i];
            Eigen::Matrix<T, 3, 1> S;
            Eigen::Matrix<T, 3, 3> U;
            Eigen::Matrix<T, 3, 3> V;
            singularValueDecomposition(M, U, S, V);
            if (accuracy_test && test_iter == 0) {
                UU.push_back(U);
                SS.push_back(S);
                VV.push_back(V);
            }
        }
        double this_time = timer.click();
        total_time += this_time;
        std::cout << std::setprecision(10) << "impQR time: " << this_time << std::endl;
    }
    std::cout << std::setprecision(10) << "impQR Average time: " << total_time / (double)(repeat) << std::endl;
    if (accuracy_test)
        testAccuracy(tests, UU, SS, VV);
}

template <class T>
void addRandomCases(std::vector<Eigen::Matrix<T, 3, 3> >& tests, const T random_range, const int N)
{
    using namespace JIXIE;
    int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding random test cases with range " << -random_range << " to " << random_range << std::endl;
    RandomNumber<T> random_gen(123);
    for (int t = 0; t < N; t++) {
        Eigen::Matrix<T, 3, 3> Z;
        random_gen.fill(Z, -random_range, random_range);
        tests.push_back(Z);
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template <class T>
void addIntegerCases(std::vector<Eigen::Matrix<T, 3, 3> >& tests, const int int_range)
{
    using namespace JIXIE;
    int old_count = tests.size();
    std::cout << std::setprecision(10) << "Adding integer test cases with range " << -int_range << " to " << int_range << std::endl;
    Eigen::Matrix<T, 3, 3> Z;
    Z.fill(-int_range);
    typename Eigen::Matrix<T, 3, 3>::Index i = 0;
    tests.push_back(Z);
    while (i < Eigen::Matrix<T, 3, 3>::SizeAtCompileTime) {
        if (Z(i) < int_range) {
            Z(i)++;
            tests.push_back(Z);
            i = 0;
        }
        else {
            Z(i) = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template <class T>
void addPerturbationFromIdentityCases(std::vector<Eigen::Matrix<T, 3, 3> >& tests, const int num_perturbations, const T perturb)
{
    using namespace JIXIE;
    int old_count = tests.size();
    std::vector<Eigen::Matrix<T, 3, 3> > tests_tmp;
    Eigen::Matrix<T, 3, 3> Z = Eigen::Matrix<T, 3, 3>::Identity();
    tests_tmp.push_back(Z);
    std::cout << std::setprecision(10) << "Adding perturbed identity test cases with perturbation " << perturb << std::endl;
    RandomNumber<T> random_gen(123);
    size_t special_cases = tests_tmp.size();
    for (size_t t = 0; t < special_cases; t++) {
        for (int i = 0; i < num_perturbations; i++) {
            random_gen.fill(Z, -perturb, perturb);
            tests.push_back(tests_tmp[t] + Z);
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

template <class T>
void addPerturbationCases(std::vector<Eigen::Matrix<T, 3, 3> >& tests, const int int_range, const int num_perturbations, const T perturb)
{
    using namespace JIXIE;
    int old_count = tests.size();
    std::vector<Eigen::Matrix<T, 3, 3> > tests_tmp;
    Eigen::Matrix<T, 3, 3> Z;
    Z.fill(-int_range);
    typename Eigen::Matrix<T, 3, 3>::Index i = 0;
    tests_tmp.push_back(Z);
    while (i < Eigen::Matrix<T, 3, 3>::SizeAtCompileTime) {
        if (Z(i) < int_range) {
            Z(i)++;
            tests_tmp.push_back(Z);
            i = 0;
        }
        else {
            Z(i) = -int_range;
            i++;
        }
    }
    std::cout << std::setprecision(10) << "Adding perturbed integer test cases with perturbation " << perturb << " and range " << -int_range << " to " << int_range << std::endl;
    RandomNumber<T> random_gen(123);
    size_t special_cases = tests_tmp.size();
    for (size_t t = 0; t < special_cases; t++) {
        for (int i = 0; i < num_perturbations; i++) {
            random_gen.fill(Z, -perturb, perturb);
            tests.push_back(tests_tmp[t] + Z);
        }
    }
    std::cout << std::setprecision(10) << tests.size() - old_count << " cases added." << std::endl;
    std::cout << std::setprecision(10) << "Total test cases: " << tests.size() << std::endl;
}

void runBenchmark()
{
    using namespace JIXIE;
    using std::fabs;

    bool run_qr;

    bool test_float;
    bool test_double;
    bool accuracy_test;
    bool normalize_matrix;
    int number_of_repeated_experiments;
    bool test_random;
    int random_range;
    int number_of_random_cases;
    bool test_integer;
    int integer_range;
    bool test_perturbation;
    int perturbation_count;
    float float_perturbation;
    double double_perturbation;
    bool test_perturbation_from_identity;
    int perturbation_from_identity_count;
    float float_perturbation_identity;
    double double_perturbation_identity;
    std::string title;

    // Finalized options
    run_qr = true;

    test_float = true;
    test_double = true;
    normalize_matrix = false;
    int number_of_repeated_experiments_for_timing = 2;

    for (int test_number = 1; test_number <= 10; test_number++) {

        if (test_number == 1) {
            title = "random timing test";
            number_of_repeated_experiments = number_of_repeated_experiments_for_timing;
            accuracy_test = false;
            test_random = true, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, integer_range = 3, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 2) {
            title = "integer timing test";
            number_of_repeated_experiments = number_of_repeated_experiments_for_timing;
            accuracy_test = false;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = true; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 3) {
            title = "integer-perturbation timing test: 256 eps";
            number_of_repeated_experiments = number_of_repeated_experiments_for_timing;
            accuracy_test = false;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = true, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 4) {
            title = "identity-perturbation timing test: 1e-3";
            number_of_repeated_experiments = number_of_repeated_experiments_for_timing;
            accuracy_test = false;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 5) {
            title = "identity-perturbation timing test: 256 eps";
            number_of_repeated_experiments = number_of_repeated_experiments_for_timing;
            accuracy_test = false;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation_identity = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed itentity test
        }

        if (test_number == 6) {
            title = "random accuracy test";
            number_of_repeated_experiments = 1;
            accuracy_test = true;
            test_random = true, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, integer_range = 3, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 7) {
            title = "integer accuracy test";
            number_of_repeated_experiments = 1;
            accuracy_test = true;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = true; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 8) {
            title = "integer-perturbation accuracy test: 256 eps";
            number_of_repeated_experiments = 1;
            accuracy_test = true;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = true, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = false, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 9) {
            title = "identity-perturbation accuracy test: 1e-3";
            number_of_repeated_experiments = 1;
            accuracy_test = true;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = 1e-3, double_perturbation_identity = 1e-3; // perturbed itentity test
        }
        if (test_number == 10) {
            title = "identity-perturbation accuracy test: 256 eps";
            number_of_repeated_experiments = 1;
            accuracy_test = true;
            test_random = false, random_range = 3, number_of_random_cases = 1024 * 1024; // random test
            test_integer = false; // integer test
            integer_range = 2; // this variable is used by both integer test and perturbed integer test
            test_perturbation = false, perturbation_count = 4, float_perturbation = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed integer test
            test_perturbation_from_identity = true, perturbation_from_identity_count = 1024 * 1024, float_perturbation_identity = (float)256 * std::numeric_limits<float>::epsilon(), double_perturbation_identity = (double)256 * std::numeric_limits<double>::epsilon(); // perturbed itentity test
        }

        std::cout << " \n========== RUNNING BENCHMARK TEST == " << title << "=======" << std::endl;
        std::cout << " run_qr " << run_qr << std::endl;
        std::cout << " test_float " << test_float << std::endl;
        std::cout << " test_double " << test_double << std::endl;
        std::cout << " accuracy_test " << accuracy_test << std::endl;
        std::cout << " normalize_matrix " << normalize_matrix << std::endl;
        std::cout << " number_of_repeated_experiments " << number_of_repeated_experiments << std::endl;
        std::cout << " test_random " << test_random << std::endl;
        std::cout << " random_range " << random_range << std::endl;
        std::cout << " number_of_random_cases " << number_of_random_cases << std::endl;
        std::cout << " test_integer " << test_integer << std::endl;
        std::cout << " integer_range " << integer_range << std::endl;
        std::cout << " test_perturbation " << test_perturbation << std::endl;
        std::cout << " perturbation_count " << perturbation_count << std::endl;
        std::cout << " float_perturbation " << float_perturbation << std::endl;
        std::cout << " double_perturbation " << double_perturbation << std::endl;
        std::cout << " test_perturbation_from_identity " << test_perturbation_from_identity << std::endl;
        std::cout << " perturbation_from_identity_count " << perturbation_from_identity_count << std::endl;
        std::cout << " float_perturbation_identity " << float_perturbation_identity << std::endl;
        std::cout << " double_perturbation_identity " << double_perturbation_identity << std::endl;

        std::cout << std::setprecision(10) << "\n--- float test ---\n" << std::endl;
        if (test_float) {
            std::vector<Eigen::Matrix<float, 3, 3> > tests;
            if (test_integer)
                addIntegerCases(tests, integer_range);
            if (test_perturbation)
                addPerturbationCases(tests, integer_range, perturbation_count, float_perturbation);
            if (test_perturbation_from_identity)
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, float_perturbation_identity);
            if (test_random)
                addRandomCases(tests, (float)random_range, number_of_random_cases);
            if (normalize_matrix) {
                for (size_t i = 0; i < tests.size(); i++) {
                    float norm = tests[i].norm();
                    if (norm > (float)8 * std::numeric_limits<float>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if (run_qr)
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);

        }

        std::cout << std::setprecision(10) << "\n--- double test ---\n" << std::endl;
        if (test_double) {
            std::vector<Eigen::Matrix<double, 3, 3> > tests;
            if (test_integer)
                addIntegerCases(tests, integer_range);
            if (test_perturbation)
                addPerturbationCases(tests, integer_range, perturbation_count, double_perturbation);
            if (test_perturbation_from_identity)
                addPerturbationFromIdentityCases(tests, perturbation_from_identity_count, double_perturbation_identity);
            if (test_random)
                addRandomCases(tests, (double)random_range, number_of_random_cases);
            if (normalize_matrix) {
                for (size_t i = 0; i < tests.size(); i++) {
                    double norm = tests[i].norm();
                    if (norm > (double)8 * std::numeric_limits<double>::epsilon()) {
                        tests[i] /= norm;
                    }
                }
            }
            std::cout << std::setprecision(10) << "\n-----------" << std::endl;
            if (run_qr)
                runImplicitQRSVD(number_of_repeated_experiments, tests, accuracy_test);

        }
    }
}

float det(const Eigen::Matrix2f& A) {
    return A(0,0)*A(1,1) - A(1,0)*A(0,1);
}

void swapCols(Eigen::Matrix2f& A) {
    for(int i=0; i<2; i++) {
        float temp = A(i,0);
        A(i,0) = A(i,1);
        A(i,1) = temp;
    }
}

void My_SVD(const Eigen::Matrix2f& F,Eigen::Matrix2f& U,Eigen::Matrix2f& sigma,Eigen::Matrix2f& V){
//
//Compute the SVD of input F with sign conventions discussed in class and in assignment
//
//input: F
//output: U,sigma,V with F=U*sigma*V.transpose() and U*U.transpose()=V*V.transpose()=I

    using std::cout;
    using std::sqrt;
    using std::abs;
    const float tol = 32 * std::numeric_limits<float>::epsilon();

    // C  = F^T F
    Eigen::Matrix2f C;
    C.noalias() = F.transpose() * F;

    // First check if C is rank-deficient
    if(abs(det(C)) < tol) {
        // Check if C is zero
        if(C.norm() < tol) {
            U = Eigen::Matrix2f::Identity();
            V = Eigen::Matrix2f::Identity();
            sigma = Eigen::Matrix2f::Zero();
            return;
        }

        // Suppose our matrix is written as 
        // [ a  b
        //   b  d ]
        // At this point we know eigenvalues are 0 and a+d
        // Need to do slightly different work to get eigenvectors
        // depending on which of a, b, d are zero.
        // It's possible for one of a,d to be zero, but not both.
        // If any of a,d are zero, then b = 0
        float sigma_1 = sqrt(C(0,0) + C(1,1));
        float sigma_2 = 0;
        
        // If a is nonzero
        if(abs(C(0,0)) > tol) {
            // in this case (a, b)^T is an eigenvector
            float denom = sqrt( C(0,0)*C(0,0) + C(1,0)*C(1,0));
            V(0,0) = C(0,0)/denom;
            V(1,0) = C(1,0)/denom;
            // the second eigenvector is anything orthogonal to the first
            V(0,1) = -C(1,0)/denom;
            V(1,1) = C(0,0)/denom;
        }
        else {
            // in this case, (b,d)^T is an eigenvector
            float denom = sqrt( C(1,0)*C(1,0) + C(1,1)*C(1,1));
            V(0,0) = C(1,0)/denom;
            V(1,0) = C(1,1)/denom;
            // the second eigenvector is anything orthogonal to the first
            V(0,1) = -C(1,1)/denom;
            V(1,1) = C(1,0)/denom;
        }
        
        Eigen::Matrix2f A;
        A.noalias() = F*V; // So A = U*sigma. But sigma has only one nonzero entry.
                           // So really A = [ sigma_1 * u_11   0
                           //                 sigma_1 * u_21   0 ]
        U.col(0) = A.col(0)/sigma_1;
        U(0,1) = -U(1,0);
        U(1,1) = U(0,0);
        
        // Now we have A = U*Sigma*V^T. But still need that sigma_1 > abs(sigma_2).
        sigma << sigma_1, 0, 0, sigma_2;
        return;
    }

    // Apply eigensolver to C to get V, \sigma_1 and \sigma_2
    //       Use eigen self-adjoint solver
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix2f> esolve;
    esolve.compute(C);
    V = esolve.eigenvectors();
    
    // Set A = FV  (so A = U*Sigma)
    Eigen::Matrix2f A;
    A.noalias() = F*V;
    
    // Use givens rotation on A. The cos and sin should give U
    //            U = [ c  s
    //                 -s  c ]
    //         ImplicitQRSVD.h   should have givens code
    //    Here c is givens.c and s is givens.s
    JIXIE::GivensRotation<float> givens(A(0,0),A(1,0),0,1);
    U << givens.c, givens.s, -givens.s, givens.c;
    
    // Use that U.transpose()*A = Sigma to get the eigenvalues of F
    float sigma_1 = A(0,0)*givens.c - A(1,0)*givens.s;
    float sigma_2 = A(0,1)*givens.s + A(1,1)*givens.c;
    
    // Pass through U, V, Sigma to fix sign conventions.
    // in other words, this is the cleanup step
    if(abs(sigma_1)<abs(sigma_2)) {
        float temp = sigma_1;
        sigma_1 = sigma_2;
        sigma_2 = temp;
        swapCols(U);
        swapCols(V);
     }

    if(sigma_1 < 0) {
        sigma_1 *= -1;
        U.col(0) *= -1;
    }
    
    if(det(F) > 0 && sigma_2 < 0) {
        sigma_2 *= -1;
        U.col(1) *= -1;
    }
    else if (det(F) < 0 && sigma_2 > 0){
        sigma_2 *= -1;
        U.col(1) *= -1;
    }

    if(det(U)<0) {
        U.col(0) *= -1;
        V.col(0) *= -1;
    }

    sigma << sigma_1 ,0 , 0 , sigma_2;
    
}

template <class T>
T symError(const Eigen::Matrix<T,3,3>& F) {
    using std::fabs;
    return std::max(std::max(fabs(F(1,0)-F(0,1)), fabs(F(2,0)-F(0,2))), fabs(F(2,1)-F(1,2)));
}

void My_Polar(const Eigen::Matrix3f& F,Eigen::Matrix3f& R,Eigen::Matrix3f& S){
  //
  //Compute the polar decomposition of input F (with det(R)=1)
  //
  //input: F
  //output: R,s with F=R*S and R*R.transpose()=I and S=S.transpose()

    using std::sqrt;
    using std::cout;

    // Stop-loop conditions
    const long max_it = 100000;
    const float tol = 64 * std::numeric_limits<float>::epsilon();
    
    // Set up iteration. R = I,  S = F, it = 0
    typedef Eigen::Matrix<float, 3, 3> Matrix3;
    R = Matrix3::Identity();
    S = F;
    long it = 0;


    // Iterate
    while(it < max_it && symError(S) > tol) {
        for(int i=0; i<3; i++) {
            for(int j=i+1; j<3; j++) {
                JIXIE::GivensRotation<float> g( S(i,i)+S(j,j), S(j,i)-S(i,j), i, j);
                g.columnRotation(R);
                g.rowRotation(S);
            }
        }
        it++;
    }

}

// void Algorithm_2_Test(){
//
//   Eigen::Matrix2f F,C,U,V;
//   F<<1,2,3,4;
//   C=F*F.transpose();
//   Eigen::Vector2f s2;
//   JIXIE::Jacobi(C,s2,V);
//
// }

void my_benchmark_SVD();
void my_benchmark_Polar();

int main()
{
  bool run_benchmark = false;
  if (run_benchmark) runBenchmark();
  
  bool run_my_benchmark_SVD = false;
  bool run_my_benchmark_Polar = false;
  if (run_my_benchmark_SVD) my_benchmark_SVD();
  if (run_my_benchmark_Polar) my_benchmark_Polar();
  
  return 0;
}

void my_benchmark_SVD() {
    
  // Input: 2x2 matrix F
  //    For now just use a random matriix.
  srand((unsigned int) time(0));
  Eigen::Matrix2f F;
  int a = rand()%5-2;
  int b = rand()%5-2;
  int c = rand()%5-2;
  int d = rand()%5-2;
  F << a,b,c,d;
  Eigen::Matrix2f U,V,Sigma;
  
  My_SVD(F,U,Sigma,V);
  
  std::cout << "F =\n" << F
            << "\n\nSigma =\n" << Sigma
            << "\n\nU and V are (respectively)\n" << U << "\n\n" << V
            << "\n\nU*Sigma*V^T =\n" << U*Sigma*V.transpose()
            << "\n\ndet(U) = " << det(U) << ", det(V) = " << det(V)
            <<"\nsigma_1 >= abs(sigma_2) true or false: " << (Sigma(0,0) >= std::abs(Sigma(1,1)))
            << "\nError in \\ell^2 norm: " << (U*Sigma*V.transpose()-F).norm() << "\n\n";

}

void my_benchmark_Polar() {
    
  // Input: 3x3 matrix F
  //    For now just use a random matriix.
  srand((unsigned int) time(0));
  Eigen::Matrix3f F = Eigen::Matrix3f::Random();
  Eigen::Matrix3f R,S;
  
  My_Polar(F,R,S);
  
  std::cout << "Original Matrix F:\n" << F
            << "\n\nRotation R:\n" << R
            << "\n\nSymmetric S:\n" << S
            << "\nSymmetric Error in S: " << symError(S)
            << "\n\\ell^2 error of F-RS: " << (F-R*S).norm()
            << "\nnorm of R^T*R - I = " << (R*R.transpose()-Eigen::Matrix3f::Identity()).norm()
            << "\ndet(R) = " << R.determinant() << "\n";

}
