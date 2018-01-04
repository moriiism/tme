#include "mif_fits.h"
#include "mif_img_info.h"
#include "mir_graph2d_nerr.h"
#include "mir_hist2d_nerr.h"
#include "mir_hist2d_ope.h"
#include "arg_mergeimgrank.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValMergeimgrank* argval = new ArgValMergeimgrank;
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

    int nrank = argval->GetNrank();
    GraphDataNerr2d* gd2d = new GraphDataNerr2d;
    gd2d->Load(argval->GetSvDat());

    string* lines_fits_list = NULL;
    long nline_fits_list = 0;
    MiIolib::GenReadFileSkipComment(argval->GetFitsList(),
                                    &lines_fits_list,
                                    &nline_fits_list);

    int ndim = 2;
    long naxes[2];
    for(int idim = 0; idim < ndim; idim ++){
        naxes[idim] = MifFits::GetAxisSize(lines_fits_list[0], idim);
    }
    MifImgInfo* img_info = new MifImgInfo;
    img_info->InitSetImg(1, 1, naxes[0], naxes[1]);
    int bitpix = 0;    
    
    HistData2d** hd2d_arr = new HistData2d* [nrank];
    for(int irank = 0; irank < nrank; irank ++){
        double* data_arr = NULL;
        MifFits::InFitsImageD(lines_fits_list[irank],
                              img_info, &bitpix,
                              &data_arr);
        hd2d_arr[irank] = new HistDataNerr2d;
        hd2d_arr[irank]->Init(naxes[0], 0, naxes[0], naxes[1], 0, naxes[1]);
        hd2d_arr[irank]->SetOvalArr(naxes[0] * naxes[1], data_arr);
        delete [] data_arr;
    }
    HistData2d** hd2d_scale_arr = new HistData2d* [nrank];
    for(int irank = 0; irank < nrank; irank ++){
        hd2d_scale_arr[irank] = new HistDataNerr2d;
        HistData2dOpe::GetScale(hd2d_arr[irank],
                                gd2d->GetOvalElm(irank), 0.0,
                                dynamic_cast<HistDataNerr2d*>(hd2d_scale_arr[irank]));
    }

    HistDataNerr2d* hd2d_merge = new HistDataNerr2d;
    HistData2dOpe::GetSum(hd2d_scale_arr, nrank, hd2d_merge);

    char outimg[kLineSize];
    sprintf(outimg, "%s/%s.fit",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());

    MifFits::OutFitsImageD(argval->GetOutdir(), argval->GetOutfileHead(), "merge", 2,
                           bitpix, naxes,
                           hd2d_merge->GetOvalArr()->GetVal());

    fclose(fp_log);
    delete argval;
    return status_prog;
}

