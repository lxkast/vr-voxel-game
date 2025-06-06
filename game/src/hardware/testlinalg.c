//
// Created by sam on 06/06/2025.
//
#include <stdio.h>

#include "linalg.h"

void main() {
    double matrix[4][4] = {{1, 2, 3, 4}, {0, 1, 1, 3}, {-1, 0, 1, 2}, {-4, -1, 0, 1}};
    double matrix2[4][4];
    inverse4x4(matrix, matrix2);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%lf\t", matrix2[i][j]);
        }
        printf("\n");
    }
}