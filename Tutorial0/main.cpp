// 40166293
#include <iostream>

int** create2DArray(int rows, int cols){
    int** arr = new int*[rows];
    for(int i=0; i < rows; i++){
        arr[i] = new int[cols];
    }
    return arr;
}

void delete2DArray(int** arr, int rows) {
    for(int i=0; i < rows; i++){
        delete[] arr[i];
    }
    delete[] arr;
}


int main() {
    int rows;
    int cols = 4;

    std::cout << "Please input the number of rows: ";
    std::cin >> rows;

    int** arr = create2DArray(rows, cols);

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            arr[i][j] = i * cols + j;
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            std::cout << arr[i][j] << " ";
        }
        std::cout << std::endl;
    }

    delete2DArray(arr, rows);

    return 0;
}