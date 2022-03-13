/********************************************************************
 Created: 2006/07/28 
 File name: mcdsutils.h
 Purpose: DirectShow helper utilities

 Copyright (c) 2006-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#ifndef __MC_DSHOW_UTILS_H__
#define __MC_DSHOW_UTILS_H__

#include <streams.h>

#define SAFE_RELEASE(p) {if (p) {p->Release (); p = NULL;}}
#define SAFE_DELETE(p) {if (p) {delete p; p = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if (p) {delete [] p; p = NULL;}}

// Adds a DirectShow filter graph to the Running Object Table,
// allowing GraphEdit to "spy" on a remote filter graph.
//
// IMPORTANT: Beginning with Vista's DirectX 10 Microsoft moved the related functionality from
//            quartz.dll into GraphEdit's proppage.dll. So in order to see this feature working
//            under Vista, make sure to utilize GraphEdit from the latest Windows PlatformSDK!
HRESULT AddGraphToRot( IUnknown *pUnkGraph, DWORD *pdwRegister );

// Removes a filter graph from the Running Object Table
void RemoveGraphFromRot( DWORD pdwRegister );

// Tear down everything downstream of a given filter
void NukeDownstream( IGraphBuilder *pFG, IBaseFilter *pf );

// Initialize a filter graph manager
HRESULT InitFilterGraph( IGraphBuilder **, ICaptureGraphBuilder2 ** );

// Cleanup the filter graph manager retrieved by InitFilterGraph()
void UnInitFilterGraph( IGraphBuilder *, ICaptureGraphBuilder2 * );
// pointer-pointer version which makes sure that the passed pointers are cleared to NULL
void UnInitFilterGraph( IGraphBuilder **, ICaptureGraphBuilder2 ** );

// Find a pin by its majortype and direction, get pin and subtype
HRESULT FindPinByMajorType( IBaseFilter *pFilter, const GUID &MajorType, PIN_DIRECTION PinDir,
                            GUID *SubType, IPin ** ppPin );

// Find a pin by category
HRESULT FindPinByCategory( IBaseFilter *pFilter, const GUID &Category, IPin ** ppPin );

// Find a pin by name
HRESULT FindPinByName( IBaseFilter *pFilter, wchar_t *pName, IPin ** ppPin );

// Find a pin by direction - set bFree TRUE if the pin must be unconnected
HRESULT FindPinByDirection( IBaseFilter *pFilter, PIN_DIRECTION PinDir, bool bFree, IPin ** ppPin );

// Count all pins with the specified direction and major type
HRESULT CountPinsByDirectionAndType( IBaseFilter *pFilter, const PIN_DIRECTION *pPinDir, const GUID *pMajorType,
                                     IPin ** ppPin, ULONG &PinCount);

// Open a media stream from 'file' using the source reader specified by its 'clsId'.
HRESULT FindSourceReader( IGraphBuilder *pFG, IBaseFilter ** ppFileReader, const GUID &clsId, const wchar_t *file );

// Connect an output pin to a (downstream) filter
HRESULT TryConnect( IGraphBuilder *pFG, IPin *pOutPin, IBaseFilter ** ppFilter, bool bDestroy );

// Use IFileSinkInterface2 or IFileSinkInterface to set a file writer's output file - bDeleteOldFile should be true when not life-capturing
HRESULT SetFileSink( IBaseFilter *pFileWriter, const wchar_t *filename, bool bDeleteOldFile );

bool IsMediaSubtypeYUV( const GUID& subtype );
bool IsMediaSubtypeRGB( const GUID& subtype );

// Checks for running/paused/stop state
HRESULT GetGraphState( IMediaControl *pMCtrl, const int timeout, OAFilterState& fstate );

// Retrieves the current graph time (not to be confused with the media sample time)
HRESULT GetGraphTime ( IGraphBuilder *pFG, REFERENCE_TIME& refTime );

// Sets the filter graph's clock filter - to undo the selection call pFG->SetDefaultSyncSource()
HRESULT SetGraphClock( IGraphBuilder *pFG, IBaseFilter *pClockFilter );

// Opens a DirectShow filter's property page in a modal window
HRESULT OpenPropertyPage( IBaseFilter *pFilter, HWND AppHandle );

#endif // __MC_DSHOW_UTILS_H__