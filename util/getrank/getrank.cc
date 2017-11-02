#include "mif_fits.h"
#include "mif_img_info.h"
#include "mi_time.h"
#include "arg_getrank.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

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

    GraphDataNerr2d* gd2d = new GraphDataNerr2d;
    gd2d->Load(argval->GetInfile());

    MirFunc* func = new LinMidFunc;
//     double ans = (xval[0] - par[2]) * par[1] + par[0];

    

    


    
    
    fclose(fp_log);
    delete argval;

    
    return status_prog;
}
