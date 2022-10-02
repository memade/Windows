#if !defined(INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__)
#define INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__


#define SK_VNAME(value) (#value)

#define SK_HEAP_ALLOC(x) ::HeapAlloc(::GetProcessHeap(), 0, (x))
#define SK_HEAP_FREE(x) {if(x)::HeapFree(::GetProcessHeap(), 0, (x));x=NULL;}


#define SK_CLASS_NEW(name) Class##name * temp = new Class#name();
#define SK_CLASS_MALLOC(name,outptr) auto p = new name(); outptr= reinterpret_cast<long long>(p);

#define SK_CLOSE_HANDLE(h) {if(h)::CloseHandle(h);h=nullptr;}
#define SK_NTCLOSE(h) {if(h)NtClose(h);h=nullptr;}
#define SK_DELETE_PTR(p) {if(p) delete p;p=nullptr;}
#define SK_DELETE_PTR_BUFFER(pbuf) {if(pbuf) delete[] pbuf;pbuf=nullptr;}
#define SK_FREE_LIBRARY(m) {if(m)::FreeLibrary(HMODULE(m));m=nullptr;}
#define SK_DELETE_PTR_C(p){if(p){::free(p);p=nullptr;}}

#define SK_COM_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

//!@ Compatible with JSON
#define SK_FUNCTION __FUNCTION__
#define SK_FUNCTIONW __FUNCTIONW__

#define SK_CRLF	"\r\n"

#define SK_SHARED_PTR(type,name,size) std::shared_ptr<type> name(new type[size],[](type *pbuffer){ delete[] pbuffer; })

#define SK_NET_STARTUP \
{\
WORD wVersionRequested;\
WSADATA wsaData;\
int err;\
wVersionRequested = MAKEWORD(2, 2);\
err = WSAStartup(wVersionRequested, &wsaData);\
if (err == 0) {\
if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {\
WSACleanup();\
}\
}\
}\

#define SK_NET_CLEANUP \
WSACleanup();\

#define SK_SHELL_CODE_PACKET(function,funend) std::string((char*)function, abs((char*)funend - (char*)function))

/// /*新生®（上海）**/
/// /*2022_02_19T12:53:21.8856425Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__
