// IQueryInfoImpl.h
//
//////////////////////////////////////////////////////////////////////
#include <AtlCom.h>
#include <ShlObj.h>



class ATL_NO_VTABLE IQueryInfoImpl : public IQueryInfo 
{
public:

	// IUnknown
	//
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	_ATL_DEBUG_ADDREF_RELEASE_IMPL(IQueryInfoImpl)


	// IQueryInfo 
	//

	// IQueryInfo::GetInfoTip
	STDMETHOD(GetInfoTip)(DWORD dwFlags, LPWSTR *ppwszTip)
	{
		wcscpy(*ppwszTip, L"InfoTip");
		return S_OK;
	}


	// IQueryInfo::GetInfoFlags
	STDMETHOD(GetInfoFlags)(LPDWORD pdwFlags)
	{ 
		*pdwFlags = 0;
		return E_NOTIMPL; 
	}

};


