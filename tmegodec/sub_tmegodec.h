#ifndef MORIIISM_TME_TMEGODEC_SUB_TMEGODEC_H_
#define MORIIISM_TME_TMEGODEC_SUB_TMEGODEC_H_

#include "mi_rand.h"
#include "mib_linalg.h"
#include "mi_sort.h"

void GenGodec(const double* const X_mat,
              long nchan,
              long ntime,
              long nrank,
              long ncard,
              int  qpar,
              double epsilon,
              long niter,
              long seed,
              double** const S_mat_ptr,
              double** const L_mat_ptr,
              double** const G_mat_ptr,
              double** const L_U_mat_ptr,
              double** const L_S_vec_ptr,
              double** const L_V_mat_ptr);

void GenMatThresNcard(const double* const X_mat, long nelem,
                      long ncard, double** const X_th_mat_ptr);

void GenAbs(const double* const X_mat, long nelem,
            double** const X_abs_mat_ptr);


#endif // MORIIISM_TME_TMEGODEC_SUB_TMEGODEC_H_
