#include "sub_tmegodec.h"

//
// matrix
//

// X_mat: matrix with nchan x ntime

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
              double** const L_V_mat_ptr)
{

    long nelem_X  = nchan * ntime;
    printf("nelem_X = %ld\n", nelem_X);
    
    int incX = 1;
    int incY = 1;
    double* L_mat = new double [nelem_X];
    double* S_mat = new double [nelem_X];
    double* G_mat = new double [nelem_X];
    dcopy_(nelem_X, const_cast<double*>(X_mat), incX, L_mat, incY);
    dscal_(nelem_X, 0.0, S_mat, incX);
    dscal_(nelem_X, 0.0, G_mat, incX);

    double* L_U_mat = new double [nchan * nrank];
    double* L_S_vec = new double [nrank];
    double* L_V_mat = new double [ntime * nrank];
    dscal_(nchan * nrank, 0.0, L_U_mat, incX);
    dscal_(nrank, 0.0, L_S_vec, incX);
    dscal_(ntime * nrank, 0.0, L_V_mat, incX);
    
    MiRand::Init(seed);

    // Y1_mat: nchan x nrank
    // Y2_mat: ntime x nrank
    int nelem_Y1 = nchan * nrank;
    double* Y1_mat = new double [nelem_Y1];
    
    for(long ielem = 0; ielem < nelem_Y1; ielem ++){
        Y1_mat[ielem] = 0.0;
    }
    int nelem_Y2 = ntime * nrank;
    double* Y2_mat = new double [nelem_Y2];
    char* transa = new char [1];
    char* transb = new char [1];

    int lwork_QR1 = 0;
    int lwork_QR2 = 0;
    for(int iiter = 0; iiter < niter; iiter ++){

        // printf("iiter = %d\n", iiter);
        for(long ielem = 0; ielem < nelem_Y2; ielem ++){
            Y2_mat[ielem] = MiRand::Normal(0.0, 1.0);
        }
        for(int ipow = 0; ipow < qpar; ipow ++){
            // C := alpha * op(A) * op(B) + beta * C
            // op(A) : M x K
            // op(B) : K x N
            //    C  : M x N
            // ldA, ldB and ldC : first dimension of A, B and C.
            
            // dgemm_(transa, transb,
            //        M, N, K,
            //        alpha,
            //        A, ldA,
            //        B, ldB,
            //        beta,
            //        C, ldC)
           
            // Y1 = L %*% Y2
            strcpy(transa, "N");
            strcpy(transb, "N");
            dgemm_(transa, transb,
                   nchan, nrank, ntime,
                   1.0, L_mat, nchan, Y2_mat, ntime,
                   0.0, Y1_mat, nchan);
            // Y2 = t(L) %*% Y1
            strcpy(transa, "T");
            strcpy(transb, "N");
            dgemm_(transa, transb,
                   ntime, nrank, nchan,
                   1.0, L_mat, nchan, Y1_mat, nchan,
                   0.0, Y2_mat, ntime);
        }
        
        double* Q_mat = NULL;
        double* R_mat = NULL;
        // printf("ntime %d, nrank %d\n", ntime, nrank);
        MibLinalg::GenQR(Y2_mat, ntime, nrank,
                         &Q_mat, &R_mat,
                         &lwork_QR1, &lwork_QR2);
        
        double* L_new_mat = new double [nelem_X];
        double* Mul_mat = new double [nchan * nrank];
                
        // L.new = L %*% qr.Q(qr.ret) %*% t(qr.Q(qr.ret))
        //   Mul_mat = L %*% qr.Q(qr.ret)
        //   L.new   = Mul_mat %*% t(qr.Q(qr.ret))

        //   Mul_mat = L %*% qr.Q(qr.ret)
        strcpy(transa, "N");
        strcpy(transb, "N");
        dgemm_(transa, transb, 
               nchan, nrank, ntime,
               1.0, L_mat, nchan, Q_mat, ntime,
               0.0, Mul_mat, nchan);
        //   L.new   = Mul_mat %*% t(qr.Q(qr.ret))
        strcpy(transa, "N");
        strcpy(transb, "T");
        dgemm_(transa, transb,
               nchan, ntime, nrank,
               1.0, Mul_mat, nchan, Q_mat, ntime,
               0.0, L_new_mat, nchan);
        
        delete [] Mul_mat;

        // L.pre = L
        double* L_pre_mat = new double [nelem_X];
        dcopy_(nelem_X, L_mat, incX, L_pre_mat, incY);
        
        // T = L - L.new + S
        //   T_mat = L
        //   T_mat = - L.new + T_mat
        //   T_mat = S + T_mat
        double* T_mat = new double [nelem_X];
        // dcopy_(nelem_X, L_mat, incX, T_mat, incY);
        // daxpy_(nelem_X, -1.0, L_new_mat, incX, T_mat, incY);
        // daxpy_(nelem_X, 1.0, S_mat, incX, T_mat, incY);
        for(long ielem = 0; ielem < nelem_X; ielem ++){
            T_mat[ielem] = L_mat[ielem] - L_new_mat[ielem] + S_mat[ielem];
        }
        // L = L.new
        dcopy_(nelem_X, L_new_mat, incX, L_mat, incY);


        double* T_abs_mat = NULL;
        GenAbs(T_mat, nelem_X, &T_abs_mat);
        long* index_arr = new long [nelem_X];
        //MiSort::Sort<double, long>(nelem_X, T_abs_mat, index_arr, 1);
        MiSort::KthElement<double, long>(ncard, nelem_X, T_abs_mat, index_arr, 1);
        
        dscal_(nelem_X, 0.0, S_mat, incX);
        for(long icard = 0; icard < ncard; icard ++){
            S_mat[index_arr[icard]] = T_mat[index_arr[icard]];
        }
        for(long icard = 0; icard < ncard; icard ++){
            T_mat[index_arr[icard]] = 0.0;
        }

        // diff = norm((T), type=c("F")) / norm(X, type=c("F"))
        double norm_T = dnrm2_(nelem_X, T_mat, incX);
        double norm_X = dnrm2_(nelem_X, const_cast<double*>(X_mat), incX);
        double diff = norm_T / norm_X;
        printf("iiter = %d: diff = %e, epsilon = %e\n", iiter, diff, epsilon);
        if(diff < epsilon || iiter == niter - 1){
            // printf("diff (%e) < epsilon (%e), then break.\n", diff, epsilon);

            // LQ = L_pre %*% Q_mat
            double* LQ_mat = new double [nchan * nrank];
            strcpy(transa, "N");
            strcpy(transb, "N");
            dgemm_(transa, transb, 
                   nchan, nrank, ntime,
                   1.0, L_pre_mat, nchan, Q_mat, ntime,
                   0.0, LQ_mat, nchan);
            
            // *** SVD of LQ_mat ***
            // subroutine dgesvd(character JOBU, character JOBVT,
            // integer M,
            // integer N,
            // double precision, dimension( lda, * ) A,
            // integer LDA,
            // double precision, dimension( * ) S,
            // double precision, dimension( ldu, * ) U,
            // integer LDU,
            // double precision, dimension( ldvt, * ) VT,
            // integer LDVT,
            // double precision, dimension( * ) WORK,
            // integer LWORK,
            // integer INFO)

            int info_svd = 0;
           
            double* LQ_VT_mat = new double [nrank * nrank];
            char* jobu  = new char [1];
            char* jobvt = new char [1];            
            strcpy(jobu, "S");
            strcpy(jobvt, "S");

            // query of workspace
            int lwork_query = -1;
            double work_query[1];
            dgesvd_(jobu, jobvt, nchan, nrank, LQ_mat, nchan,
                    L_S_vec, L_U_mat, nchan, LQ_VT_mat, nrank,
                    work_query, lwork_query, info_svd);
            if(0 > info_svd){
                printf("ERROR: the %d-th argument had an illegal value.\n", -1 * info_svd);
                abort();
            } else if(0 < info_svd){
                printf("ERROR: info_svd(%d) > 0\n", info_svd);
                abort();
            }
            int lwork_svd = (int) work_query[0];
            printf("optimal lwork(SVD) = %d\n", lwork_svd);

            info_svd = 0;
            double* work_svd = new double [lwork_svd];
            dgesvd_(jobu, jobvt, nchan, nrank, LQ_mat, nchan,
                    L_S_vec, L_U_mat, nchan, LQ_VT_mat, nrank,
                    work_svd, lwork_svd, info_svd);

            if(0 > info_svd){
                printf("ERROR: the %d-th argument had an illegal value.\n", -1 * info_svd);
                abort();
            } else if(0 < info_svd){
                printf("ERROR: info_svd(%d) > 0\n", info_svd);
                abort();
            }
            delete [] work_svd;

            // L_V_mat = Q_mat * t(svd.VT);
            strcpy(transa, "N");
            strcpy(transb, "T");
            dgemm_(transa, transb, 
                   ntime, nrank, nrank,
                   1.0, Q_mat, ntime, LQ_VT_mat, nrank,
                   0.0, L_V_mat, ntime);

            delete [] LQ_mat;
            delete [] LQ_VT_mat;
            delete [] jobu;
            delete [] jobvt;
            delete [] L_pre_mat;
            delete [] Q_mat;
            delete [] R_mat;
            delete [] L_new_mat;
            delete [] T_mat;
            delete [] T_abs_mat;
            delete [] index_arr;
            
            break;
        }
        // L = L + T
        // daxpy_(nelem_X, 1.0, T_mat, incX, L_mat, incY);
        for(long ielem = 0; ielem < nelem_X; ielem ++){
            L_mat[ielem] += T_mat[ielem];
        }

        delete [] L_pre_mat;
        delete [] Q_mat;
        delete [] R_mat;        
        delete [] L_new_mat;
        delete [] T_mat;
        delete [] T_abs_mat;
        delete [] index_arr;

    }

    //    G = X - (L + S)
    for(long ielem = 0; ielem < nelem_X; ielem ++){
      G_mat[ielem] = X_mat[ielem] - (L_mat[ielem] + S_mat[ielem]);
    }
    
    //    G = X - (L + S)
    for(long ielem = 0; ielem < nelem_X; ielem ++){
      G_mat[ielem] = X_mat[ielem] - (L_mat[ielem] + S_mat[ielem]);
    }

    *S_mat_ptr = S_mat;
    *L_mat_ptr = L_mat;
    *G_mat_ptr = G_mat;
    *L_U_mat_ptr = L_U_mat;
    *L_S_vec_ptr = L_S_vec;
    *L_V_mat_ptr = L_V_mat;
}


void GenAbs(const double* const X_mat, long nelem,
            double** const X_abs_mat_ptr)
{
    double* X_abs_mat = new double [nelem];
    for(int ielem = 0; ielem < nelem; ielem ++){
        X_abs_mat[ielem] = fabs(X_mat[ielem]);
    }
    *X_abs_mat_ptr = X_abs_mat;
}
