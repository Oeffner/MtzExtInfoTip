
// Modified version of phaser\codebase\phaser\lib\safe_mtz.h
// for reading column data into array of floats

#include <string> 
#include <vector> 
#include "cmtzlib.h"
#include "mtzdata.h"


int MtzReadRefl(CMtz::MTZ *mtz, std::vector< std::vector<float> > &columns, 
  int logmss[], CMtz::MTZCOL *lookup[], int ncols, int nrefs);

