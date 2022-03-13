/* ----------------------------------------------------------------------------
 * File: mcindextypes.h
 *
 * Desc: index definitions, used mainly by the MPEG muxers and demuxers
 *
 * Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 *
 * ----------------------------------------------------------------------------
 */

#if !defined (__MC_INDEXTYPES_INCLUDED__)
#define __MC_INDEXTYPES_INCLUDED__

#include "mctypes.h"


// The index layout is shown below. A file header is followed by a number of index headers
// (and the associated index entries) or external index headers. It is allowed to have a 
// mixture of index and external index headers:
//
//     mcidx_file_header_t, nIndexCount must be > 0
//
//        mcidx_ext_index_header_t, index "0"
//
//        mcidx_index_header_t, index "1", nAUCount must be > 0 
//            mcidx_au_entry_t, AU 0 of index "1"
//            ....
//            mcidx_au_entry_t, AU "nAUCount - 1" of index "1"
//
//        mcidx_ext_index_header_t, index "2"
//
//        ...
//
//        mcidx_index_header_t, index "nIndexCount - 1", nAUCount must be > 0
//            mcidx_au_entry_t, AU 0 of index "nIndexCount - 1"
//            ....
//            mcidx_au_entry_t, AU "nAUCount - 1" of index "nIndexCount - 1"
//
//
//
// An external index header refers to an external file that contains a single index
// header and its associated index entries:
//
//        mcidx_index_header_t, nAUCount must be > 0 
//            mcidx_au_entry_t, AU 0
//            ....
//            mcidx_au_entry_t, AU "nAUCount - 1"
//


// String coding type flags, common for name string type fields, if none of these
// flags are specified, the name string is considered to be a UTF-8 string
#define MCIDX_STRING_IS_UNICODE             0x80000000    // string is UNICODE


#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
// Index file header definition

#define MCIDX_FILE_HDR_ID                   "MCIDX_FILE_HDRID"

// format version and sizes
#define MCIDX_FILE_HDR_VERSION_0100         0x0100        // version 0100
#define MCIDX_FILE_HDR_VERSION_0100_SIZE    32            // size is 32 bytes

typedef struct mcidx_file_header_s
{        
    uint8_t  fileIdentifier[16];            // Set to MCIDX_FILE_HDR_ID

    uint16_t nVersion;                      // Version of this structure, one of the MCIDX_FILE_HDR_VERSION_xxx defines

    uint16_t nReserved;                     // Reserved

    uint32_t nFlags;                        // Zero or more of the MCIDX_STRING_xxx flags

    uint32_t nContainerType;                // Container type, one of the mcmajortypes_t enums in mcmediatypes.h

    uint16_t nIndexCount;                   // Number of indexes contained in this file

    uint16_t nMediaNameByteLength;          // The length in bytes of the media name string that immediately
                                            // follows this structure, can be 0. The string is considered to
                                            // a UTF-8 string unless the nFlags field indicates otherwise.
                                            // This name refers to the media that contains the streams
                                            // referred to by the indexes.

    // end of version 0x0100, size is 32 bytes

} mcidx_file_header_t;


/////////////////////////////////////////////////////////////////////////////////////////////////
// Index external header definition

#define MCIDX_INDEX_EXT_ID                  "MCIDX_INDX_EXTID"

// format versions and sizes
#define MCIDX_INDEX_EXT_VERSION_0100        0x0100        // version 0100
#define MCIDX_INDEX_EXT_VERSION_0100_SIZE   24            // size is 32 bytes

typedef struct mcidx_ext_index_header_s
{
    uint8_t  indexIdentifier[16];           // Set to MCIDX_INDEX_EXT_ID

    uint16_t nVersion;                      // Version of this structure, one of the MCIDX_INDEX_EXT_VERSION_xxx defines

    uint32_t nFlags;                        // Zero or more of the MCIDX_STRING_xxx flags

    uint16_t nExtNameByteLength;            // The length in bytes of the external index name string
                                            // that immediately follows this structure, must be > 0.
                                            // The string is considered to a UTF-8 string unless the
                                            // nFlags field indicates otherwise.

    // end of version 0x0100, size is 24 bytes

} mcidx_ext_index_header_t;


/////////////////////////////////////////////////////////////////////////////////////////////////
// Index header definition

#define MCIDX_INDEX_HDR_ID                  "MCIDX_INDX_HDRID"

// the type of information this index holds
#define MCIDX_INDEX_HDR_TYPE_PRIVATE        0             // uses a private structure
#define MCIDX_INDEX_HDR_TYPE_AU_LIST        1             // uses the mcidx_au_entry_t structure below

// flags
#define MCIDX_INDEX_HDR_PROGRAM_VALID       0x00000001    // nProgramNum field is valid
#define MCIDX_INDEX_HDR_PID_VALID           0x00000002    // nPID field is valid
#define MCIDX_INDEX_HDR_ESID_VALID          0x00000004    // nStreamId field is valid
#define MCIDX_INDEX_HDR_SUBID_VALID         0x00000008    // nSubStreamId field is valid

// format versions and sizes
#define MCIDX_INDEX_HDR_VERSION_0100        0x0100        // version 0100
#define MCIDX_INDEX_HDR_VERSION_0100_SIZE   56            // size is 56 bytes

#pragma pack(push, 1)
typedef struct mcidx_index_header_s
{
    uint8_t  indexIdentifier[16];           // Set to MCIDX_INDEX_HDR_ID

    uint16_t nVersion;                      // Version of this structure, one of the MCIDX_INDEX_HDR_VERSION_xxx defines

    uint16_t nIndexType;                    // the content type of this index, one of the MCIDX_INDEX_HDR_TYPE_xxx defines

    uint16_t nIndexTypeVersion;             // Version of the structures used in this index

    uint16_t nReserved1;                    // Reserved

    int32_t  nIndexTypeSize;                // Size in bytes of the structures used in this index

    uint64_t nItemCount;                    // Number of items in this index

    uint32_t nFlags;                        // Zero or more of the MCIDX_INDEX_HDR_xxx flags

    uint32_t nStreamType;                   // Stream type, one of the mcmediatypes_t enums in mcmediatypes.h
    
    uint16_t nProgramNum;                   // Stream program number
    
    uint16_t nPID;                          // Stream PID
    
    uint8_t  nStreamId;                     // Stream PES/Elementary ID
    
    uint8_t  nSubStreamId;                  // Stream SubStream ID

    uint16_t nReserved2;                    // Reserved

    uint16_t nFormatSize;                   // Size in bytes of the format structure that immediately
                                            // follows this structure, depends on streamType field, can be 0
                                            // usually one of sizeof(mc_audio_format_t) or sizeof(mc_video_format_t)

    uint16_t nDecoderConfigSize;            // Size in bytes of the decoder config data that immediately
                                            // follows this structure (or the format structure if present), can be 0

    // end of version 0100, size is 56 bytes

} mcidx_index_header_t;
#pragma pack(pop)

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access Unit entries

// common flags
#define MCIDX_AU_TIMESTAMP_IS_VALID         0x80000000    // nTimestamp field is valid
#define MCIDX_AU_GEN_TIMESTAMP_IS_VALID     0x40000000    // nGenTimestamp field is valid
#define MCIDX_AU_TIME_DISCON                0x20000000    // A time discontinuity occurs at this access unit
#define MCIDX_AU_TIMESTAMPS_27MHz_UNITS     0x10000000    // nTimestamp and nGenTimestamp fields are in 27MHz units
#define MCIDX_AU_TIMESTAMPS_90KHz_UNITS     0x08000000    // nTimestamp and nGenTimestamp fields are in 90KHz units

// video specific flags
#define MCIDX_VAU_IDR_TYPE                  0x00000001    // IDR type access unit
#define MCIDX_VAU_I_TYPE                    0x00000002    // I type access unit
#define MCIDX_VAU_P_TYPE                    0x00000004    // P type access unit
#define MCIDX_VAU_B_TYPE                    0x00000008    // B type access unit
#define MCIDX_VAU_SPS_PRESENT               0x00000010    // Sequence type hdr present, if known
#define MCIDX_VAU_GOP_PRESENT               0x00000020    // GOP type hdr present, if known
#define MCIDX_VAU_RFF                       0x00000040    // Repeat first field flag, if known
#define MCIDX_VAU_TFF                       0x00000080    // Top field first flag, if known
#define MCIDX_VAU_PROG                      0x00000100    // Progressive frame, if known
#define MCIDX_VAU_INTERLACED                0x00000200    // Interlaced frame, if known
#define MCIDX_VAU_RP_TYPE                   0x00000400    // Recovery point type access unit

// format versions and sizes
#define MCIDX_AU_ENTRY_VERSION_0100         0x0100        // version 0100, use in the mcidx_index_header_t.nAUFormatVersion field
#define MCIDX_AU_ENTRY_VERSION_0100_SIZE    32            // size is 32 bytes

// access Unit entry type
typedef struct mcidx_au_entry_s
{
    uint32_t nFlags;                        // Zero or more of the AU flags
    
    uint32_t nOffset;                       // Relative byte offset of AU from nPackOffset if not an elementary stream
                                            // Not used for elementary streams, set to 0

    uint64_t nPackOffset;                   // Absolute byte offset of pack that contains the AU if not an elementary stream
                                            // Absolute byte offset of AU if an elementary stream

    uint64_t nTimestamp;                    // Actual timestamp in 100ns units, unless specified otherwise with a MCIDX_AU_TIMESTAMPS_xxx_UNITS flag
    
    uint64_t nGenTimestamp;                 // Generated timestamp in 100ns units, unless specified otherwise with a MCIDX_AU_TIMESTAMPS_xxx_UNITS flag

    // end of version 0100, size is 32 bytes

} mcidx_au_entry_t;


#pragma pack(pop)


#endif // #if !defined (__MC_INDEXTYPES_INCLUDED__)
