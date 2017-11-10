#include "Math/QuantFuncMathMore.h"
#include "mif_fits.h"
#include "mif_img_info.h"
#include "mi_time.h"
#include "mir_graph2d_nerr.h"
#include "mir_graph2d_serr.h"
#include "arg_getrank.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

void GetSvalue(long ndata, const double* const xval_arr,
               const double* const yval_arr, const double* const yval_err_arr,
               double* x0_ptr, double* S_1_ptr,
               double* S_x2_ptr, double* S_y_ptr,
               double* S_xy_ptr);

double GetButt(double delta_chi2, 
               double xval, double x0, double S_1, double S_x2,
               double S_y, double S_xy, int plus_minus);

double GetUpMinfcn(double value, string mode, int as_npar);

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValGetrank* argval = new ArgValGetrank;
    argval->Init(argc, argv);
    argval->Print(stdout);

    char logfile[kLineSize];
    if( MiIolib::TestFileExist(argval->GetOutdir()) ){
        char cmd[kLineSize];
        sprintf(cmd, "mkdir -p %s", argval->GetOutdir().c_str());
        system(cmd);
    }
    sprintf(logfile, "%s/%s_%s.log",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str(),
            argval->GetProgname().c_str());
    FILE* fp_log = fopen(logfile, "w");
    MiIolib::Printf2(fp_log, "-----------------------------\n");
    argval->Print(fp_log);

    GraphDataSerr2d* gd2d = new GraphDataSerr2d;
    gd2d->Load(argval->GetInfile(), "x,y");
    // set sigma_y = 1.0 for all data
    for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
        gd2d->SetPoint(idata,
                       gd2d->GetXvalElm(idata),
                       0.0,
                       gd2d->GetOvalElm(idata),
                       1.0);
    }

    DataArrayNerr1d* da1d_mask_sel = new DataArrayNerr1d;
    da1d_mask_sel->Init(gd2d->GetNdata());
    da1d_mask_sel->SetConst(1);

    double a_best = 0.0;
    double b_best = 0.0;
    double x0     = 0.0;
    for(int iclip = 0; iclip < argval->GetNclip(); iclip++){
        vector<double> xval_vec;
        vector<double> oval_vec;
        vector<double> oval_err_vec;
        for(long idata = 0; idata < gd2d->GetNdata(); idata++){
            if( da1d_mask_sel->GetValElm(idata) > 0){
                xval_vec.push_back(gd2d->GetXvalArr()->GetValElm(idata));
                oval_vec.push_back(gd2d->GetOvalArr()->GetValElm(idata));
                oval_err_vec.push_back(gd2d->GetOvalArr()->GetValSerrElm(idata));
            }
        }
        long narr = 0;
        double* xval_arr = NULL;
        double* oval_arr = NULL;
        double* oval_err_arr = NULL;        
        MiBase::GenArray(xval_vec, &narr, &xval_arr);
        MiBase::GenArray(oval_vec, &narr, &oval_arr);
        MiBase::GenArray(oval_err_vec, &narr, &oval_err_arr);
      
        x0   = 0.0;
        double S_1  = 0.0;
        double S_x2 = 0.0;
        double S_y  = 0.0;
        double S_xy = 0.0;
        GetSvalue(xval_vec.size(),
                  xval_arr,
                  oval_arr,
                  oval_err_arr,
                  &x0, &S_1, &S_x2, &S_y, &S_xy);
        MiBase::DelArray(xval_arr);
        MiBase::DelArray(oval_arr);
        MiBase::DelArray(oval_err_arr);
        
        a_best = S_xy / S_x2;
        b_best = S_y / S_1;
        printf("a_best = %e\n", a_best);
        printf("b_best = %e\n", b_best);

        // diff from best fit line
        double* diff_arr = new double [gd2d->GetNdata()];
        for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
            double xval = gd2d->GetXvalElm(idata);
            double yval_line = a_best * (xval - x0) + b_best;
            diff_arr[idata] = gd2d->GetOvalArr()->GetValElm(idata) - yval_line;
        }
        double stddev = MirMath::GetStddev(gd2d->GetNdata(), diff_arr);
        printf("stddev = %e\n", stddev);
    
        for(long idata = 0; idata < gd2d->GetNdata(); idata++){
            if( fabs(diff_arr[idata]) > argval->GetSignificance() * stddev){
                da1d_mask_sel->SetValElm(idata, 0);
            } else {
                da1d_mask_sel->SetValElm(idata, 1);
            }
        }

        long nbin_mask_sel = 0;
        for(long idata = 0; idata < gd2d->GetNdata(); idata++){
            if( da1d_mask_sel->GetValElm(idata) > 0){
                nbin_mask_sel ++;
            }
        }
        printf("nbin_mask_sel = %ld\n", nbin_mask_sel);
        
        if(nbin_mask_sel == (int) xval_vec.size()){
            break;
        }

    }

    // rank
    char outfile[kLineSize];
    sprintf(outfile, "%s/%s_rank.dat",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    FILE* fp_out = fopen(outfile, "w");
    int rank = 0;
    for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
        if(da1d_mask_sel->GetValElm(idata) > 0){
            break;
        }
        rank ++;        
    }
    fprintf(fp_out, "%d\n", rank);
    fprintf(fp_log, "rank = %d\n", rank);
    printf("rank = %d\n", rank);
    fclose(fp_out);
    
    char outqdp[kLineSize];
    sprintf(outqdp, "%s/%s.qdp",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    FILE* fp_qdp = fopen(outqdp, "w");
    fprintf(fp_qdp, "skip sing\n");
    fprintf(fp_qdp, "\n");
    for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
        fprintf(fp_qdp, "%e %e\n", gd2d->GetXvalElm(idata), gd2d->GetOvalElm(idata));
    }
    fprintf(fp_qdp, "\n");
    fprintf(fp_qdp, "no\n");
    fprintf(fp_qdp, "\n");
    for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
        double xval = gd2d->GetXvalElm(idata);
        double yval = a_best * (xval - x0) + b_best;
        fprintf(fp_qdp, "%e %e\n", xval, yval);
    }

    fprintf(fp_qdp, "\n");
    fprintf(fp_qdp, "no\n");
    fprintf(fp_qdp, "\n");
    for(long idata = 0; idata < gd2d->GetNdata(); idata ++){
        if(da1d_mask_sel->GetValElm(idata) > 0){
            double xval = gd2d->GetXvalElm(idata);
            double yval = a_best * (xval - x0) + b_best;
            fprintf(fp_qdp, "%e %e\n", xval, yval);
        }
    }
    fclose(fp_qdp);
    
    fclose(fp_log);
    delete argval;

    
    return status_prog;
}


void GetSvalue(long ndata, const double* const xval_arr,
               const double* const yval_arr, const double* const yval_err_arr,
               double* x0_ptr, double* S_1_ptr,
               double* S_x2_ptr, double* S_y_ptr,
               double* S_xy_ptr)
{
    double x0 = 0.0;
    double x0_err = 0.0; // not used
    long nsel = 0;       // not used
    int* mask_sel_arr = 0; // not used
    MirMath::GenWMean(ndata,
                      xval_arr,
                      yval_err_arr,
                      &x0, &x0_err,
                      &nsel, &mask_sel_arr);
    printf("x0 = %e\n", x0);

    double S_1  = 0.0;
    double S_x2 = 0.0;
    double S_y  = 0.0;
    double S_xy = 0.0;
    for(long idata = 0; idata < ndata; idata ++){
        double den = pow(yval_err_arr[idata], 2);
        S_1  += 1.0 / den;
        S_x2 += pow(xval_arr[idata] - x0, 2) / den;
        S_y  += yval_arr[idata] / den;
        S_xy += (yval_arr[idata] * (xval_arr[idata] - x0)) / den;
    }

    *x0_ptr = x0;
    *S_1_ptr = S_1;
    *S_x2_ptr = S_x2;
    *S_y_ptr = S_y;
    *S_xy_ptr = S_xy;
}

    
double GetButt(double delta_chi2, 
               double xval, double x0, double S_1, double S_x2,
               double S_y, double S_xy, int plus_minus)
{
    double ans = S_xy / S_x2 * (xval - x0) + S_y / S_1 +
        plus_minus * sqrt( delta_chi2 * ( pow(xval - x0, 2 ) / S_x2 + 1.0 / S_1 ) );
    return ans;
}


// mode: "sigma", "cl"
double GetUpMinfcn(double value, string mode, int as_npar)
{
    double confidence_level = 0.0;
    if("cl" == mode){
        confidence_level = value;
    } else if ("sigma" == mode){
        confidence_level = MirMath::Sigma2CL(value);
    }

    if(confidence_level < 0.0 || confidence_level > 1.0){
        char msg[kLineSize];
        sprintf(msg, "bad confidence_level (=%e).\n", confidence_level);
        MPrintErr(msg);
        abort();
    }
    double up_minfcn = ROOT::MathMore::chisquared_quantile(confidence_level, as_npar);
    return up_minfcn;
}
