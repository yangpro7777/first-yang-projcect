/********************************************************************
 Created: 2009/07/30
 File name: sample_demux_mxf_file_ds.cpp
 Purpose: command-line DirectShow sample (see 'Description' below)

 Copyright (c) 2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

// Name:             sample_demux_mxf_file_ds
// Used components:  demux_mxf_ds, render_fileindex_ds
// Description:      Demux all elementary streams from muxed MXF stream into target directory
// Command line:     sample_demux_mxf_file_ds.exe <INPUT_FILE> <OUTPUT_DIRECTORY>
// Expected result:  The tool will use file source filter for INPUT_FILE.
//                   It will connect MXF Demuxer filter. If the connection fails, the tool will abort
//                   ("Connection failed, not an MXF file?"). If the connection succeeded, a dump filter
//                   will be connected to all output pins of the demuxer.
//                   Filename for dump filters is "OUTPUT_DIRECTORY/pin_x.pin_type".
// Example with console output > sample_demux_mxf_file_ds.exe c:\input.mxf c:\demuxed_files\
// Created files:    c:\demuxed_files\pin_1.mp2v
//                   c:\demuxed_files\pin_2.pcm
// [The tool can be stopped with any key press]

//*********************************************************************/

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <errno.h>

#include <streams.h>
#include <initguid.h> // necessary for DEFINE_GUID macro, must be included after streams.h

#include "mcdsutils.h" // DShow utilities
#include "sample_common_args.h" // command-line parsing

// Codec SDK headers
#include "demux_mxf_guid.h" // our MXF demuxer
#include "render_fileindex_guid.h" // our Sink filter
#include "mediatypes_guids.h"

#define _WIN32_DCOM  // for CoInitializeEx()


//----------------------------------
// sample-specific methods:

//----------------------------------

// miscellaneous methods:

void calcOutputExtension( IPin* outPin, wchar_t* outExt, size_t outExtLen )
{
    // this function tries to figure out a file extension based on a pin's media type information;
    // but don't expect too much, the media type analysis can't go into deep too far in this sample
    const wchar_t defext[4] = L"mxf";
    wchar_t *extension = (wchar_t*)&defext;

    if( outPin )
    {
        PIN_INFO info; // first try to ask the demuxer's output pin for some info
        if( SUCCEEDED(outPin->QueryPinInfo( &info )) )
        {
            size_t len = wcslen(info.achName);
            _wcslwr_s(info.achName,len+1);
            if( wcsstr( info.achName, L"aac" ) )
                extension = L"aac";
            else if( wcsstr( info.achName, L"amr" ) )
                extension = L"amr";
            else if( wcsstr( info.achName, L"ac3" ) || wcsstr( info.achName, L"dolby" ) )
                extension = L"ac3";
            else if( wcsstr( info.achName, L"id 192" ) )
                extension = L"mpa";
            else if( wcsstr( info.achName, L"pcm" ) )
                extension = L"pcm";
            else if( wcsstr( info.achName, L"h264" ) || wcsstr( info.achName, L"h.264" ) || wcsstr( info.achName, L"avc" ) )
                extension = L"avc";
            else if( wcsstr( info.achName, L"mpeg4" ) )
                extension = L"mp4v";
            else if( wcsstr( info.achName, L"vc1" ) )
                extension = L"vc1";
            else if( wcsstr( info.achName, L"ves" ) )
                extension = L"mpv";
            else if( wcsstr( info.achName, L"vc3" ) )
                extension = L"vc3";
            else if( wcsstr( info.achName, L"pcm" ) )
                extension = L"pcm";
        }
        SAFE_RELEASE(info.pFilter);

        AM_MEDIA_TYPE mt; // go on with AM_MEDIA_TYPE structure's information
        if( SUCCEEDED( outPin->ConnectionMediaType(&mt) ) )
        {
            if( MEDIATYPE_Video == mt.majortype )
            {
                if( 0 == _wcsicmp( extension, defext ) )
                {
                    if( MEDIASUBTYPE_dvsl == mt.subtype || MEDIASUBTYPE_dvsd == mt.subtype || MEDIASUBTYPE_dvhd == mt.subtype ||
                        MEDIASUBTYPE_dv25 == mt.subtype || MEDIASUBTYPE_dv50 == mt.subtype || MEDIASUBTYPE_dvh1 == mt.subtype )
                        extension = L"dv";
                    else if( MEDIASUBTYPE_MPEG1System == mt.subtype || MEDIASUBTYPE_MPEG1VideoCD == mt.subtype || MEDIASUBTYPE_MPEG1Video == mt.subtype ||
                             MEDIASUBTYPE_MPEG1Packet == mt.subtype || MEDIASUBTYPE_MPEG1Payload == mt.subtype )
                        extension = L"mp1v";
                    else if( MEDIASUBTYPE_MPEG2_VIDEO == mt.subtype || MEDIASUBTYPE_MPEG2_PROGRAM == mt.subtype ||
                             MEDIASUBTYPE_MPEG2_TRANSPORT == mt.subtype )
                        extension = L"mp2v";
                }
            }
            else if( MEDIATYPE_Audio == mt.majortype )
            {
                if( 0 == _wcsicmp( extension, defext ) )
                {
                    if( MEDIASUBTYPE_PCM == mt.subtype || MEDIASUBTYPE_DVD_LPCM_AUDIO == mt.subtype )
                        extension = L"pcm";
                    else if( MEDIASUBTYPE_MPEG2_AUDIO == mt.subtype || MEDIASUBTYPE_MPEG1Packet == mt.subtype || MEDIASUBTYPE_MPEG1Payload == mt.subtype )
                        extension = L"mpa";
                    else if( MEDIASUBTYPE_DOLBY_AC3 == mt.subtype || MEDIASUBTYPE_DOLBY_AC3_SPDIF == mt.subtype )
                        extension = L"ac3";
                    else if (MEDIASUBTYPE_RAW_AAC1 == mt.subtype || MEDIASUBTYPE_LATM_AAC == mt.subtype)
                        extension = L"aac";
                    else if( MEDIASUBTYPE_AMR == mt.subtype )
                        extension = L"amr";
                }
            }

            else if( MEDIATYPE_MXF_DATA == mt.majortype )
            {
              if( MEDIASUBTYPE_MXF_ANC_FRAME_ELEMENTS == mt.subtype )
                        extension = L"anc";
              else if( MEDIASUBTYPE_MXF_VBI_FRAME_ELEMENTS == mt.subtype )
                        extension = L"vbi";

            }

            // as a last resort try to map the subtype's FourCC code
            if( 0 == _wcsicmp( extension, defext ) )
            {
                DWORD32 fourcc = FOURCCMap(&mt.subtype).GetFOURCC();
                wchar_t wfcc[5];
                wfcc[0] = towlower( (fourcc>>0)  & 255 );
                wfcc[1] = towlower( (fourcc>>8)  & 255 );
                wfcc[2] = towlower( (fourcc>>16) & 255 );
                wfcc[3] = towlower( (fourcc>>24) & 255 );
                wfcc[4] = '\0';
                extension = wfcc;
            }
            FreeMediaType(mt);
        }
    }
    wcsncpy_s( outExt, outExtLen, extension, sizeof(wchar_t)*wcslen(extension) );
}

void calcOutputPath( wchar_t* outputPath, size_t outPathLen, const wchar_t* inputFile, const wchar_t* outputDir,
                     unsigned int index, const wchar_t* extension )
{
    wchar_t outputFile[_MAX_PATH];
    int offset = 0;
    for( offset=(int)wcslen(inputFile)-1; offset>=0; offset-- )
    {
        if( inputFile[offset] == '\\' || inputFile[offset] == '/' )
            break;
    }

    swprintf_s(outputFile, sizeof(outputFile)/sizeof(outputFile[0]), L"%s_pin_%u.%s", inputFile+offset+1, index, extension );
    wcsncpy_s( outputPath, outPathLen, outputDir, _MAX_PATH - sizeof(wchar_t)*(wcslen(outputFile)+1) );
    wcsncat_s( outputPath, outPathLen, outputFile, sizeof(wchar_t)*wcslen(outputFile) );
}

//----------------------------------
// sample-specific method implementations:

//----------------------------------

int main( int argc, char * argv[] )
{
    int ret = 0;
    HRESULT hr = E_FAIL;
    wchar_t wInputFile[_MAX_PATH], wOutputDir[_MAX_PATH], wOutputPath[_MAX_PATH], wOutputExtension[_MAX_PATH];
    char *InputFile = NULL, *OutputDir = NULL;
    const unsigned short _MAX_PINS = 16; // should be enough


    arg_item_t params[] = {
        { IDS_INPUT_FILE,  1, &InputFile },
        { IDS_OUTPUT_FILE, 1, &OutputDir }
    };
    if( parse_args( argc-1, argv+1, sizeof(params)/sizeof(params[0]), params ) < 0 )
    {
        printf_s("\n==== MainConcept 'demux_mxf_file' DirectShow Sample ====\n"
                 "Usage: sample_demux_mxf_file_ds -i \"C:\\input.mxf\" -o \"C:\\demuxed_files\"\n"
                 "Supported input stream formats are: Muxed MXF streams\n"
                 "Options: -i <input_file> -o <output_directory>\n\n"
                );

        printf_s( "Please enter the input file full path and name (hit <Return> to abort):\n" );
        std::wcin.get( wInputFile, _MAX_PATH );
        std::wcin.ignore(std::cin.rdbuf()->in_avail());
        if( wcslen(wInputFile) <= 0 )
            return 0;

        printf_s( "Please enter the output directory full path and name - be aware that existing files will be overwritten without prompt (hit <Return> to abort):\n" );
        std::wcin.get( wOutputDir, _MAX_PATH );
        std::wcin.ignore(std::cin.rdbuf()->in_avail());
        if( wcslen(wOutputDir) <= 0 )
            return 0;
    }
    else
    {
        // we need the InputFile as wide character string
        ret = MultiByteToWideChar( CP_ACP, 0, InputFile, (int)strlen(InputFile)+1,
                                   wInputFile, (int)(sizeof(wInputFile)/sizeof(wInputFile[0])) );
        // ditto the OutputDir
        ret = MultiByteToWideChar( CP_ACP, 0, OutputDir, (int)strlen(OutputDir)+1,
                                   wOutputDir, (int)(sizeof(wOutputDir)/sizeof(wOutputDir[0])) );
    }

    // append a trailing backslash to OutputFile if not yet done
    size_t oLen = wcslen( wOutputDir );
    if( wOutputDir[oLen-1] != '\\' && wOutputDir[oLen-1] != '/' )
        wcsncat_s( wOutputDir, (int)(sizeof(wOutputDir)/sizeof(wOutputDir[0])), L"\\", 1 );
    if( _wmkdir(wOutputDir) < 0 )
    {
        int err = 0;
        _get_errno(&err);
        if( ENOENT == err )
        {
            wprintf_s( L"\nCould not create the output directory %s! Please select another one.\n", wOutputDir );
            return 0;
        }
    }


    // initialize COM engine
    CoInitializeEx(NULL,COINIT_MULTITHREADED);

    IGraphBuilder *pFilterGraph = NULL;
    ICaptureGraphBuilder2 *pGraphBuilder = NULL;
    IMediaControl* pMediaControl = NULL;
    IMediaEventEx* pMediaEvent = NULL;
    IBaseFilter *pFileReader = NULL;
    IBaseFilter *pDemux = NULL;
    DWORD dwGraphReg = 0;
    GUID subType = GUID_NULL;
    IPin *pOutPin = NULL;

    // for the pin enumeration we need:
    unsigned long i = 0, pinCounter = _MAX_PINS; // expected max - should be enough (normally we'll fetch 1 or 2)
    PIN_DIRECTION pinDir = PINDIR_OUTPUT;
    IBaseFilter ** pSinkArray = new IBaseFilter*[pinCounter];
    IPin ** pOutPinArray = new IPin*[pinCounter];
    for( i = 0; i < _MAX_PINS; i++ )
    {
        pSinkArray[i] = NULL;
        pOutPinArray[i] = NULL;
    }

    // initialize a filter graph manager
    hr = InitFilterGraph( &pFilterGraph, &pGraphBuilder );
    if( FAILED(hr) )
    {
        printf_s( "\nERROR: Failed to initialize the filter graph manager!\n" );
        goto CLEAN_EXIT;
    }

    // open the specified input media stream
    // hr = pFilterGraph->AddSourceFilter( wInputFile, NULL, &pFileReader ); // standard DShow method
    hr = FindSourceReader( pFilterGraph, &pFileReader, CLSID_AsyncReader, wInputFile ); // allows AsyncReader only
    if( FAILED(hr) )
    {
        wprintf_s( L"\nERROR: Failed to find the input file %s!\n", wInputFile );
        goto CLEAN_EXIT;
    }

    // load MXF demultiplexer
    hr = CoCreateInstance( CLSID_MainConceptMXFDemuxer, NULL, CLSCTX_INPROC_SERVER,
                           IID_IBaseFilter, reinterpret_cast<void**>(&pDemux) );
    if( SUCCEEDED(hr) )
        hr = pFilterGraph->AddFilter( pDemux, NULL );

    if( FAILED(hr) )
        printf_s( "\nERROR: Failed to add the MXF demultiplexer to the filter graph, possibly not registered!\n" );


    printf_s( "\nI'll now raise the Demultiplexer Property page. Make your settings.." );
    waitForAnyKey(0);
    OpenPropertyPage(pDemux, NULL);

    // now try to connect the source filter with the demuxer - if this fails the media format isn't supported by this splitter
    if( SUCCEEDED(hr) )
    {
        // get reader's output pin - assume major type MEDIATYPE_Stream here, subType will return the stream's MEDIASUBTYPE
        hr = FindPinByMajorType( pFileReader, MEDIATYPE_Stream, PINDIR_OUTPUT, &subType, &pOutPin );
        if( !pOutPin )
            hr = E_FAIL;
        else
            hr = TryConnect( pFilterGraph, pOutPin, &pDemux, true );
        SAFE_RELEASE( pOutPin ); // we don't need this interface anymore in either case
    }

    if( FAILED(hr) )
    {
        printf_s( "\nERROR: Failed to connect the source filter with the MXF demuxer, the input file is not MXF!\n" );
        goto CLEAN_EXIT;
    }


    // enumerate all output pins of the demuxer - each elementary stream shall be dumped
    hr = CountPinsByDirectionAndType( pDemux, &pinDir, NULL, pOutPinArray, pinCounter );
    if( !pOutPinArray[0] )
    {
        printf_s( "\nERROR: Failed to find any MXF demuxer output pins!\n" );
        goto CLEAN_EXIT;
    }
    else
        printf_s( "Found %u demuxer output pins.\n", pinCounter );

    // now try to connect each of the demuxer pins with one Sink filter instance
    for( i = 0; i < pinCounter; i++ )
    {
        // load our Sink filter [mc/demo_render_fileindex_ds.ax] -
        // the standard DirectShow FileWriter doesn't support dumping elementary streams!
        // an alternative is to use the Dump filter example from the PlatformSDK(\Samples\Multimedia\DirectShow\Filters\Dump)
        hr = CoCreateInstance( CLSID_ESF, NULL, CLSCTX_INPROC_SERVER,
                               IID_IBaseFilter, reinterpret_cast<void**>(&pSinkArray[i]) );
        if( SUCCEEDED(hr) )
            hr = pFilterGraph->AddFilter( pSinkArray[i], NULL );

        if( FAILED(hr) )
        {
            printf_s( "\nERROR: Failed to add the Sink filter to the filter graph, possibly not registered!\n" );
            goto CLEAN_EXIT;
        }

        if( pOutPinArray[i] )
            hr = TryConnect( pFilterGraph, pOutPinArray[i], &pSinkArray[i], true );

        if( FAILED(hr) )
            printf_s( "\nWARNING: Failed to connect pin no. %u with a Sink filter instance.\n", i+1 );
        else
        {
            // we're not interested in rendering here -
            // it remains to set the Sink filter's output file (if such yet exists, it will be replaced!)

            calcOutputExtension( pOutPinArray[i], wOutputExtension, sizeof(wOutputExtension)/sizeof(wOutputExtension[0]) );
            calcOutputPath( wOutputPath, sizeof(wOutputPath)/sizeof(wOutputPath[0]),
                            wInputFile, wOutputDir, i+1, wOutputExtension );

            hr = SetFileSink( pSinkArray[i], wOutputPath, true );
            if( FAILED(hr) )
                wprintf_s( L"WARNING: Could not set a sink filter's output filename (instance no. %u).\n", i+1 );
            else
                wprintf_s( L"Set output filename %s\n", wOutputPath );
        }
        SAFE_RELEASE( pOutPinArray[i] ); // we don't need this interface anymore in neither case
    }


    // populate the ROT (Running Object Table) for analysis in GraphEdit
    AddGraphToRot( pFilterGraph, &dwGraphReg );
    // now you can spy the graph within graphedt.exe (hit Ctrl+G and select the PID dwGraphReg)

    printf_s( "\nSuccessfully composed the graph." );
    waitForAnyKey(0);

    // make sure we process events while the graph is running!
    hr = pFilterGraph->QueryInterface( IID_IMediaEventEx, reinterpret_cast<void**>(&pMediaEvent) );
    if( !pMediaEvent )
        printf_s( "\nWARNING: Could not access IMediaEventEx interface.\n" );

    // now Run() the graph, video playback will be displayed in an "ActiveMovie Window"
    hr = pFilterGraph->QueryInterface( IID_IMediaControl, reinterpret_cast<void**>(&pMediaControl) );
    if( SUCCEEDED(hr) )
        hr = pMediaControl->Run();
    else
    {
        printf_s( "\nERROR: Could not access IMediaControl interface!\n" );
        goto CLEAN_EXIT;
    }

    if( FAILED(hr) )
    {
        if( ERROR_PATH_NOT_FOUND == (hr&255) )
            printf_s( "\nThe output directory does not exist! Please specify a valid output path." );
        printf_s( "\nERROR: Failed to run the graph!\n" ); // goto CLEAN_EXIT
    }
    else
    {
        // Wait for completion - normally an application will catch EC_COMPLETE and other messages in its event loop,
        // here we'll check IMediaEvent::WaitForCompletion() periodically
        long evCode = 0, modifier = 0;
        do
        {
            modifier++;
            printf_s( "Enjoy the brilliant action ... press any key to abort %s  \r", (1==modifier%3) ? "." : (2==modifier%3) ? ".." : "..." );

            hr = pMediaEvent ? pMediaEvent->WaitForCompletion( 333, &evCode ) : E_ABORT;

            if( _kbhit() )
                break; // don't _getch() it here
        }
        while( E_ABORT == hr );

        if( SUCCEEDED(hr) )
        {
            if( EC_COMPLETE == evCode )
                printf_s( "\n\n=== END OF STREAM ===\n" );
            else if( EC_ERRORABORT == evCode )
                printf_s( "\n\nERROR: An unknown error occured, graph running aborted!\n" );
            else if( EC_USERABORT == evCode )
                printf_s( "\n\nRunning graph aborted on user request.\n" );
        }
        else if( E_ABORT == hr )
            printf_s( "\n\nRunning graph aborted on user request.\n" );
    }


    CLEAN_EXIT:

    // ->Stop() before the next waitForAnyKey() in order to release locked FILE pointers (file-writing samples)
    if( pMediaControl )
        pMediaControl->Stop();

    // give the user a chance to read EOS or eventual error messages (unless he aborted)
    if( E_ABORT != hr )
        waitForAnyKey( "\nPress any key to finish ...\n" );

    printf_s( "Clean all up..." );
    SAFE_RELEASE( pMediaEvent );
    SAFE_RELEASE( pMediaControl );

    if( dwGraphReg )
        RemoveGraphFromRot(dwGraphReg);

    if( pFilterGraph && pFileReader )
        NukeDownstream( pFilterGraph, pFileReader );
    // NukeDownstream() also cares about removing everything from the filter graph

    SAFE_RELEASE( pDemux );
    SAFE_RELEASE( pFileReader );
    SAFE_RELEASE( pOutPin );

    // don't forget to clean up the pin arrays
    for( i = 0; i < _MAX_PINS; i++ )
    {
        SAFE_RELEASE(pSinkArray[i]);
        SAFE_RELEASE(pOutPinArray[i]);
    }
    SAFE_DELETE_ARRAY(pSinkArray);
    SAFE_DELETE_ARRAY(pOutPinArray);

    UnInitFilterGraph( pFilterGraph, pGraphBuilder );
    CoUninitialize(); // release COM engine
    printf_s( "Done." );

    return 0;
}

