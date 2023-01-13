// InAPI.h

#ifdef INAPI_EXPORTS
#define INAPI_API	__declspec(dllexport)
#define INAPI_CLASS	__declspec(dllexport)
#else
#define INAPI_API	__declspec(dllimport)
#define INAPI_CLASS	__declspec(dllimport)
#endif
