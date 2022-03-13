

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 03:14:07 2038
 */
/* Compiler settings for CACryptoProvider.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __CACryptoProvider_h__
#define __CACryptoProvider_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICATSDecryptor_FWD_DEFINED__
#define __ICATSDecryptor_FWD_DEFINED__
typedef interface ICATSDecryptor ICATSDecryptor;

#endif 	/* __ICATSDecryptor_FWD_DEFINED__ */


#ifndef __ICATSDecryptorProvider_FWD_DEFINED__
#define __ICATSDecryptorProvider_FWD_DEFINED__
typedef interface ICATSDecryptorProvider ICATSDecryptorProvider;

#endif 	/* __ICATSDecryptorProvider_FWD_DEFINED__ */


#ifndef __ICACryptoFactory_FWD_DEFINED__
#define __ICACryptoFactory_FWD_DEFINED__
typedef interface ICACryptoFactory ICACryptoFactory;

#endif 	/* __ICACryptoFactory_FWD_DEFINED__ */


#ifndef __ICACryptoProvider_FWD_DEFINED__
#define __ICACryptoProvider_FWD_DEFINED__
typedef interface ICACryptoProvider ICACryptoProvider;

#endif 	/* __ICACryptoProvider_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "crypto_ca.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_CACryptoProvider_0000_0000 */
/* [local] */ 

/********************************************************************
 File name: ModuleConfig.h
 Purpose: IModuleConfig interfaces definitions

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized reproduction or distribution of any portion is prohibited by law.
*********************************************************************/


////////////////////////////////////////////////////////////////////////////////
//
//    Original file name: CACryptoProvider.idl
//
////////////////////////////////////////////////////////////////////////////////

// {092A150B-3DA6-4C29-9F7F-719C40202F14}
static const GUID IID_ICACryptoFactory = 
 { 0x92a150b, 0x3da6, 0x4c29, { 0x9f, 0x7f, 0x71, 0x9c, 0x40, 0x20, 0x2f, 0x14 } };

// {BCB9E1F2-B517-4A42-B3C0-FDFD27FB5D78}
static const GUID IID_ICATSDecryptorProvider = 
 { 0xbcb9e1f2, 0xb517, 0x4a42, { 0xb3, 0xc0, 0xfd, 0xfd, 0x27, 0xfb, 0x5d, 0x78 } };

// {37F5B8B5-C338-4143-B81B-0562BD87CF11}
static const GUID IID_ICATSDecryptor = 
 { 0x37f5b8b5, 0xc338, 0x4143, { 0xb8, 0x1b, 0x5, 0x62, 0xbd, 0x87, 0xcf, 0x11 } };

// {E7CBF7A3-84BE-445C-A1E8-216D5D6DEC3F}
static const GUID IID_ICACryptoProvider = 
 { 0xe7cbf7a3, 0x84be, 0x445c, { 0xa1, 0xe8, 0x21, 0x6d, 0x5d, 0x6d, 0xec, 0x3f } };



extern RPC_IF_HANDLE __MIDL_itf_CACryptoProvider_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_CACryptoProvider_0000_0000_v0_0_s_ifspec;

#ifndef __ICATSDecryptor_INTERFACE_DEFINED__
#define __ICATSDecryptor_INTERFACE_DEFINED__

/* interface ICATSDecryptor */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ICATSDecryptor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("37F5B8B5-C338-4143-B81B-0562BD87CF11")
    ICATSDecryptor : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Decrypt( 
            /* [out][in] */ unsigned char *pBuffer,
            /* [in] */ unsigned int iBufferLength) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICATSDecryptorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICATSDecryptor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICATSDecryptor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICATSDecryptor * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Decrypt )( 
            ICATSDecryptor * This,
            /* [out][in] */ unsigned char *pBuffer,
            /* [in] */ unsigned int iBufferLength);
        
        END_INTERFACE
    } ICATSDecryptorVtbl;

    interface ICATSDecryptor
    {
        CONST_VTBL struct ICATSDecryptorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICATSDecryptor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICATSDecryptor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICATSDecryptor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICATSDecryptor_Decrypt(This,pBuffer,iBufferLength)	\
    ( (This)->lpVtbl -> Decrypt(This,pBuffer,iBufferLength) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICATSDecryptor_INTERFACE_DEFINED__ */


#ifndef __ICATSDecryptorProvider_INTERFACE_DEFINED__
#define __ICATSDecryptorProvider_INTERFACE_DEFINED__

/* interface ICATSDecryptorProvider */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ICATSDecryptorProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BCB9E1F2-B517-4A42-B3C0-FDFD27FB5D78")
    ICATSDecryptorProvider : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateStaticTSDecryptor( 
            /* [out] */ ICATSDecryptor **pDecryptor,
            /* [in] */ enum eCA_TS_Mode eCryptoAlgorithm,
            /* [in] */ enum eCA_TS_BlockTermMode eBlockTerminationMode,
            /* [in] */ const unsigned char *pKey,
            /* [in] */ unsigned int iKeyLength,
            /* [in] */ const unsigned char *pIV1,
            /* [in] */ unsigned int iIV1Length,
            /* [in] */ const unsigned char *pIV2,
            /* [in] */ unsigned int iIV2Length) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICATSDecryptorProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICATSDecryptorProvider * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICATSDecryptorProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICATSDecryptorProvider * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateStaticTSDecryptor )( 
            ICATSDecryptorProvider * This,
            /* [out] */ ICATSDecryptor **pDecryptor,
            /* [in] */ enum eCA_TS_Mode eCryptoAlgorithm,
            /* [in] */ enum eCA_TS_BlockTermMode eBlockTerminationMode,
            /* [in] */ const unsigned char *pKey,
            /* [in] */ unsigned int iKeyLength,
            /* [in] */ const unsigned char *pIV1,
            /* [in] */ unsigned int iIV1Length,
            /* [in] */ const unsigned char *pIV2,
            /* [in] */ unsigned int iIV2Length);
        
        END_INTERFACE
    } ICATSDecryptorProviderVtbl;

    interface ICATSDecryptorProvider
    {
        CONST_VTBL struct ICATSDecryptorProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICATSDecryptorProvider_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICATSDecryptorProvider_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICATSDecryptorProvider_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICATSDecryptorProvider_CreateStaticTSDecryptor(This,pDecryptor,eCryptoAlgorithm,eBlockTerminationMode,pKey,iKeyLength,pIV1,iIV1Length,pIV2,iIV2Length)	\
    ( (This)->lpVtbl -> CreateStaticTSDecryptor(This,pDecryptor,eCryptoAlgorithm,eBlockTerminationMode,pKey,iKeyLength,pIV1,iIV1Length,pIV2,iIV2Length) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICATSDecryptorProvider_INTERFACE_DEFINED__ */


#ifndef __ICACryptoFactory_INTERFACE_DEFINED__
#define __ICACryptoFactory_INTERFACE_DEFINED__

/* interface ICACryptoFactory */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ICACryptoFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("092A150B-3DA6-4C29-9F7F-719C40202F14")
    ICACryptoFactory : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDecryptor( 
            /* [out] */ ICATSDecryptor **pDecryptor,
            /* [in] */ const long prog_num,
            /* [in] */ const long pid) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICACryptoFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICACryptoFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICACryptoFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICACryptoFactory * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDecryptor )( 
            ICACryptoFactory * This,
            /* [out] */ ICATSDecryptor **pDecryptor,
            /* [in] */ const long prog_num,
            /* [in] */ const long pid);
        
        END_INTERFACE
    } ICACryptoFactoryVtbl;

    interface ICACryptoFactory
    {
        CONST_VTBL struct ICACryptoFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICACryptoFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICACryptoFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICACryptoFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICACryptoFactory_GetDecryptor(This,pDecryptor,prog_num,pid)	\
    ( (This)->lpVtbl -> GetDecryptor(This,pDecryptor,prog_num,pid) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICACryptoFactory_INTERFACE_DEFINED__ */


#ifndef __ICACryptoProvider_INTERFACE_DEFINED__
#define __ICACryptoProvider_INTERFACE_DEFINED__

/* interface ICACryptoProvider */
/* [unique][uuid][local][object] */ 


EXTERN_C const IID IID_ICACryptoProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E7CBF7A3-84BE-445C-A1E8-216D5D6DEC3F")
    ICACryptoProvider : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDecryptorProvider( 
            /* [out] */ ICATSDecryptorProvider **pDecryptor) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICACryptoProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICACryptoProvider * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICACryptoProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICACryptoProvider * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDecryptorProvider )( 
            ICACryptoProvider * This,
            /* [out] */ ICATSDecryptorProvider **pDecryptor);
        
        END_INTERFACE
    } ICACryptoProviderVtbl;

    interface ICACryptoProvider
    {
        CONST_VTBL struct ICACryptoProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICACryptoProvider_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICACryptoProvider_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICACryptoProvider_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICACryptoProvider_GetDecryptorProvider(This,pDecryptor)	\
    ( (This)->lpVtbl -> GetDecryptorProvider(This,pDecryptor) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICACryptoProvider_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


