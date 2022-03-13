/********************************************************************
 Created: 2009/08/12 
 File name: imodcfg_utils.h
 Purpose: IModuleConfig programming utilities

 Copyright (c) 2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MC_IMODCFG_UTILS_H__
#define __MC_IMODCFG_UTILS_H__

#include <streams.h>
#include <initguid.h>

#include "ModuleConfig.h"

//
// In general see the "MainConcept IModuleConfig DS Documentation.pdf" guide
// for further information regarding the following functions.
//

// All-purpose value retrieval from pFilter via IModuleConfig parameter paramGUID,
// the variant type vt will tell which type of value is returned!
HRESULT GetIMCValue( IBaseFilter *pFilter, const GUID &paramGuid, VARTYPE &vt, void *value );

// Variable type pFilter configuration via IModuleConfig parameter paramGUID of variant type vt,
// make sure to pass a reference to a matching value!
HRESULT SetIMCValue( IBaseFilter *pFilter, const GUID &paramGuid, const VARTYPE vt, void *value );

// Ask pFilter for its IModuleConfig parameter paramGUID's integer value
HRESULT GetIMCIntValue( IBaseFilter *pFilter, const GUID &paramGuid, int &intValue );

// Configure pFilter via its IModuleConfig parameter paramGUID's integer value
HRESULT SetIMCIntValue( IBaseFilter *pFilter, const GUID &paramGuid, const int intValue );

// Ask pFilter for its IModuleConfig parameter paramGUID's BSTR value
HRESULT GetIMCStringValue( IBaseFilter *pFilter, const GUID &paramGuid, wchar_t *stringValue, const size_t stringSize );

// Configure pFilter via its IModuleConfig parameter paramGUID's BSTR value
HRESULT SetIMCStringValue( IBaseFilter *pFilter, const GUID &paramGuid, const wchar_t *stringValue );

#endif // __MC_IMODCFG_UTILS_H__