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
		(bstrInfo.Length() +1) * sizeof(WCHAR));
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
	TCHAR sizestr[20], datestr[30], szTemp[maxncols*nchr + 500], colsstr[maxncols*nchr];
	char clabs[MCOLUMNS][31], ctyps[MCOLUMNS][3], colstr[maxncols*nchr];
	int csetid[MCOLUMNS];

	HANDLE hFile = CreateFile(m_szFile, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_EXISTING, 0, NULL);
	if (hFile == NULL)
	{
		p->LoadString(IDS_TEXT_UNABLETOREAD);
		return S_OK;
	}	
	DWORD bsize = GetFileSize(hFile,  NULL);
	// Retrieve the file times for the file.
	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
		return S_OK;

	BOOL bFlag = CloseHandle(hFile);

	// Convert the last-write time to local time.
	FileTimeToSystemTime(&ftWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	// Build a string showing the date and time.
	StringCchPrintf(datestr, 30, "%02d/%02d/%d  %02d:%02d",
		stLocal.wMonth, stLocal.wDay, stLocal.wYear,
		stLocal.wHour, stLocal.wMinute);
	// format file size string
	TCHAR suffixes[7][4] = {{"B\0"}, {"KB\0"}, {"MB\0"}, {"GB\0"}, {"TB\0"}, {"PB\0"}};
	int s = 0;
	double size = (double)bsize;
	while (size >= 1000)
	{
		s++;
		size /= 1000;
	}
	StringCchPrintf(sizestr, 20, "%2.1f %s", size, suffixes[s]);

  CMtz::MTZ *mtzdata = CMtz::MtzGet(m_szFile, 0);
  // get name of the columns and the letter indicating their type
  int ncol = CMtz::MtzListColumn(mtzdata, clabs, ctyps, csetid);
  int i = 0;
  colstr[0] = 0;
  int nallowedcols = min(ncol, maxncols);
  while (i++ < nallowedcols)
  {
    char tmpstr[34];
    _strlwr_s(ctyps[i], 3); // lowercase column type to improve readability
    if ( strcmp(ctyps[i], "h")==0 ) // don't list H K L columns
      continue;
    if (i < (nallowedcols - 1))
      sprintf(tmpstr, "%s ", clabs[i]);
      //sprintf(tmpstr, "%s(%s) ", clabs[i], ctyps[i]);
    else
      if (i == (maxncols - 1) && i < (ncol - 1))
        //sprintf(tmpstr, "%s(%s) and %d other columns", clabs[i], ctyps[i], (ncol - maxncols));
        sprintf(tmpstr, "%s and %d other columns", clabs[i], (ncol - maxncols));
      else
        //sprintf(tmpstr, "%s(%s)", clabs[i], ctyps[i]);
        sprintf(tmpstr, "%s", clabs[i]);

    strcat(colstr, tmpstr);
  }
  wsprintf(colsstr, colstr);

  CMtz::MTZCOL *Hcol = CMtz::MtzColLookup(mtzdata, "H");
  int hmin = Hcol->min;
  int hmax = Hcol->max;
  CMtz::MTZCOL *Kcol = CMtz::MtzColLookup(mtzdata, "K");
  int kmin = Kcol->min;
  int kmax = Kcol->max;
  CMtz::MTZCOL *Lcol = CMtz::MtzColLookup(mtzdata, "L");
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

  int failure = MtzReadRefl(mtzdata, columns, logmss, lookup, ncols, mtzdata->nref);
  CSym::CCP4SPG* myspg = CSym::ccp4spg_load_by_standard_num(mtzdata->mtzsymm.spcgrp);
  int ncentrics = 0;
  for (int iref = 0; iref < mtzdata->nref; iref++)
  { // count centrics and get min max miller indices
    int h = columns[0][iref];
    hmin = min(h, hmin);
    hmax = max(h, hmax);
    int k = columns[1][iref];
    kmin = min(k, kmin);
    kmax = max(k, kmax);
    int l = columns[2][iref];
    lmin = min(l, lmin);
    lmax = max(l, lmax);
    ncentrics += CSym::ccp4spg_is_centric(myspg, h, k, l);
  }

  // Format the infotip string 
  hr = StringCchPrintf(szTemp, maxncols*nchr + 500 + 20 + 30,
    _T("Type: Structure Factors\n"
      "Reflections: %d\n"
      "H: [%d; %d], K: [%d; %d], L: [%d; %d]\n"
      "Point group: %s\n"
      "Space group, (number): %s (%d)\n"
      "Centrics: %d\n"
      "Resolution: %2.3f - %2.3fÅ\n"
      "Cell: %2.2fÅ %2.2fÅ %2.2fÅ  %2.2f° %2.2f° %2.2f°\n"
      "Columns: %s\n"
      "Size: %s\n"
      "Date modified: %s"),
    mtzdata->nref,
    hmin, hmax, kmin, kmax, lmin, lmax,
    mtzdata->mtzsymm.pgname,
    mtzdata->mtzsymm.spcgrpname,
    mtzdata->mtzsymm.spcgrp,
    ncentrics,
    1.0 / sqrt(mtzdata->xtal[0]->resmax),
    1.0 / sqrt(mtzdata->xtal[0]->resmin),
    mtzdata->xtal[0]->cell[0],
    mtzdata->xtal[0]->cell[1],
    mtzdata->xtal[0]->cell[2],
    mtzdata->xtal[0]->cell[3],
    mtzdata->xtal[0]->cell[4],
    mtzdata->xtal[0]->cell[5],
    colsstr,
    sizestr,
    datestr);

  if (FAILED(hr))
    return hr;

  MtzFree(mtzdata);
  p->Append(szTemp);

  return S_OK;
}

