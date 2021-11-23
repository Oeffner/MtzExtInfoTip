#include <strsafe.h>
#include <math.h>
#include <string.h>
#include "..\ccp4\cmtzlib.h"
#include "..\ccp4\csymlib.h"
#include "..\ccp4\mtzdata.h"
#include "..\ccp4\safe_mtz.h"
#include <tchar.h>

// Test program for debugging cmtzlib

int main(int argc, char* argv[])
{
  const int nchr = 40;
  const int maxncols = 40;
  char clabs[MCOLUMNS][31], ctyps[MCOLUMNS][3], colstr[maxncols * nchr], *m_szFile;
  int csetid[MCOLUMNS];
  long int ncentrics = 0;

  try
  {
    m_szFile = argv[1];
    //CMtz::MTZ* mtzdata = CMtz::MtzGet(m_szFile, 1);
    CMtz::MTZ* mtzdata = CMtz::MtzGetUserCellTolerance(m_szFile, false, 0.0);
    if (mtzdata)
    {
      // get name of the columns and the letter indicating their type
      int ncol = CMtz::MtzListColumn(mtzdata, clabs, ctyps, csetid);
      if (ncol > 0)
      {
        int i = 0;
        colstr[0] = 0;
        int nallowedcols = MIN(ncol, maxncols) - 1;
        while (i++ < nallowedcols)
        {
          char tmpstr[34];
          _strlwr_s(ctyps[i], 3); // lowercase column type to improve readability
          if (strcmp(ctyps[i], "h") == 0) // don't list H K L columns
            continue;
          if (i < (nallowedcols))
            sprintf(tmpstr, "%s, ", clabs[i]);
          //sprintf(tmpstr, "%s(%s) ", clabs[i], ctyps[i]);
          else
            if (i == (maxncols - 1) && i < (ncol - 1))
              //sprintf(tmpstr, "%s(%s) and %d other columns", clabs[i], ctyps[i], (ncol - maxncols));
              sprintf(tmpstr, "%s and %d other columns\n", clabs[i], (ncol - maxncols));
            else
            {
              //sprintf(tmpstr, "%s(%s)", clabs[i], ctyps[i]);
              sprintf(tmpstr, "%s\n", clabs[i]);
            }

          strcat(colstr, tmpstr);
        }
        printf(_T(colstr));

        CMtz::MTZCOL* Hcol = CMtz::MtzColLookup(mtzdata, "H");
        int hmin = Hcol->min;
        int hmax = Hcol->max;
        CMtz::MTZCOL* Kcol = CMtz::MtzColLookup(mtzdata, "K");
        int kmin = Kcol->min;
        int kmax = Kcol->max;
        CMtz::MTZCOL* Lcol = CMtz::MtzColLookup(mtzdata, "L");
        int lmin = Lcol->min;
        int lmax = Lcol->max;
        int ncols = 3;
        std::vector< std::vector<float> > columns;
        std::vector<int> logmss_(ncols);
        int* logmss = &*logmss_.begin();
        std::vector<CMtz::MTZCOL*> lookup_(ncols);
        CMtz::MTZCOL** lookup = &*lookup_.begin();
        lookup[0] = Hcol;
        lookup[1] = Kcol;
        lookup[2] = Lcol;

        for (int i = 0; i < mtzdata->nxtal; i++)
        {
          printf("Unit cell: %2.4fÅ, %2.4fÅ, %2.4fÅ, %2.2f°, %2.2f°, %2.2f°\n",
            mtzdata->xtal[i]->cell[0],
            mtzdata->xtal[i]->cell[1],
            mtzdata->xtal[i]->cell[2],
            mtzdata->xtal[i]->cell[3],
            mtzdata->xtal[i]->cell[4],
            mtzdata->xtal[i]->cell[5]
          );
        }
        int nxt = CMtz::MtzNxtal(mtzdata);
        printf("nxt = %d\n", nxt);
        CMtz::MTZXTAL *px= CMtz::MtzXtalLookup(mtzdata, "project");

        for (int i = 0; i < mtzdata->nxtal; i++)
        {
          for (int j = 0; j < mtzdata->xtal[i]->nset; j++)
          {
            double wl = mtzdata->xtal[i]->set[j]->wavelength;
            if (wl != 0.0)
            {
              printf("Wavelength: %1.5fÅ, ", wl);
            }
          }
        }

        // Format the infotip string 
        printf("Type: Structure Factors\n"
            "Reflections: %d\n"
            "H: [%d; %d], K: [%d; %d], L: [%d; %d]\n"
            "Point group: %s\n"
            "Space group: %s (%d)\n"
            "Resolution: %2.3f - %2.3fÅ\n",
          mtzdata->nref,
          hmin, hmax, kmin, kmax, lmin, lmax,
          mtzdata->mtzsymm.pgname,
          mtzdata->mtzsymm.spcgrpname,
          mtzdata->mtzsymm.spcgrp,
          1.0 / sqrt(mtzdata->xtal[0]->resmax),
          1.0 / sqrt(mtzdata->xtal[0]->resmin)
          );
      }

      MtzFree(mtzdata);
    }

  }

  catch (...)
  {
    printf("Error trying to read CCP4.MTZ file header");
    return -42;
  }

  return 0;
}

