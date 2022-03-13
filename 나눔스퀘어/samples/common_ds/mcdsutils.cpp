/********************************************************************
 Created: 2006/07/28 
 File name: mcdsutils.cpp
 Purpose: DirectShow helper utilities

 Copyright (c) 2006-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdio.h>
#include "mcdsutils.h"

// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
HRESULT AddGraphToRot( IUnknown *pUnkGraph, DWORD *pdwRegister )
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    wchar_t wsz[128];
    size_t wsize = sizeof(wsz)/sizeof(wsz[0]);
    HRESULT hr;

    if( FAILED(GetRunningObjectTable(0, &pROT)) )
        return E_FAIL;

#if (_MSC_VER < 1400)
    _snwprintf(wsz, wsize, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
#else
    _snwprintf_s(wsz, wsize, wsize, L"FilterGraph %p pid %08x", pUnkGraph, GetCurrentProcessId());
#endif


    hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if( SUCCEEDED(hr) )
    {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

// Removes a filter graph from the Running Object Table
void RemoveGraphFromRot( DWORD pdwRegister )
{
    IRunningObjectTable *pROT;

    if( SUCCEEDED(GetRunningObjectTable(0, &pROT)) )
    {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

// Tear down everything downstream of a given filter
void NukeDownstream( IGraphBuilder *pFG, IBaseFilter *pf ) 
{
    OutputDebugStringA("DSUtils: NukeDownstream\n");

    IPin *pP, *pTo;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;
    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();
    while( hr == NOERROR )
    {
        hr = pins->Next(1, &pP, &u);
        if( (hr == S_OK) && pP )
        {
            pP->ConnectedTo(&pTo);
            if( pTo )
            {
                hr = pTo->QueryPinInfo(&pininfo);
                if( hr == NOERROR )
                {
                    if( pininfo.dir == PINDIR_INPUT )
                    {
                        NukeDownstream(pFG,pininfo.pFilter);
                        pFG->Disconnect(pTo);
                        pFG->Disconnect(pP);
                        pFG->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                }
                pTo->Release();
            }
            pP->Release();
        }
    }
    if( pins )
        pins->Release();
}

// Initialize a filter graph manager
HRESULT InitFilterGraph( IGraphBuilder ** ppFilterGraph, ICaptureGraphBuilder2 ** ppGraphBuilder )
{
  // create a filtergraph
    HRESULT hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,
                                   IID_IGraphBuilder, reinterpret_cast<void**>(ppFilterGraph) );
    if( FAILED(hr) )
        return hr;

  // create a capture graphbuilder
    hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
                           IID_ICaptureGraphBuilder2,  reinterpret_cast<void**>(ppGraphBuilder) );
    if( FAILED(hr) )
    {
        SAFE_RELEASE((*ppFilterGraph));
        return hr;
    }

  // set the graphbuilder's filtergraph
    hr = (*ppGraphBuilder)->SetFiltergraph(*ppFilterGraph);
    if( FAILED(hr) )
    {
        SAFE_RELEASE((*ppGraphBuilder));
        SAFE_RELEASE((*ppFilterGraph));
        return hr;
    }

    return hr;
}

// Cleanup the filter graph manager retrieved by InitFilterGraph()
void UnInitFilterGraph( IGraphBuilder * pFG, ICaptureGraphBuilder2 * pCGB )
{
    SAFE_RELEASE(pFG);
    SAFE_RELEASE(pCGB);
}
// pointer-pointer version
void UnInitFilterGraph( IGraphBuilder ** ppFG, ICaptureGraphBuilder2 ** ppCGB )
{
    if( ppFG )
        SAFE_RELEASE((*ppFG));
    if( ppCGB )
        SAFE_RELEASE((*ppCGB));
}


// Find a pin by its majortype and direction, get pin and subtype
HRESULT FindPinByMajorType( IBaseFilter *pFilter, const GUID &MajorType, PIN_DIRECTION PinDir, GUID *SubType, IPin ** ppPin )
{
    OutputDebugStringA("DSUtils: FindPinByMajorType\n");

    HRESULT     hr = S_OK;
    IEnumPins *   pEnumPin = NULL;
    PIN_DIRECTION PinDirection;
    ULONG     ulFetched;
    ULONG     ulMtFetched;

    bool      bFound = false;

    IEnumMediaTypes *pEnumMediaTypes;
    AM_MEDIA_TYPE *pmt;

    *ppPin = NULL;

    if( !pFilter )
    {
        OutputDebugStringA("DSUtils: FindPinByMajorType: pFilter is NULL!\n");
        return E_INVALIDARG;
    }

    hr = pFilter->EnumPins( &pEnumPin );
    if( SUCCEEDED(hr) )
    {
        while( S_OK == (hr = pEnumPin->Next( 1L, ppPin, &ulFetched )) )
        {
            hr = (*ppPin)->QueryDirection(&PinDirection);
            if( ( S_OK == hr ) && ( PinDirection == PinDir ) )
            {
                hr = (*ppPin)->EnumMediaTypes(&pEnumMediaTypes);
                if( SUCCEEDED(hr) )
                {
                    while( S_OK == pEnumMediaTypes->Next( 1L, &pmt, &ulMtFetched ) )
                    {
                        if( IsEqualGUID(pmt->majortype,MajorType) )
                        {
                            if( SubType != NULL )
                                *SubType = pmt->subtype;
                            bFound = true;
                            DeleteMediaType(pmt);
                            break;
                        }
                        DeleteMediaType(pmt); 
                    }
                    pEnumMediaTypes->Release ();
                }
                if( bFound )
                    break;
            }
            (*ppPin)->Release();
            (*ppPin) = NULL;
        }
        pEnumPin->Release();
    }
    return hr;
}

// Find a pin by category
HRESULT FindPinByCategory( IBaseFilter *pFilter, const GUID &Category, IPin ** ppPin )
{
    OutputDebugStringA("DSUtils: FindPinByCategory\n");

    IEnumPins *pEnumPin = NULL;
    ULONG ulFetched;
    *ppPin = NULL;

    if( !pFilter )
    {
        OutputDebugStringA("DSUtils: FindPinByCategory: pFilter is NULL!\n");
        return E_INVALIDARG;
    }

    HRESULT hr = pFilter->EnumPins( &pEnumPin );
    if( SUCCEEDED(hr) )
    {
        while( S_OK == (hr = pEnumPin->Next( 1L, ppPin, &ulFetched )) )
        {
            IKsPropertySet *pKs;
            hr = (*ppPin)->QueryInterface(IID_IKsPropertySet, (void **)&pKs);
            // why does this fail e.g. with Smart Tee filter ?!?
            if( SUCCEEDED(hr) )
            {
                // Try to retrieve the pin category.
                GUID PinCategory;
                DWORD cbReturned;
                hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, 
                              &PinCategory, sizeof(GUID), &cbReturned);
                if( SUCCEEDED(hr) )
                {
                    // PinCategory now contains the category GUID.
                    if( IsEqualGUID(Category,PinCategory) )
                        break; // *ppPin points now to a pin with desired Category
                }
                pKs->Release();
            }
            (*ppPin)->Release();
            (*ppPin) = NULL;
        }
        pEnumPin->Release();
    }
    return hr;
}

// Find a pin by its name
HRESULT FindPinByName( IBaseFilter *pFilter, wchar_t* pName, IPin ** ppPin )
{
    OutputDebugStringA("DSUtils: FindPinByName\n");

    IEnumPins *pEnumPin = NULL;
    ULONG ulFetched;
    *ppPin = NULL;

    if( !pFilter )
    {
        OutputDebugStringA("DSUtils: FindPinByName: pFilter is NULL!\n");
        return E_INVALIDARG;
    }

    PIN_INFO *pPinInfo = new PIN_INFO;

    HRESULT hr = pFilter->EnumPins( &pEnumPin );
    if( SUCCEEDED(hr) )
    {
        while( S_OK == (hr = pEnumPin->Next( 1L, ppPin, &ulFetched )) )
        {
            // Try to retrieve the pin info.
            hr = (*ppPin)->QueryPinInfo(pPinInfo);
            if( SUCCEEDED(hr) )
            {
                if( 0 == _wcsnicmp(pName,pPinInfo->achName,MAX_PIN_NAME) )
                    break; // *ppPin points now to a pin with desired Category
            }
            SAFE_RELEASE(pPinInfo->pFilter);
            SAFE_RELEASE((*ppPin));
        }
        pEnumPin->Release();
    }
    SAFE_DELETE(pPinInfo);
    return hr;
}

// Find a pin by direction - set bFree true if the pin must be unconnected
HRESULT FindPinByDirection( IBaseFilter *pFilter, PIN_DIRECTION PinDir, bool bFree, IPin ** ppPin )
{
    OutputDebugStringA("DSUtils: FindPinByDirection\n");

    HRESULT       hr = S_OK;
    IEnumPins *   pEnumPin = NULL;
    PIN_DIRECTION PinDirection;
    ULONG         ulFetched;

    IPin *pConPin; // where the found pin is connected to
    *ppPin = NULL;

    if( !pFilter )
    {
        OutputDebugStringA("DSUtils: FindPinByDirection: pFilter is NULL!\n");
        return E_INVALIDARG;
    }

    hr = pFilter->EnumPins( &pEnumPin );
    if( SUCCEEDED(hr) )
    {
        while( S_OK == (hr = pEnumPin->Next( 1L, ppPin, &ulFetched )) )
        {
            hr = (*ppPin)->QueryDirection(&PinDirection);
            if( ( S_OK == hr ) && ( PinDirection == PinDir ) )
            {
                if( bFree ) // pin must be unconnected
                {
                    hr = (*ppPin)->ConnectedTo(&pConPin);
                    if( hr == VFW_E_NOT_CONNECTED )
                    {
                        hr = S_OK;
                        break;
                    }
                    SAFE_RELEASE(pConPin);            
                }
                else
                    break;
            }
            (*ppPin)->Release();
            (*ppPin) = NULL;
        }
        pEnumPin->Release();
    }
    return hr;
} 


// Count all pins with the specified direction and major type (useful e.g. for SVC decoder) -
// you also may pass a NULL-pointer for direction or major type to count all of them
// Note: this function doesn't handle VFW_E_ENUM_OUT_OF_SYNC scenarios for now!
// PinCount is [in/out] parameter: [in] specifies the ppPin array length, [out] returns the number of fetched ones
HRESULT CountPinsByDirectionAndType( IBaseFilter *pFilter, const PIN_DIRECTION *pPinDir, const GUID *pMajorType,
                                     IPin ** ppPin, ULONG &PinCount )
{
    OutputDebugStringA("DSUtils: CountPinsByDirection\n");

    HRESULT hr = S_OK;
    IEnumPins *pEnumPin = NULL;
    PIN_DIRECTION PinDirection;
    IEnumMediaTypes *pEnumMediaTypes = NULL;
    AM_MEDIA_TYPE *pmt = NULL;
    ULONG i=0, j=0, ulFetched=0, ulMtFetched=0, ulMax = PinCount;

    for( i=0; i < ulMax; i++ ) // initialize all pointers in the array to zero
        ppPin[i] = NULL;
    PinCount = 0; // don't confuse ulFetched and PinCount: ulFetched counts ALL directions' pins!

    if( !pFilter )
    {
        OutputDebugStringA("DSUtils: CountPinsByDirection: pFilter is NULL!\n");
        return E_INVALIDARG;
    }

    hr = pFilter->EnumPins( &pEnumPin );
    if( SUCCEEDED(hr) )
    {
        if( SUCCEEDED(pEnumPin->Next( ulMax, ppPin, &ulFetched )) )
        {
            for( i=0,j=0; i < ulMax; i++ )
            {
                if( !ppPin[i] )
                    continue;

                bool bFound = false;
                hr = (ppPin[i])->QueryDirection(&PinDirection);
                if( ( S_OK == hr ) && ( !pPinDir || (pPinDir && (PinDirection == *pPinDir)) ) )
                {
                    if( !pMajorType )
                    {
                        PinCount++; // Gotcha!
                        bFound = true;
                        if( i!=j )
                        {
              // shift the pointers a bit, so that afterwards the first PinCount pointers reference an IPin object;
              // else the returned array would reference something in e.g. the 1st, 3rd, 6th,... i.e. some indices would be skipped
                            ppPin[j] = ppPin[i];
                            ppPin[i] = NULL;
                        }
                        j++;
                    }
                    else
                    {
                        hr = (ppPin[i])->EnumMediaTypes(&pEnumMediaTypes);
                        if( SUCCEEDED(hr) )
                        {
                            while( S_OK == pEnumMediaTypes->Next( 1L, &pmt, &ulMtFetched ) )
                            {
                                if( !pMajorType || (pMajorType && IsEqualGUID(pmt->majortype,*pMajorType)) )
                                {
                                    PinCount++; // Gotcha!
                                    bFound = true;
                                    if( i!=j )
                                    {
                    // shift the pointers a bit, so that afterwards the first PinCount pointers reference an IPin object;
                    // else the returned array would reference something in e.g. the 1st, 3rd, 6th,... i.e. some indices would be skipped
                                        ppPin[j] = ppPin[i];
                                        ppPin[i] = NULL;
                                    }
                                    j++;
                                    DeleteMediaType(pmt);
                                    break;
                                }
                                else
                                    DeleteMediaType(pmt); 
                            }// while
                            pEnumMediaTypes->Release();
                        }
                    }
                }

                if( !bFound )
                {
                    (ppPin[i])->Release();
                    (ppPin[i]) = NULL;
                }
            } // for
        }
        pEnumPin->Release();
    }
    return hr; // better check the ppPin array fields instead of this return value for success!
}


// Open a media stream from 'file' using the source reader specified by its 'clsId'.
HRESULT FindSourceReader( IGraphBuilder *pFG, IBaseFilter ** ppFileReader, const GUID &clsId, const wchar_t *file )
{
    if( !file )
        return E_POINTER;

    if( *ppFileReader )
    {
        if( pFG )
            pFG->RemoveFilter( *ppFileReader );
        SAFE_RELEASE( (*ppFileReader) );
    }

    HRESULT hr = CoCreateInstance( clsId, NULL, CLSCTX_INPROC_SERVER,
                                   IID_IBaseFilter, reinterpret_cast<void**>(ppFileReader) );
    if( *ppFileReader )
    {
        // Add it to the filter graph
        if( pFG )
            hr = pFG->AddFilter( *ppFileReader, NULL );

        // Set its source filename
        IFileSourceFilter* pAsfFileSource = NULL;
        hr = (*ppFileReader)->QueryInterface( IID_IFileSourceFilter, reinterpret_cast<void**>(&pAsfFileSource) );
        if( pAsfFileSource )
        {
            BSTR pbFile = SysAllocString(file);
            hr = pAsfFileSource->Load( pbFile, NULL );
            SysFreeString(pbFile);
            pAsfFileSource->Release();
            pAsfFileSource = NULL;
        }
    }
    return hr;
}

// Connect an output pin to a (downstream) filter
HRESULT TryConnect( IGraphBuilder *pFG, IPin *pOutPin, IBaseFilter ** ppFilter,  bool bDestroy )
{
    if( !pFG || !ppFilter || !(*ppFilter) || !pOutPin )
        return E_UNEXPECTED;
    // *ppFilter must yet be added to the filter graph, else this method cannot succeed;
    // pass bDestroy as false if -- in error cases -- the filter shall just be removed from the filter graph,
    // or as true if the filter instance shall be cleaned up completely

    HRESULT hr = E_FAIL;
    IPin *pInPin = NULL;

    // find *ppFilter's input pins
    hr = FindPinByDirection( *ppFilter, PINDIR_INPUT, true, &pInPin );
    if( !pInPin )
    {
        pFG->RemoveFilter(*ppFilter); // we don't need filters without pins in the graph!
        if( bDestroy )
            SAFE_RELEASE((*ppFilter));
        hr = E_NOINTERFACE; // better don't trust the FindPinByDirection() return, set it to "no pin interface"
    }
    else // and try the connection
    {
        hr = pFG->ConnectDirect( pOutPin, pInPin, NULL );
        if( FAILED(hr) )
        {
            pFG->RemoveFilter(*ppFilter);
            if( bDestroy )
                SAFE_RELEASE((*ppFilter));
        }
    }

    // release the in-pin in any case!
    SAFE_RELEASE( pInPin );
    return hr;
}

// Use IFileSinkInterface2 or IFileSinkInterface to set a filter writer's output file
HRESULT SetFileSink( IBaseFilter *pFileWriter, const wchar_t *filename, bool bDeleteOldFile )
{
    if( !pFileWriter )
        return E_POINTER;
    // filename may be NULL: it is legal to call SetFileName(0,0) for resetting

    HRESULT hr = E_FAIL;
    // try to access IFileSinkFilter2 interface first - legacy writers like dump.ax don't support it, however
    IFileSinkFilter2 *pIFileSink2 = NULL;
    hr = pFileWriter->QueryInterface( IID_IFileSinkFilter2, reinterpret_cast<void**>(&pIFileSink2) );
    if( SUCCEEDED(hr) )
    {
        hr = pIFileSink2->SetMode( bDeleteOldFile ? AM_FILE_OVERWRITE : 0 );
        hr = pIFileSink2->SetFileName( filename, NULL );
    }
    SAFE_RELEASE(pIFileSink2);

    if( FAILED(hr) ) // then try legacy interface which doesn't support SetMode()
    {
        IFileSinkFilter *pIFileSink = NULL;
        hr = pFileWriter->QueryInterface( IID_IFileSinkFilter, reinterpret_cast<void**>(&pIFileSink) );
        if( SUCCEEDED(hr) )
        {
            hr = pIFileSink->SetFileName( filename, NULL );
        }
        SAFE_RELEASE(pIFileSink);
    }

    return hr;
}

// According to MSDN DirectShow article "YUV Video Subtypes"
bool IsMediaSubtypeYUV( const GUID& subtype )
{
    return(    IsEqualGUID( MEDIASUBTYPE_AYUV, subtype )    // Packed
               || IsEqualGUID( MEDIASUBTYPE_YUY2, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_UYVY, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_IMC1, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_IMC2, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_IMC3, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_IMC4, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_YV12, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_NV12, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_Y411, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_Y41P, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_Y211, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_YVYU, subtype ) // Packed
               || IsEqualGUID( MEDIASUBTYPE_YVU9, subtype ) // Planar
               || IsEqualGUID( MEDIASUBTYPE_IF09, subtype ) // Planar
          );
}

// According to MSDN DirectShow article "Uncompressed RGB Video Subtypes"
bool IsMediaSubtypeRGB( const GUID& subtype )
{
    return(    IsEqualGUID( MEDIASUBTYPE_RGB1,           subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB4,        subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB8,        subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB555,      subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB565,      subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB24,       subtype )
               || IsEqualGUID( MEDIASUBTYPE_RGB32,       subtype )
               || IsEqualGUID( MEDIASUBTYPE_ARGB1555,    subtype )
               || IsEqualGUID( MEDIASUBTYPE_ARGB32,      subtype )
               || IsEqualGUID( MEDIASUBTYPE_ARGB4444,    subtype )
               || IsEqualGUID( MEDIASUBTYPE_A2R10G10B10, subtype )
               || IsEqualGUID( MEDIASUBTYPE_A2B10G10R10, subtype )
          );
}

// Checks for running/paused/stop state
HRESULT GetGraphState( IMediaControl *pMCtrl, const int timeout, OAFilterState& fstate )
{
    HRESULT hr = E_FAIL;
    if( !pMCtrl )
        hr = E_POINTER;
    else
    {
        OAFilterState oastate;
        hr = pMCtrl->GetState( timeout, &oastate );
        if( SUCCEEDED(hr) )
            fstate = oastate;
    }
    return hr;
}

// Retrieves the current graph time (not to be confused with the media sample time)
HRESULT GetGraphTime( IGraphBuilder *pFG, REFERENCE_TIME& refTime )
{
    if( !pFG )
        return E_POINTER;

    IMediaFilter *pMediaFilter = NULL;
    HRESULT hr = pFG->QueryInterface( IID_IMediaFilter, reinterpret_cast<void**>(&pMediaFilter) );
    if( SUCCEEDED(hr) )
    {
        IReferenceClock *pClock = NULL;
        hr = pMediaFilter->GetSyncSource(&pClock);
        if( SUCCEEDED(hr) && pClock )
            hr = pClock->GetTime(&refTime);

        SAFE_RELEASE(pClock);
        SAFE_RELEASE(pMediaFilter);
    }
    return hr;
}

// Sets the filter graph's clock filter - to undo the selection call pFG->SetDefaultSyncSource()
HRESULT SetGraphClock( IGraphBuilder *pFG, IBaseFilter *pClockFilter )
{
    if( !pFG )
        return E_FAIL;

    IReferenceClock *pClock = NULL;
    HRESULT hr = S_OK;
    if( pClockFilter )
        hr = pClockFilter->QueryInterface( IID_IReferenceClock, reinterpret_cast<void**>(&pClock) );
    else // this will run the graph unsynchronized and as fast as possible
        OutputDebugStringA("SetGraphClock: setting clock to NULL!\n");

    if( FAILED(hr) )
        return hr;

    IMediaFilter *pMediaFilter = NULL;
    hr = pFG->QueryInterface( IID_IMediaFilter, reinterpret_cast<void**>(&pMediaFilter) );
    if( SUCCEEDED(hr) )
    {
        hr = pMediaFilter->SetSyncSource(pClock);
    }
    SAFE_RELEASE(pClock);
    SAFE_RELEASE(pMediaFilter);

    return hr;
}

// Opens a DirectShow filter's property page in a modal window
HRESULT OpenPropertyPage( IBaseFilter *pFilter, HWND AppHandle )
{
    if( !pFilter )
        return E_POINTER;

    ISpecifyPropertyPages *pProp = NULL;
    HRESULT hr = pFilter->QueryInterface( IID_ISpecifyPropertyPages, reinterpret_cast<void**>(&pProp) );
    if( SUCCEEDED(hr) && pProp )
    {
        // Get the filter's name and IUnknown pointer.
        FILTER_INFO FilterInfo;
        hr = pFilter->QueryFilterInfo(&FilterInfo); 
        IUnknown *pFilterUnk = NULL;
        hr = pFilter->QueryInterface( IID_IUnknown, reinterpret_cast<void**>(&pFilterUnk) );

        // Show the page. 
        CAUUID caGUID;
        hr = pProp->GetPages(&caGUID);
        pProp->Release(); pProp = NULL;

        hr = OleCreatePropertyFrame(
                                   AppHandle,           // Parent window
                                   0, 0,                // Reserved
                                   FilterInfo.achName,  // Caption for the dialog box
                                   1,                   // Number of objects (just the filter)
                                   &pFilterUnk,         // Array of object pointers. 
                                   caGUID.cElems,       // Number of property pages
                                   caGUID.pElems,       // Array of property page CLSIDs
                                   0,                   // Locale identifier
                                   0, NULL              // Reserved
                                   );

        // Clean up.
        SAFE_RELEASE(pFilterUnk);
        SAFE_RELEASE(FilterInfo.pGraph); 
        CoTaskMemFree(caGUID.pElems);
    }
    else
        SAFE_RELEASE(pProp);

    return hr;
}