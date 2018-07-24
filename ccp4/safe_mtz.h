#ifndef __PHASER_SAFE_MTZ__
#define __PHASER_SAFE_MTZ__
#include <string> 
#include <vector> 
#include "cmtzlib.h"
#include "mtzdata.h"


CMtz::MTZ* safe_mtz_get(std::string const& logname,int read_refs);
CMtz::MTZCOL* safe_mtz_col_lookup(const CMtz::MTZ* mtzfrom,std::string const& label);
int MtzReadRefl(CMtz::MTZ *mtz, std::vector< std::vector<float> > &columns, 
  int logmss[], CMtz::MTZCOL *lookup[], int ncols, int nrefs);

inline bool mtz_isnan(float const& value)
{ return CCP4::ccp4_utils_isnan((union float_uint_uchar *)&value); }



#endif

