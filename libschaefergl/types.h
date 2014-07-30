#ifndef TYPES_H
#define TYPES_H

#if ! (defined _GUID_DEFINED || defined GUID_DEFINED) /* also defined in winnt.h */
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID,*REFGUID,*LPGUID;
#endif /* GUID_DEFINED */

typedef unsigned long ULONG;
typedef signed long LONG;
typedef LONG HRESULT;
typedef GUID IID;
typedef IID *REFIID;

#endif // TYPES