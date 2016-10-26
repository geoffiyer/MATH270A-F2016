CXXFLAGS = -O3 -march=native -DNDEBUG -std=c++14
CXX = g++
EIGEN_INCLUDE = ./eigen3

a: main.cpp ImplicitQRSVD.h Tools.h
	$(CXX) $(CXXFLAGS) -o a main.cpp -I$(EIGEN_INCLUDE)

clean:
	rm -f a
