#ifndef MORIIISM_TME_UTIL_MERGELCRANK_ARG_MERGELCRANK_H_
#define MORIIISM_TME_UTIL_MERGELCRANK_ARG_MERGELCRANK_H_

#include "mi_base.h"

class ArgValMergelcrank : public MiArgBase{
public:
    ArgValMergelcrank() :
        MiArgBase(),
        progname_(""),
        hd1d_list_(""),
        sv_dat_(""),
        nrank_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValMergelcrank(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetHd1dList() const {return hd1d_list_;};
    string GetSvDat() const {return sv_dat_;};
    int    GetNrank() const {return nrank_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string hd1d_list_;
    string sv_dat_;
    int    nrank_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_TME_UTIL_MERGELCRANK_ARG_MERGELCRANK_H_
