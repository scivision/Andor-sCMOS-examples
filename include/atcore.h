#ifndef ATCORE_H
#define ATCORE_H

typedef int AT_H;
typedef int AT_BOOL;
#if defined(__BORLANDC__) && (__BORLANDC__<=0x540)
  typedef  __int64 AT_64;
#elif defined(_MSC_VER) && (_MSC_VER<=1310)
  typedef  __int64 AT_64;  
#else
  typedef  long long AT_64;
#endif
typedef unsigned char AT_U8;
typedef wchar_t AT_WC;

#if defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#define AT_EXP_CONV WINAPI
#else 
#define AT_EXP_CONV
#endif

#define AT_INFINITE 0xFFFFFFFF

#define AT_CALLBACK_SUCCESS 0

#define AT_TRUE 1
#define AT_FALSE 0

#define AT_SUCCESS 0
#define AT_ERR_NOTINITIALISED 1
#define AT_ERR_NOTIMPLEMENTED 2
#define AT_ERR_READONLY 3
#define AT_ERR_NOTREADABLE 4
#define AT_ERR_NOTWRITABLE 5
#define AT_ERR_OUTOFRANGE 6
#define AT_ERR_INDEXNOTAVAILABLE 7
#define AT_ERR_INDEXNOTIMPLEMENTED 8
#define AT_ERR_EXCEEDEDMAXSTRINGLENGTH 9
#define AT_ERR_CONNECTION 10
#define AT_ERR_NODATA 11
#define AT_ERR_INVALIDHANDLE 12
#define AT_ERR_TIMEDOUT 13
#define AT_ERR_BUFFERFULL 14
#define AT_ERR_INVALIDSIZE 15
#define AT_ERR_INVALIDALIGNMENT 16
#define AT_ERR_COMM 17
#define AT_ERR_STRINGNOTAVAILABLE 18
#define AT_ERR_STRINGNOTIMPLEMENTED 19

#define AT_ERR_NULL_FEATURE 20
#define AT_ERR_NULL_HANDLE 21
#define AT_ERR_NULL_IMPLEMENTED_VAR 22
#define AT_ERR_NULL_READABLE_VAR 23
#define AT_ERR_NULL_READONLY_VAR 24
#define AT_ERR_NULL_WRITABLE_VAR 25
#define AT_ERR_NULL_MINVALUE 26
#define AT_ERR_NULL_MAXVALUE 27
#define AT_ERR_NULL_VALUE 28
#define AT_ERR_NULL_STRING 29
#define AT_ERR_NULL_COUNT_VAR 30
#define AT_ERR_NULL_ISAVAILABLE_VAR 31
#define AT_ERR_NULL_MAXSTRINGLENGTH 32
#define AT_ERR_NULL_EVCALLBACK 33
#define AT_ERR_NULL_QUEUE_PTR 34
#define AT_ERR_NULL_WAIT_PTR 35
#define AT_ERR_NULL_PTRSIZE 36
#define AT_ERR_NOMEMORY 37
#define AT_ERR_DEVICEINUSE 38
#define AT_ERR_DEVICENOTFOUND 39

#define AT_ERR_HARDWARE_OVERFLOW 100

#define AT_HANDLE_UNINITIALISED -1
#define AT_HANDLE_SYSTEM 1

#ifdef __cplusplus
extern "C" {
#endif

 int AT_EXP_CONV AT_InitialiseLibrary();
 int AT_EXP_CONV AT_FinaliseLibrary();

 int AT_EXP_CONV AT_Open(int CameraIndex, AT_H *Hndl);
 int AT_EXP_CONV AT_Close(AT_H Hndl);

typedef int (AT_EXP_CONV *FeatureCallback)(AT_H Hndl, const AT_WC* Feature, void* Context);
 int AT_EXP_CONV AT_RegisterFeatureCallback(AT_H Hndl, const AT_WC* Feature, FeatureCallback EvCallback, void* Context);
 int AT_EXP_CONV AT_UnregisterFeatureCallback(AT_H Hndl, const AT_WC* Feature, FeatureCallback EvCallback, void* Context);

 int AT_EXP_CONV AT_IsImplemented(AT_H Hndl, const AT_WC* Feature, AT_BOOL* Implemented);
 int AT_EXP_CONV AT_IsReadable(AT_H Hndl, const AT_WC* Feature, AT_BOOL* Readable);
 int AT_EXP_CONV AT_IsWritable(AT_H Hndl, const AT_WC* Feature, AT_BOOL* Writable);
 int AT_EXP_CONV AT_IsReadOnly(AT_H Hndl, const AT_WC* Feature, AT_BOOL* ReadOnly);

 int AT_EXP_CONV AT_SetInt(AT_H Hndl, const AT_WC* Feature, AT_64 Value);
 int AT_EXP_CONV AT_GetInt(AT_H Hndl, const AT_WC* Feature, AT_64* Value);
 int AT_EXP_CONV AT_GetIntMax(AT_H Hndl, const AT_WC* Feature, AT_64* MaxValue);
 int AT_EXP_CONV AT_GetIntMin(AT_H Hndl, const AT_WC* Feature, AT_64* MinValue);

 int AT_EXP_CONV AT_SetFloat(AT_H Hndl, const AT_WC* Feature, double Value);
 int AT_EXP_CONV AT_GetFloat(AT_H Hndl, const AT_WC* Feature, double* Value);
 int AT_EXP_CONV AT_GetFloatMax(AT_H Hndl, const AT_WC* Feature, double* MaxValue);
 int AT_EXP_CONV AT_GetFloatMin(AT_H Hndl, const AT_WC* Feature, double* MinValue);

 int AT_EXP_CONV AT_SetBool(AT_H Hndl, const AT_WC* Feature, AT_BOOL Value);
 int AT_EXP_CONV AT_GetBool(AT_H Hndl, const AT_WC* Feature, AT_BOOL* Value);

 int AT_EXP_CONV AT_SetEnumerated(AT_H Hndl, const AT_WC* Feature, int Value);
 int AT_EXP_CONV AT_SetEnumeratedString(AT_H Hndl, const AT_WC* Feature, const AT_WC* String);
 int AT_EXP_CONV AT_GetEnumerated(AT_H Hndl, const AT_WC* Feature, int* Value);
 int AT_EXP_CONV AT_GetEnumeratedCount(AT_H Hndl,const  AT_WC* Feature, int* Count);
 int AT_EXP_CONV AT_IsEnumeratedIndexAvailable(AT_H Hndl, const AT_WC* Feature, int Index, AT_BOOL* Available);
 int AT_EXP_CONV AT_IsEnumeratedIndexImplemented(AT_H Hndl, const AT_WC* Feature, int Index, AT_BOOL* Implemented);
 int AT_EXP_CONV AT_GetEnumeratedString(AT_H Hndl, const AT_WC* Feature, int Index, AT_WC* String, int StringLength);

 int AT_EXP_CONV AT_SetEnumIndex(AT_H Hndl, const AT_WC* Feature, int Value);
 int AT_EXP_CONV AT_SetEnumString(AT_H Hndl, const AT_WC* Feature, const AT_WC* String);
 int AT_EXP_CONV AT_GetEnumIndex(AT_H Hndl, const AT_WC* Feature, int* Value);
 int AT_EXP_CONV AT_GetEnumCount(AT_H Hndl,const  AT_WC* Feature, int* Count);
 int AT_EXP_CONV AT_IsEnumIndexAvailable(AT_H Hndl, const AT_WC* Feature, int Index, AT_BOOL* Available);
 int AT_EXP_CONV AT_IsEnumIndexImplemented(AT_H Hndl, const AT_WC* Feature, int Index, AT_BOOL* Implemented);
 int AT_EXP_CONV AT_GetEnumStringByIndex(AT_H Hndl, const AT_WC* Feature, int Index, AT_WC* String, int StringLength);

 int AT_EXP_CONV AT_Command(AT_H Hndl, const AT_WC* Feature);

 int AT_EXP_CONV AT_SetString(AT_H Hndl, const AT_WC* Feature, const AT_WC* String);
 int AT_EXP_CONV AT_GetString(AT_H Hndl, const AT_WC* Feature, AT_WC* String, int StringLength);
 int AT_EXP_CONV AT_GetStringMaxLength(AT_H Hndl, const AT_WC* Feature, int* MaxStringLength);

 int AT_EXP_CONV AT_QueueBuffer(AT_H Hndl, AT_U8* Ptr, int PtrSize);
 int AT_EXP_CONV AT_WaitBuffer(AT_H Hndl, AT_U8** Ptr, int* PtrSize, unsigned int Timeout);
 int AT_EXP_CONV AT_Flush(AT_H Hndl);

#ifdef __cplusplus
}
#endif

#endif

