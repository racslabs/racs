/*
 * Copyright (c) 2024 Computer Music Research Lab (https://cmrlab.ai)
 *
 * This file is part of ZForm.
 *
 * ZForm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ZForm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZForm. If not, see <http://www.gnu.org/licenses/>.
 *
 * If you are using this library in a commercial, closed-source, or
 * proprietary project, you must purchase a ZForm license.
 */

#include "linalg.hpp"

namespace racs::simd {

void transpose(float *a, float *b, int i_s, int j_s, int i_e, int j_e, int n, int m) {
    int n1 = i_e - i_s + 1;
    int m1 = j_e - j_s + 1;
    int k = n1 * m1;

    if (k <= 16) {
        for (int i = i_s; i <= i_e; i++) {

            RACS_UNROLL(8)
            for (int j = j_s; j <= j_e; j++) {
                b[j * n + i] = a[i * m + j];
            }
        }
    } else {
        if (n1 >= m1) {
            int mid = (i_s + i_e) / 2;

            transpose(a, b, i_s, j_s, mid, j_e, n, m);
            transpose(a, b, mid + 1, j_s, i_e, j_e, n, m);

        } else {
            int mid = (j_s + j_e) / 2;

            transpose(a, b, i_s, j_s, i_e, mid, n, m);
            transpose(a, b, i_s, mid + 1, i_e, j_e, n, m);
        }
    }
}

void transpose(std::complex<float> *a, std::complex<float> *b, int i_s, int j_s, int i_e, int j_e, int n, int m) {
    int n1 = i_e - i_s + 1;
    int m1 = j_e - j_s + 1;
    int k = n1 * m1;

    if (k <= 8) {
        for (int i = i_s; i <= i_e; i++) {

            RACS_UNROLL(4)
            for (int j = j_s; j <= j_e; j++) {
                b[j * n + i] = a[i * m + j];
            }
        }
    } else {
        if (n1 >= m1) {
            int mid = (i_s + i_e) / 2;

            transpose(a, b, i_s, j_s, mid, j_e, n, m);
            transpose(a, b, mid + 1, j_s, i_e, j_e, n, m);

        } else {
            int mid = (j_s + j_e) / 2;

            transpose(a, b, i_s, j_s, i_e, mid, n, m);
            transpose(a, b, i_s, mid + 1, i_e, j_e, n, m);
        }
    }
}

}