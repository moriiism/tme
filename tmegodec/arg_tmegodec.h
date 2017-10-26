#ifndef MORIIISM_TME_TMEGODEC_ARG_TMEGODEC_H_
#define MORIIISM_TME_TMEGODEC_ARG_TMEGODEC_H_

#include "mi_base.h"

class ArgValTmegodec : public MiArgBase{
public:
    ArgValTmegodec() :
        MiArgBase(),
        progname_(""),
        in_file_(""),
        subcube_(""),
        nrank_(0),
        ncard_(0),
        qpar_(0),
        epsilon_(0.0),
        niter_(0),
        seed_(0),
        outdir_(""),
        outfile_head_("") {}
    ~ArgValTmegodec(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInFile() const {return in_file_;};
    string GetSubcube() const {return subcube_;};
    int    GetNrank() const {return nrank_;};
    int    GetNcard() const {return ncard_;};
    int    GetQpar() const {return qpar_;};
    double GetEpsilon() const {return epsilon_;};
    int    GetNiter() const {return niter_;};
    long   GetSeed() const {return seed_;};
    string GetOutdir() const {return outdir_;};
    string GetOutfileHead() const {return outfile_head_;};

private:
    string progname_;
    string in_file_;
    string subcube_;
    int    nrank_;      // rank of output low-rank matrix
    int    ncard_;      // cardinarity of output sparse matrix
    int    qpar_;       // parameter of q
    double epsilon_;
    int    niter_;
    long   seed_;
    string outdir_;
    string outfile_head_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_TME_TMEGODEC_ARG_TMEGODEC_H_
