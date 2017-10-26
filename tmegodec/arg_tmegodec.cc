#include "arg_tmegodec.h"

// public

void ArgValTmegodec::Init(int argc, char* argv[])
{
    progname_ = "tmegodec";

    option long_options[] = {
        {"debug",       required_argument, NULL, 'd'},
        {"help",        required_argument, NULL, 'h'},
        {"verbose",     required_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    // long option default

    SetOption(argc, argv, long_options);
    
    printf("ArgVal::Init: # of arg = %d\n", argc - optind);
    int narg = 10;
    if (argc - optind != narg){
        printf("# of arguments must be %d.\n", narg);
        Usage(stdout);
    }
    int iarg = optind;
    in_file_        = argv[iarg]; iarg++;
    subcube_        = argv[iarg]; iarg++;
    nrank_          = atoi(argv[iarg]); iarg++;
    ncard_          = atoi(argv[iarg]); iarg++;
    qpar_           = atoi(argv[iarg]); iarg++;
    epsilon_        = atof(argv[iarg]); iarg++;
    niter_          = atoi(argv[iarg]); iarg++;
    seed_           = atoi(argv[iarg]); iarg++;
    outdir_         = argv[iarg]; iarg++;
    outfile_head_   = argv[iarg]; iarg++;
}

void ArgValTmegodec::Print(FILE* fp) const
{
    fprintf(fp, "%s: g_flag_debug   : %d\n", __func__, g_flag_debug);
    fprintf(fp, "%s: g_flag_help    : %d\n", __func__, g_flag_help);
    fprintf(fp, "%s: g_flag_verbose : %d\n", __func__, g_flag_verbose);

    fprintf(fp, "%s: progname_       : %s\n", __func__, progname_.c_str());
    fprintf(fp, "%s: in_file_        : %s\n", __func__, in_file_.c_str());
    fprintf(fp, "%s: subcube_        : %s\n", __func__, subcube_.c_str());
    fprintf(fp, "%s: nrank_          : %d\n", __func__, nrank_);
    fprintf(fp, "%s: ncard_          : %d\n", __func__, ncard_);
    fprintf(fp, "%s: qpar_           : %d\n", __func__, qpar_);
    fprintf(fp, "%s: epsilon_        : %f\n", __func__, epsilon_);
    fprintf(fp, "%s: niter_          : %d\n", __func__, niter_);
    fprintf(fp, "%s: seed_           : %ld\n", __func__, seed_);    
    fprintf(fp, "%s: outdir_         : %s\n", __func__, outdir_.c_str());
    fprintf(fp, "%s: outfile_head_   : %s\n", __func__, outfile_head_.c_str());
}

// private

void ArgValTmegodec::Null()
{
    progname_ = "";
    in_file_  = "";
    subcube_  = "";
    nrank_    = 0;
    ncard_    = 0;
    qpar_     = 0;
    epsilon_  = 0.0;
    niter_    = 0;
    seed_     = 0;
    outdir_   = "";
    outfile_head_ = "";
}

void ArgValTmegodec::SetOption(int argc, char* argv[], option* long_options)
{
    if(0 < g_flag_verbose){
        MPrintInfo("start...");
    }
    // option default
    g_flag_debug   = 0;
    g_flag_help    = 0;
    g_flag_verbose = 0;
    while (1) {
        int option_index = 0;
        int retopt = getopt_long(argc, argv, "dhv",
                                 long_options, &option_index);
        if(-1 == retopt)
            break;
        switch (retopt) {
        case 0:
            // long option
            break;
        case 'd':
            g_flag_debug = atoi(optarg);
            printf("%s: g_flag_debug = %d\n", __func__, g_flag_debug);
            break;
        case 'h':
            g_flag_help = atoi(optarg);
            printf("%s: g_flag_help = %d\n", __func__, g_flag_help);
            if(0 != g_flag_help){
                Usage(stdout);
            }
            break;
        case 'v':
            g_flag_verbose = atoi(optarg);
            printf("%s: g_flag_verbose = %d\n", __func__, g_flag_verbose);
            break;
        case '?':
            printf("%s: retopt (= %c) is invalid flag.\n",
                   __func__, retopt);
            Usage(stdout);
            break;
        default:
            printf("%s: error: getopt returned character code 0%o ??\n", __func__, retopt);
            abort();
        }
    }
    if(0 < g_flag_verbose){
        MPrintInfo("done.");
    }
}


void ArgValTmegodec::Usage(FILE* fp) const
{
    fprintf(fp,
            "usage: %s [--help (0)] [--verbose (0)] [--debug (0)] "
            "in_file  subcube  nrank  ncard  qpar  epsilon  niter  seed  outdir  outfile_head\n",
            progname_.c_str());
    abort();
}
