/*
commndline for building on Windows

cl /D "WIN32" /c MtzReadCrash.cpp && cl /D "WIN32" /c ccp4\cmtzlib.c  && cl /D "WIN32" /c ccp4\csymlib.c && cl /D "WIN32" /c ccp4\library_file.c  && cl /D "WIN32" /c ccp4\library_utils.c && cl /D "WIN32" /c ccp4\ccp4_array.c && cl /D "WIN32" /c ccp4\ccp4_parser.c && cl /D "WIN32" /c ccp4\library_err.c  && cl /D "WIN32" /c ccp4\ccp4_unitcell.c && cl /D "WIN32" /c ccp4\cvecmat.c && cl /D "WIN32" /c ccp4\ccp4_general.c  && cl /D "WIN32" /c ccp4\ccp4_program.c && cl /D "WIN32" /c ccp4\safe_mtz.cc  &&  link cmtzlib.obj csymlib.obj library_file.obj library_utils.obj ccp4_array.obj ccp4_parser.obj library_err.obj ccp4_unitcell.obj cvecmat.obj ccp4_general.obj ccp4_program.obj safe_mtz.obj Advapi32.lib MtzReadCrash.obj /OUT:MtzReadCrash.exe

*/



#include <string.h>
#include <vector>
#include "ccp4\cmtzlib.h"
#include "ccp4\safe_mtz.h"


int main(int argc, char* argv[], char* envp[])
{
  if (argc > 1)
  {
    CMtz::MTZ* mtzdata = CMtz::MtzGet(argv[1], 0);
    char clabs[MCOLUMNS][31], ctyps[MCOLUMNS][3], colstr[256];
    int csetid[MCOLUMNS];
    int ncol = CMtz::MtzListColumn(mtzdata, clabs, ctyps, csetid);

    CMtz::MTZCOL* Hcol = CMtz::MtzColLookup(mtzdata, "H");
    CMtz::MTZCOL* Kcol = CMtz::MtzColLookup(mtzdata, "K");
    CMtz::MTZCOL* Lcol = CMtz::MtzColLookup(mtzdata, "L");
    int ncols = 3;
    std::vector< std::vector<float> > columns;
    std::vector<int> logmss_(ncols);
    int* logmss = &*logmss_.begin();
    std::vector<CMtz::MTZCOL*> lookup_(ncols);
    CMtz::MTZCOL** lookup = &*lookup_.begin();
    lookup[0] = Hcol;
    lookup[1] = Kcol;
    lookup[2] = Lcol;

    int failure = MtzReadRefl(mtzdata, columns, logmss, lookup, ncols, mtzdata->nref);


    MtzFree(mtzdata);
  }
  return 0;
}