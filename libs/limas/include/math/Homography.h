#pragma once

namespace limas {
namespace math {

class Homography {
 public:
  template <class T>
  static glm::mat4 getHomography(const T &src, const T &dst) {
    float homography[16];
    findHomography(src, dst, homography);
    return glm::make_mat4(homography);
  }

 private:
  static void gaussianElimination(float *input, int n) {
    float *A = input;
    int i = 0;
    int j = 0;
    int m = n - 1;
    while (i < m && j < n) {
      int maxi = i;
      for (int k = i + 1; k < m; k++) {
        if (fabs(A[k * n + j]) > fabs(A[maxi * n + j])) {
          maxi = k;
        }
      }
      if (A[maxi * n + j] != 0) {
        if (i != maxi)
          for (int k = 0; k < n; k++) {
            float aux = A[i * n + k];
            A[i * n + k] = A[maxi * n + k];
            A[maxi * n + k] = aux;
          }
        float A_ij = A[i * n + j];
        for (int k = 0; k < n; k++) {
          A[i * n + k] /= A_ij;
        }
        for (int u = i + 1; u < m; u++) {
          float A_uj = A[u * n + j];
          for (int k = 0; k < n; k++) {
            A[u * n + k] -= A_uj * A[i * n + k];
          }
        }

        i++;
      }
      j++;
    }

    for (int i = m - 2; i >= 0; i--) {
      for (int j = i + 1; j < n - 1; j++) {
        A[i * n + m] -= A[i * n + j] * A[j * n + m];
      }
    }
  }

  template <class T>
  static void findHomography(T &src, T &dst, float homography[16]) {
    float P[8][9] = {
        {-src[0].x, -src[0].y, -1, 0, 0, 0, src[0].x * dst[0].x,
         src[0].y * dst[0].x, -dst[0].x},  // h11
        {0, 0, 0, -src[0].x, -src[0].y, -1, src[0].x * dst[0].y,
         src[0].y * dst[0].y, -dst[0].y},  // h12

        {-src[1].x, -src[1].y, -1, 0, 0, 0, src[1].x * dst[1].x,
         src[1].y * dst[1].x, -dst[1].x},  // h13
        {0, 0, 0, -src[1].x, -src[1].y, -1, src[1].x * dst[1].y,
         src[1].y * dst[1].y, -dst[1].y},  // h21

        {-src[2].x, -src[2].y, -1, 0, 0, 0, src[2].x * dst[2].x,
         src[2].y * dst[2].x, -dst[2].x},  // h22
        {0, 0, 0, -src[2].x, -src[2].y, -1, src[2].x * dst[2].y,
         src[2].y * dst[2].y, -dst[2].y},  // h23

        {-src[3].x, -src[3].y, -1, 0, 0, 0, src[3].x * dst[3].x,
         src[3].y * dst[3].x, -dst[3].x},  // h31
        {0, 0, 0, -src[3].x, -src[3].y, -1, src[3].x * dst[3].y,
         src[3].y * dst[3].y, -dst[3].y},  // h32
    };

    gaussianElimination(&P[0][0], 9);

    float aux_H[] = {P[0][8], P[3][8], 0, P[6][8],  // h11 h21 0 h31
                     P[1][8], P[4][8], 0, P[7][8],  // h12 h22 0 h32
                     0,       0,       1, 0,        // 0 0 0 0
                     P[2][8], P[5][8], 0, 1};       // h13 h23 0 h33

    for (int i = 0; i < 16; i++) homography[i] = aux_H[i];
  }
};

}  // namespace math
}  // namespace limas