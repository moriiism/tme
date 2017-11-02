#ifndef MORIIISM_TME_UTIL_GETRANK_ARG_GETRANK_H_
#define MORIIISM_TME_UTIL_GETRANK_ARG_GETRANK_H_

#include "mi_base.h"

class ArgValGetrank : public MiArgBase{
public:
    ArgValGetrank() :
        MiArgBase(),
        progname_(""),
        infile_(""),
        significance_(0.0),
        nclip_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValGetrank(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInfile() const {return infile_;};
    double GetSignificance() const {return significance_;};
    int    GetNclip() const {return nclip_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string infile_;
    double significance_;
    int    nclip_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_TME_UTIL_GETRANK_ARG_GETRANK_H_
