#include "mir_graph2d_nerr.h"
#include "mir_hist1d_nerr.h"
#include "mir_hist1d_ope.h"
#include "arg_mergelcrank.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValMergelcrank* argval = new ArgValMergelcrank;
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

    GraphDataNerr2d* gd2d = new GraphDataNerr2d;
    gd2d->Load(argval->GetSvDat());
    int nrank = argval->GetNrank();

    string* lines_hd1d_list = NULL;
    long nline_hd1d_list = 0;
    MiIolib::GenReadFileSkipComment(argval->GetHd1dList(),
                                    &lines_hd1d_list,
                                    &nline_hd1d_list);
    HistData1d** hd1d_arr = new HistData1d* [nline_hd1d_list];
    for(int iline = 0; iline < nline_hd1d_list; iline ++){
        hd1d_arr[iline] = new HistDataNerr1d;
        hd1d_arr[iline]->Load(lines_hd1d_list[iline]);
    }

    HistData1d** hd1d_scale_arr = new HistData1d* [nline_hd1d_list];
    for(int iline = 0; iline < nline_hd1d_list; iline ++){
        hd1d_scale_arr[iline] = new HistDataNerr1d;
        HistData1dOpe::GetScale(hd1d_arr[iline],
                                gd2d->GetOvalElm(iline), 0.0,
                                dynamic_cast<HistDataNerr1d*>(hd1d_scale_arr[iline]));
    }

    HistDataNerr1d* hd1d_merge = new HistDataNerr1d;
    HistData1dOpe::GetSum(hd1d_scale_arr, nrank, hd1d_merge);

    char outlc[kLineSize];
    sprintf(outlc, "%s/%s.dat",
            argval->GetOutdir().c_str(),
            argval->GetOutfileHead().c_str());
    hd1d_merge->Save(outlc, "x,y", 0.0, 0.0);


    delete gd2d;
    delete [] lines_hd1d_list;
    for(int iline = 0; iline < nline_hd1d_list; iline ++){
        delete hd1d_arr[iline];
        delete hd1d_scale_arr[iline];
    }
    delete [] hd1d_arr;
    delete [] hd1d_scale_arr;
    delete hd1d_merge;
    
    fclose(fp_log);
    delete argval;
    return status_prog;
}

