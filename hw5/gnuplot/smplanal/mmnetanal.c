#include <stdio.h>

//===== Main program ==========================================================
void main(void)
{
  float Ts[4] = {0.2499999, 0.9999, 0.99999, 0.9999};	// Mean service time of server
  float p[4] = {0.25, 0.25, 0.25, 0.25};
  float lambda;
  int i;
  float rho_k[4];
  double L_k[4];
  double W_k[4];
  double W, L;

  for (lambda = 0.05; lambda < 1.01; lambda += 0.05) {
    W = 0;
    L = 0;
    float lambda_k[4] = {4 * lambda, lambda, lambda, lambda};
    for (i=0; i<4; i++) {
      rho_k[i] = lambda_k[i] * Ts[i];
      L_k[i] = rho_k[i] / (1 - rho_k[i]);
      W_k[i] = L_k[i] / lambda_k[i];
      W += W_k[i];
      L += L_k[i];
    }

  //  printf("%3.2f %10.3f %10.3f %10.3f %10.3f %10.3f\n", lambda, W_k[0], W_k[1], W_k[2], W_k[3], W);
    printf("%3.2f %10.3f %10.3f %10.3f %10.3f %10.3f\n", lambda, L_k[0], L_k[1], L_k[2], L_k[3], L);
  //  printf("%3.2f %10.3f %10.3f %10.3f %10.3f\n", lambda, rho_k[0], rho_k[1], rho_k[2], rho_k[3]);
//   printf("Lambda %3.2f, W1 %10.3f, W2 %10.3f, W3 %10.3f, W4 %10.3f, W %10.3f\n", lambda, W_k[0], W_k[1], W_k[2], W_k[3], W);
    //printf("Lambda %3.2f, L1 %10.3f, L2 %10.3f, L3 %10.3f, L4 %10.3f, L %10.3f\n", lambda, L_k[0], L_k[1], L_k[2], L_k[3], L);
    //printf("Lambda %3.2f, U1 %10.3f, U2 %10.3f, U3 %10.3f, U4 %10.3f\n", lambda, rho_k[0], rho_k[1], rho_k[2], rho_k[3]);
  }
}
