// MtzTip.h : Declaration of the CMtzTip
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

#ifndef __MtzTIP_H_
#define __MtzTIP_H_

#include "resource.h"              // main symbols
#include "comdef.h"				   // GUIDs	
#include "IPersistFileImpl.h"      // IPersistFile
#include "IQueryInfoImpl.h"        // IQueryInfo
#include <shlguid.h> // This carries IID_IQueryInfo

/////////////////////////////////////////////////////////////////////////////
// CMtzTip
class ATL_NO_VTABLE CMtzTip : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMtzTip, &CLSID_MtzTip>,
	public IQueryInfoImpl,
	public IPersistFileImpl,
	public IDispatchImpl<IMtzTip, &IID_IMtzTip, &LIBID_MtzEXTLib>
{
public:
	CMtzTip()
	{
		HRESULT hr;
		hr = SHGetMalloc(&m_pAlloc);
		if (FAILED(hr))
			m_pAlloc = NULL;
	}
	~CMtzTip()
	{
		m_pAlloc->Release();
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MtzTIP)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CMtzTip)
	COM_INTERFACE_ENTRY(IMtzTip)
	COM_INTERFACE_ENTRY_IID(IID_IQueryInfo, IQueryInfo)
	//COM_INTERFACE_ENTRY(IQueryInfo)
	COM_INTERFACE_ENTRY(IPersistFile)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IQueryInfo
public:
	STDMETHOD(GetInfoTip)(DWORD, LPWSTR*);

private:
	STDMETHOD(GetStructurefactorInfo)(CComBSTR*);

private:
	LPMALLOC m_pAlloc;
};

#endif //__MtzTIP_H_
