CXXFLAGS = -O3 -march=native -DNDEBUG -std=c++14
CXX = g++
EIGEN_INCLUDE = ./eigen3

polar: main.cpp 3dPolar.h ImplicitQRSVD.h Tools.h
	$(CXX) $(CXXFLAGS) -o polar main.cpp -I$(EIGEN_INCLUDE)

clean:
	rm -f polar
