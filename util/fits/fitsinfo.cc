#include "mif_fits.h"
#include "mif_img_info.h"
#include "mi_time.h"
#include "mir_graph2d_nerr.h"
#include "mir_graph2d_serr.h"
#include "arg_fitsinfo.h"

// global variable 
int g_flag_debug = 0;
int g_flag_help = 0;
int g_flag_verbose = 0;

int main(int argc, char* argv[])
{
    int status_prog = kRetNormal;
    
    ArgValFitsinfo* argval = new ArgValFitsinfo;
    argval->Init(argc, argv);
    argval->Print(stdout);

    FILE* fp = fopen(argval->GetOutfile().c_str(), "w");
    int naxis = MifFits::GetNaxis(argval->GetInfile());
    fprintf(fp, "# naxis = %d\n", naxis);
    fprintf(fp, "# idim, size\n");
    for(int iaxis = 0; iaxis < naxis; iaxis ++){
        int size = MifFits::GetAxisSize(argval->GetInfile(), iaxis);
        fprintf(fp, "%d  %d\n", iaxis, size);
    }
    fclose(fp);
    
    return status_prog;
}

