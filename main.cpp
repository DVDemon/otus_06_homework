#include "matrix.h"
#include <cassert>

using namespace homework;

int main(int argc, char *argv[]){
    Matrix<int, -1> matrix;     // бесконечная матрица int заполнена значениями -1
    assert(matrix.size() == 0); // все ячейки свободны
    auto a = matrix[0][0];
    assert(a == -1);
    assert(matrix.size() == 0);
    matrix[100][100] = 314;
    assert(matrix[100][100] == 314);

    assert(matrix.size() == 1);
    // выведется одна строка// 100100314

    for(auto c: matrix){
        int x;
        int y;
        int v;
        std::tie(x, y, v) = c;
        std::cout << x << y << v << std::endl;
        //std::cout << c << std::endl;
        }


    std::cout << "Matrix diagonal test:" << std::endl;
    Matrix<int,-1> matrix2;
    for(int i=0;i<10;i++) matrix2[i][i] = i;
    for(int i=0;i<10;i++) matrix2[i][9-i] = 9-i;


    for(int i=1;i<=8;i++){
        for(int j=1;j<=8;j++){
            std::cout << matrix2[i][j] << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << "Matrix2 size:" << matrix2.size() << std::endl;
    std::cout << "Matrix2 elements:" << std::endl;
    for(auto c: matrix2){
        int x,y,v;
        std::tie(x,y,v) = c;
        std::cout << "matrix[" <<x <<","<<y << "]=" << v << std::endl;
        }

    UNUSED(argc);
    UNUSED(argv);
    return 0;
}