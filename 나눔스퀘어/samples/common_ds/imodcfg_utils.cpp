/********************************************************************
 Created: 2009/08/12 
 File name: imodcfg_utils.cpp
 Purpose: IModuleConfig programming utilities

 Copyright (c) 2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include "imodcfg_utils.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) {if (p) {p->Release (); p = NULL;}}
#endif

//
// In general see the "MainConcept IModuleConfig DS Documentation.pdf" guide
// for further information regarding the following functions.
//

HRESULT GetIMCValue( IBaseFilter *pFilter, const GUID &paramGuid, VARTYPE &varType, void *value )
{
    if( !pFilter )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            hr = pParamConfig->GetValue(&variant,OATRUE);
            if( SUCCEEDED(hr) )
            {
                varType = variant.vt;
                if( value ) // that way the function can be called with a NULL pointer in order to ask for the matching type
                {           // anyway the type is known from the filter's "xyz_mc.h" API header!
                    switch( varType )
                    {
                        case VT_BOOL:
                        case VT_I1:
                        case VT_UI1:     *(reinterpret_cast<BYTE*>(value))      = variant.bVal;   break;
                        case VT_I2:      *(reinterpret_cast<SHORT*>(value))     = variant.iVal;   break;
                        case VT_UI2:     *(reinterpret_cast<USHORT*>(value))    = variant.uiVal;  break;
                        case VT_I4:
                        case VT_INT:     *(reinterpret_cast<LONG*>(value))      = variant.lVal;   break;
                        case VT_ERROR:
                        case VT_UI4:
                        case VT_UINT:
                        case VT_HRESULT: *(reinterpret_cast<ULONG*>(value))     = variant.ulVal;  break;
                        case VT_I8:      *(reinterpret_cast<LONGLONG*>(value))  = variant.llVal;  break;
                        case VT_UI8:     *(reinterpret_cast<ULONGLONG*>(value)) = variant.ullVal; break;
                        case VT_R4:      *(reinterpret_cast<FLOAT*>(value))     = variant.fltVal; break;
                        case VT_R8:      *(reinterpret_cast<DOUBLE*>(value))    = variant.dblVal; break;
                        case VT_BSTR:    // please use GetIMCStringValue() instead
                        case VT_UINT_PTR:
                        case VT_PTR:
                        case VT_BYREF:
                        default: hr = E_UNEXPECTED; break;
                    }
                }
            }
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}

HRESULT SetIMCValue( IBaseFilter *pFilter, const GUID &paramGuid, const VARTYPE varType, void *value )
{
    if( !pFilter || !value )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            variant.vt = varType;
            switch( varType )
            {
                case VT_BOOL:
                case VT_I1:
                case VT_UI1:     variant.bVal    = *(reinterpret_cast<BYTE*>(value));      break;
                case VT_I2:      variant.iVal    = *(reinterpret_cast<SHORT*>(value));     break;
                case VT_UI2:     variant.uiVal   = *(reinterpret_cast<USHORT*>(value));    break;
                case VT_I4:
                case VT_INT:     variant.lVal    = *(reinterpret_cast<LONG*>(value));      break;
                case VT_ERROR:
                case VT_UI4:
                case VT_UINT:
                case VT_HRESULT: variant.ulVal   = *(reinterpret_cast<ULONG*>(value));     break;
                case VT_I8:      variant.llVal   = *(reinterpret_cast<LONGLONG*>(value));  break;
                case VT_UI8:     variant.ullVal  = *(reinterpret_cast<ULONGLONG*>(value)); break;
                case VT_R4:      variant.fltVal  = *(reinterpret_cast<FLOAT*>(value));     break;
                case VT_R8:      variant.dblVal  = *(reinterpret_cast<DOUBLE*>(value));    break;
                case VT_BSTR:    variant.bstrVal = SysAllocString( reinterpret_cast<wchar_t*>(value) ); break;
                case VT_UINT_PTR:
                case VT_PTR:
                case VT_UNKNOWN:
                case VT_BYREF:   variant.byref = value; break;
                default: hr = E_UNEXPECTED; break;
            }

            if( E_UNEXPECTED != hr )
                hr = pParamConfig->SetValue( &variant, OATRUE );

            if( VT_BSTR == varType )
                SysFreeString( variant.bstrVal );
        }
        else
        {
            VARIANT variant = {0};
            variant.vt = varType;
            variant.byref = value;
            hr = pModuleConfig->SetValue(&paramGuid, &variant);
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}


HRESULT GetIMCIntValue( IBaseFilter *pFilter, const GUID &paramGuid, int &intValue )
{
    if( !pFilter )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            hr = pParamConfig->GetValue(&variant,OATRUE);
            if( SUCCEEDED(hr) )
            {
                intValue = variant.intVal;
            }
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}

HRESULT SetIMCIntValue( IBaseFilter *pFilter, const GUID &paramGuid, const int intValue )
{
    if( !pFilter )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            variant.vt = VT_INT;
            variant.intVal = intValue;

            hr = pParamConfig->SetValue( &variant, OATRUE );
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}

HRESULT GetIMCStringValue( IBaseFilter *pFilter, const GUID &paramGuid, wchar_t *stringValue, const size_t stringSize )
{
    if( !pFilter || !stringValue )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            hr = pParamConfig->GetValue(&variant,OATRUE);
            if( SUCCEEDED(hr) )
            {
                wcsncpy_s( stringValue, stringSize, variant.bstrVal, wcslen(variant.bstrVal) );
                SysFreeString(variant.bstrVal);
            }
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}

HRESULT SetIMCStringValue( IBaseFilter *pFilter, const GUID &paramGuid, const wchar_t *stringValue )
{
    if( !pFilter )
        return E_POINTER;

    IModuleConfig *pModuleConfig = NULL;
    IParamConfig *pParamConfig = NULL;

    HRESULT hr = pFilter->QueryInterface( IID_IModuleConfig, reinterpret_cast<void**>(&pModuleConfig) );
    if( SUCCEEDED(hr) )
    {
        hr = pModuleConfig->GetParamConfig( &paramGuid, &pParamConfig );
        if( SUCCEEDED(hr) )
        {
            VARIANT variant = {0};
            variant.vt = VT_BSTR;
            variant.bstrVal = SysAllocString( stringValue );

            hr = pParamConfig->SetValue( &variant, OATRUE );

            SysFreeString( variant.bstrVal );
        }
    }

    SAFE_RELEASE( pParamConfig );
    SAFE_RELEASE( pModuleConfig );

    return hr;
}
