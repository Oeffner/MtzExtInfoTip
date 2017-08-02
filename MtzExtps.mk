
MtzExtps.dll: dlldata.obj MtzExt_p.obj MtzExt_i.obj
	link /dll /out:MtzExtps.dll /def:MtzExtps.def /entry:DllMain dlldata.obj MtzExt_p.obj MtzExt_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del MtzExtps.dll
	@del MtzExtps.lib
	@del MtzExtps.exp
	@del dlldata.obj
	@del MtzExt_p.obj
	@del MtzExt_i.obj
