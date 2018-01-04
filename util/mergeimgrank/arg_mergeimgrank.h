#ifndef MORIIISM_TME_UTIL_MERGEIMGRANK_ARG_MERGEIMGRANK_H_
#define MORIIISM_TME_UTIL_MERGEIMGRANK_ARG_MERGEIMGRANK_H_

#include "mi_base.h"

class ArgValMergeimgrank : public MiArgBase{
public:
    ArgValMergeimgrank() :
        MiArgBase(),
        progname_(""),
        fits_list_(""),
        sv_dat_(""),
        nrank_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValMergeimgrank(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetFitsList() const {return fits_list_;};
    string GetSvDat() const {return sv_dat_;};
    int    GetNrank() const {return nrank_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string fits_list_;
    string sv_dat_;
    int    nrank_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_TME_UTIL_MERGEIMGRANK_ARG_MERGEIMGRANK_H_
