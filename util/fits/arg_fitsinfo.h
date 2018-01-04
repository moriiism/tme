#ifndef MORIIISM_TME_UTIL_FITS_ARG_FITSINFO_H_
#define MORIIISM_TME_UTIL_FITS_ARG_FITSINFO_H_

#include "mi_base.h"

class ArgValFitsinfo : public MiArgBase{
public:
    ArgValFitsinfo() :
        MiArgBase(),
        progname_(""),
        infile_(""),
        outfile_("") {}
    ~ArgValFitsinfo(){
        Null();
    }
    void Init(int argc, char* argv[]);
    void Print(FILE* fp) const;

    string GetProgname() const {return progname_;};
    string GetInfile() const {return infile_;};
    string GetOutfile() const {return outfile_;};

private:
    string progname_;
    string infile_;
    string outfile_;

    void Null();
    void SetOption(int argc, char* argv[], option* long_options);
    void Usage(FILE* fp) const;
};

#endif // MORIIISM_TME_UTIL_FITS_ARG_FITSINFO_H_
