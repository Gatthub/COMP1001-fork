/*
------------------DR VASILIOS KELEFOURAS-----------------------------------------------------
------------------COMP1001 ------------------------------------------------------------------
------------------COMPUTER SYSTEMS MODULE-------------------------------------------------
------------------UNIVERSITY OF PLYMOUTH, SCHOOL OF ENGINEERING, COMPUTING AND MATHEMATICS---
*/


#include <stdio.h>
#include <time.h>
#include <pmmintrin.h>
#include <process.h>
#include <chrono>
#include <iostream>
#include <immintrin.h>
#include <omp.h>

#define M 1024*512
#define ARITHMETIC_OPERATIONS1 3*M
#define TIMES1 1

#define N 8192
#define ARITHMETIC_OPERATIONS2 4*N*N
#define TIMES2 1


//function declaration
void initialize();
void routine1(float alpha, float beta);
void routine2(float alpha, float beta);
void routine1_vec(float alpha, float beta);
void routine2_vec(float alpha, float beta);
__declspec(align(64)) float  y[M], z[M] ;
__declspec(align(64)) float A[N][N], x[N], w[N];

int main() {

    float alpha = 0.023f, beta = 0.045f;
    double run_time, start_time;
    unsigned int t;

    initialize();

    printf("\nRoutine1:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES1; t++)
        routine1(alpha, beta);
        
    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS1) / ((double)run_time / TIMES1));

    printf("\nRoutine2:");
    start_time = omp_get_wtime(); //start timer

    for (t = 0; t < TIMES2; t++)
        routine2(alpha, beta);

    run_time = omp_get_wtime() - start_time; //end timer
    printf("\n Time elapsed is %f secs \n %e FLOPs achieved\n", run_time, (double)(ARITHMETIC_OPERATIONS2) / ((double)run_time / TIMES2));



    return 0;
}

void initialize() {

    unsigned int i, j;

    //initialize routine2 arrays
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            A[i][j] = (i % 99) + (j % 14) + 0.013f;
        }

    //initialize routine1 arrays
    for (i = 0; i < N; i++) {
        x[i] = (i % 19) - 0.01f;
        w[i] = (i % 5) - 0.002f;
    }

    //initialize routine1 arrays
    for (i = 0; i < M; i++) {
        z[i] = (i % 9) - 0.08f;
        y[i] = (i % 19) + 0.07f;
    }


}




void routine1(float alpha, float beta) {

    unsigned int i;


    for (i = 0; i < M; i++)
        y[i] = alpha * y[i] + beta * z[i];

}

void routine2(float alpha, float beta) {

    unsigned int i, j;


    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            w[i] = w[i] - beta + alpha * A[i][j] * x[j];


}


void routine1_vec(float alpha, float beta) {

    unsigned int i;
    __m256 alpha_loop = _mm256_set1_ps(alpha);
    __m256 beta_loop = _mm256_set1_ps(beta);
    for (i = 0; i < M; i += 8) {
        __m256 y_loop = _mm256_load_ps(&y[i]);
        __m256 z_loop = _mm256_load_ps(&z[i]); //loads the values from the array into a variable which can be used in the loop
        __m256 beta_loop = _mm256_set1_ps(beta);
        __m256 alpha_loop = _mm256_set1_ps(alpha); //resets the values of alpha_loop and beta_loop so it uses the right values in the future calculations rather than the value previously set to them in the last loop
        beta_loop = _mm256_mul_ps(beta_loop, z_loop); //Multiplies beta and z[i] and stores it into the beta_loop variable
        alpha_loop = _mm256_mul_ps(alpha_loop, y_loop);//multiplies alpha and y[i] and stores it into the alpha_loop variable
        y_loop = _mm256_add_ps(alpha_loop, beta_loop); //adds the previous two results together and stores it into the y_loop variable
        _mm256_store_ps(&y[i], y_loop); //Finally the result of the equation is stored into the corresponding value in the array
    }



}

void routine2_vec(float alpha, float beta){
    unsigned int i;
    unsigned int j;
    __m256 alpha_loop = _mm256_set1_ps(alpha);
    __m256 beta_loop = _mm256_set1_ps(beta);
     for (i = 0; i < N; i++) {
         __m256 w_loop = _mm256_load_ps(&w[i]);

         for (j = 0; j < N; j += 8) {
             __m256 A_loop = _mm256_load_ps(&A[i][j]);
             __m256 x_loop = _mm256_load_ps(&x[j]);
              A_loop = _mm256_mul_ps(A_loop, x_loop);
              alpha_loop = _mm256_mul_ps(alpha_loop, A_loop);
              w_loop = _mm256_add_ps(w_loop, alpha_loop);
         }
         w_loop = _mm256_sub_ps(w_loop, beta_loop);
         _mm256_store_ps(&w[i], w_loop);
    }

}
