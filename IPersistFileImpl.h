// IPersistFileImpl.h
//
//////////////////////////////////////////////////////////////////////
#include <AtlCom.h>


class ATL_NO_VTABLE IPersistFileImpl : public IPersistFile
{
public:

	TCHAR m_szFile[MAX_PATH];

	// IUnknown
	//
	STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;
	_ATL_DEBUG_ADDREF_RELEASE_IMPL( IPersistFileImpl )


	// IPersistFile
	//
	STDMETHOD(Load)(LPCOLESTR wszFile, DWORD dwMode)
	{
		USES_CONVERSION;
		_tcscpy(m_szFile, OLE2T((WCHAR*)wszFile)); 
		return S_OK;	
	};

	STDMETHOD(GetClassID)(LPCLSID)
	{ return E_NOTIMPL;	}

	STDMETHOD(IsDirty)(VOID)
	{ return E_NOTIMPL; }

	STDMETHOD(Save)(LPCOLESTR, BOOL)
	{ return E_NOTIMPL; }

	STDMETHOD(SaveCompleted)(LPCOLESTR)
	{ return E_NOTIMPL; }

	STDMETHOD(GetCurFile)(LPOLESTR FAR*)
	{ return E_NOTIMPL; }
};

