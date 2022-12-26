#if !defined(INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__)
#define INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__

#define EXPORT_API_OBJECT_INIT \
extern "C" __declspec(dllexport) \
void* \
api_object_init(const void* route,unsigned long route_size)\

#define EXPORT_API_OBJECT_UNINIT \
extern "C" __declspec(dllexport) \
void \
api_object_uninit(void)\

#define SK_VNAME(value) (#value)

#define SK_HEAP_ALLOC(x) ::HeapAlloc(::GetProcessHeap(), 0, (x))
#define SK_HEAP_FREE(x) {if(x)::HeapFree(::GetProcessHeap(), 0, (x));x=NULL;}


#define SK_CLASS_NEW(name) Class##name * temp = new Class#name();
#define SK_CLASS_MALLOC(name,outptr) auto p = new name(); outptr= reinterpret_cast<long long>(p);

#define SK_CLOSE_HANDLE(h) {if(h)::CloseHandle(h);h=nullptr;}
#define SK_CLOSE_SERVICE_HANDLE(h) {if(h)::CloseServiceHandle(h);h=nullptr;}
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


#if 0
作者：Zhao Zhang
链接：https ://www.zhihu.com/question/27338446/answer/36655304
来源：知乎
著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。
#endif
#define Y_TUPLE_SIZE_II(__args) Y_TUPLE_SIZE_I __args
#define Y_TUPLE_SIZE_PREFIX__Y_TUPLE_SIZE_POSTFIX ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,0
#define Y_TUPLE_SIZE_I(__p0,__p1,__p2,__p3,__p4,__p5,__p6,__p7,__p8,__p9,__p10,__p11,__p12,__p13,__p14,__p15,__p16,__p17,__p18,__p19,__p20,__p21,__p22,__p23,__p24,__p25,__p26,__p27,__p28,__p29,__p30,__p31,__n,...) __n
#define MPL_ARGS_SIZE(...) Y_TUPLE_SIZE_II((Y_TUPLE_SIZE_PREFIX_ ## __VA_ARGS__ ## _Y_TUPLE_SIZE_POSTFIX,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

/// /*新生®（上海）**/
/// /*2022_02_19T12:53:21.8856425Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___0D7B2A4F_D499_4666_9F6E_20C69A87BFF2__HEAD__
