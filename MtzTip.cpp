// MtzTip.cpp : Implementation of CMtzTip
/*
The MIT License (MIT)

Copyright (c) 2017 Robert Oeffner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#include "stdafx.h"
#include "MtzExt.h"
#include "MtzTip.h"
#include <strsafe.h>
#include <math.h>
#include <string.h>
#include "ccp4\cmtzlib.h"
#include "ccp4\csymlib.h"
#include "ccp4\mtzdata.h"
#include "ccp4\safe_mtz.h"


/////////////////////////////////////////////////////////////////////////////
// CMtzTip

HRESULT CMtzTip::GetInfoTip(DWORD dwFlags, LPWSTR* ppwszTip)
{
	USES_CONVERSION;

	// dwFlags is currently unused.

	// At this point the shell is requiring the tip
	// for a certain Mtz file whose name is now 
	// stored in the m_szFile member of the 
	// IPersistFileImpl class. 

	// Opens the Mtz file and returns size and colors
	CComBSTR bstrInfo;
	GetStructurefactorInfo((CComBSTR *)&bstrInfo);
    
	*ppwszTip = (WCHAR*) m_pAlloc->Alloc( 
    (static_cast<SIZE_T>(bstrInfo.Length()) +1) * sizeof(WCHAR));
	if (*ppwszTip)
		wcscpy(*ppwszTip, (WCHAR*)(BSTR)bstrInfo);
	
	return S_OK;
}




/////////////////////////////////////////////////////////////////////////////
// Private Members
// Present MTZ header file information as an infotip when hovering the mouse over a file on Windows

HRESULT CMtzTip::GetStructurefactorInfo(CComBSTR *p)
{
	p->Empty();
  HRESULT hr = S_OK;
  FILETIME ftCreate, ftAccess, ftWrite;
  const int nchr = 40;
  const int maxncols = 40;
  SYSTEMTIME stUTC, stLocal;
  TCHAR sizestr[20], datestr[30], szTemp[maxncols * nchr + 5000], ucellstr[5000];
  char clabs[MCOLUMNS][31], ctyps[MCOLUMNS][3];
  int csetid[MCOLUMNS];
  long int ncentrics = 0;

  try
  {
    HANDLE hFile = CreateFile(m_szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if (hFile == NULL)
    {
      p->LoadString(IDS_TEXT_UNABLETOREAD);
      return S_OK;
    }
    DWORD bsize = GetFileSize(hFile, NULL);
    // Retrieve the file times for the file.
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
      return S_OK;

    BOOL bFlag = CloseHandle(hFile);

    // Convert the last-write time to local time.
    FileTimeToSystemTime(&ftWrite, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    // Build a string showing the date and time.
    StringCbPrintf(datestr, 30, _T("%02d/%02d/%d  %02d:%02d"),
      stLocal.wMonth, stLocal.wDay, stLocal.wYear,
      stLocal.wHour, stLocal.wMinute);
    // format file size string
    TCHAR suffixes[7][4] = { {_T("B\0")}, {_T("KB\0")}, {_T("MB\0")}, {_T("GB\0")}, {_T("TB\0")}, {_T("PB\0")} };
    int s = 0;
    double size = (double)bsize;
    while (size >= 1000)
    {
      s++;
      size /= 1000;
    }
    StringCbPrintf(sizestr, 20, _T("%2.1f %s"), size, suffixes[s]);

    // using MtzGetUserCellTolerance rather than MtzGet to pick up all unit cells that may be present
    CMtz::MTZ* mtzdata = CMtz::MtzGetUserCellTolerance(m_szFile, false, 0.0);
    if (mtzdata == NULL)
    {
      hr = StringCbPrintf(szTemp, 500 + 20 + 30,
        _T("Not a CCP4.MTZ file\n"
          "Size: %s\n"
          "Date modified: %s"),
        sizestr,
        datestr);
    }
    else
    {
      // get name of the columns and the letter indicating their type
      int ncol = CMtz::MtzListColumn(mtzdata, clabs, ctyps, csetid);
      if (ncol > 0)
      {
        CMtz::MTZCOL* Hcol = CMtz::MtzColLookup(mtzdata, "H");
        int hmin = Hcol->min;
        int hmax = Hcol->max;
        CMtz::MTZCOL* Kcol = CMtz::MtzColLookup(mtzdata, "K");
        int kmin = Kcol->min;
        int kmax = Kcol->max;
        CMtz::MTZCOL* Lcol = CMtz::MtzColLookup(mtzdata, "L");
        int lmin = Lcol->min;
        int lmax = Lcol->max;
        ucellstr[0] = L'\0';
        TCHAR mstr[500];
        for (int i = 0; i < mtzdata->nxtal; i++)
        {
          if (strncmp(mtzdata->xtal[i]->xname, "HKL_base", 8) != 0 && mtzdata->nxtal > 1)
          {
            StringCbPrintf(mstr, 300,
              _T("Unit cell: %2.3f≈, %2.3f≈, %2.3f≈, %2.2f∞, %2.2f∞, %2.2f∞\n"),
              mtzdata->xtal[i]->cell[0],
              mtzdata->xtal[i]->cell[1],
              mtzdata->xtal[i]->cell[2],
              mtzdata->xtal[i]->cell[3],
              mtzdata->xtal[i]->cell[4],
              mtzdata->xtal[i]->cell[5]
            );
            StringCbCat(ucellstr, 5000, mstr);
          }

          for (int j = 0; j < mtzdata->xtal[i]->nset; j++)
          {
            double wl = mtzdata->xtal[i]->set[j]->wavelength;
            if (wl != 0.0)
            {
              StringCbPrintf(mstr, 50, _T("Wavelength: %1.5f≈\n"), wl);
              StringCbCat(ucellstr, 5000, mstr);
            }
            bool columnlabel = false;
            for (int k = 0; k < mtzdata->xtal[i]->set[j]->ncol; k++)
            {
              char* lbl = mtzdata->xtal[i]->set[j]->col[k]->label;
              if (strcmp(lbl, "H") == 0 || strcmp(lbl, "K") == 0 || strcmp(lbl, "L") == 0)
                continue;

              if (!columnlabel)
              {
                StringCbCat(ucellstr, 5000, _T("Columns: "));
                columnlabel=true;
              }
              if ((k+1) < mtzdata->xtal[i]->set[j]->ncol)
                sprintf(mstr, "%s, ", mtzdata->xtal[i]->set[j]->col[k]->label);
              else
                sprintf(mstr, "%s", mtzdata->xtal[i]->set[j]->col[k]->label);
              StringCbCat(ucellstr, 5000, mstr);
            }
            if (columnlabel)
              StringCbCat(ucellstr, 5000, _T("\n"));
          }
        }
        
        // Format the infotip string 
        hr = StringCbPrintf(szTemp, maxncols * nchr + 1500,
          _T("Type: Reflection data (CCP4.MTZ)\n"
            "Reflections: %d\n"
            "H: [%d; %d], K: [%d; %d], L: [%d; %d]\n"
            "Point group: %s\n"
            "Space group: %s (%d)\n"
            "Resolution: %2.3f - %2.3f≈\n"
            "%s" // unitcell, wavelengths and columns from above
            "Size: %s\n"
            "Date modified: %s"),
          mtzdata->nref,
          hmin, hmax, kmin, kmax, lmin, lmax,
          mtzdata->mtzsymm.pgname,
          mtzdata->mtzsymm.spcgrpname,
          mtzdata->mtzsymm.spcgrp,
          1.0 / sqrt(mtzdata->xtal[0]->resmax),
          1.0 / sqrt(mtzdata->xtal[0]->resmin),
          ucellstr,
          sizestr,
          datestr);
      }
      else
        wsprintf(szTemp, "No columns in MTZ file");

      MtzFree(mtzdata);
    }

    if (FAILED(hr))
      return hr;
  }
  
  catch (...)
  {
    wsprintf(szTemp, "Error trying to read CCP4.MTZ file header");
  }
  
  p->Append(szTemp);

  return S_OK;
}

