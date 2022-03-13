/** 
 @file  dec_aac_mc.h
 @brief  Property GUIDs for MainConcept AAC decoder parameters.
 
 @verbatim
 File: dec_aac_mc.h

 Desc: Property GUIDs for MainConcept AAC decoder parameters.
 
 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/
 
#if !defined(MC_AAC_DECODER_CONFIG)
#define MC_AAC_DECODER_CONFIG

/**
* namespace MCAACDEC
* @brief AAC Decoder namespace
**/
namespace MCAACDEC
{
	/// @brief Channel delivery configuration
    typedef enum
    {
        Deliver_Both        = 0,		///< Both
        Deliver_First       = 1,		///< First
        Deliver_Second      = 2			///< Second

    } DeliverConfig_t;

	/// @brief Output bit depth
    typedef enum
    {
        Depth_8bit          = 0,		///< 8 bit
        Depth_16bit         = 1,		///< 16 bit
        Depth_24bit         = 2,		///< 24 bit
        Depth_32bit         = 3,		
        Depth_32bit_float   = 4			

    } BitDepth_t;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  IModuleConfig GUIDs
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///@brief Specifies the output channels' configuration
// {12FE5670-DCC2-4120-A6EF-E6E85BC20000}
static const GUID IMC_AADEC_DeliverChan =
{ 0x12fe5670, 0xdcc2, 0x4120, {0xa6, 0xef, 0xe6, 0xe8, 0x5b, 0xc2, 0x00, 0x00 }};

///@brief Specifies output bit depth
// {12FE5670-DCC2-4120-A6EF-E6E85BC20001}
static const GUID IMC_AADEC_BitDepth =
{ 0x12fe5670, 0xdcc2, 0x4120, {0xa6, 0xef, 0xe6, 0xe8, 0x5b, 0xc2, 0x00, 0x01 }};

///@brief Specifies whether silence should be added on bitstream errors or not
// {47AC07EE-BF04-474b-8861-D013B642C0F0}
static const GUID IMC_AADEC_SilenceOnError = 
{ 0x47ac07ee, 0xbf04, 0x474b, { 0x88, 0x61, 0xd0, 0x13, 0xb6, 0x42, 0xc0, 0xf0 } };

///@brief Mutes the output
// {E1998DB3-A3FF-42a2-93BC-D43D4790082E}
static const GUID IMC_AADEC_Mute = 
{ 0xe1998db3, 0xa3ff, 0x42a2, { 0x93, 0xbc, 0xd4, 0x3d, 0x47, 0x90, 0x8, 0x2e } };

///@brief To ignore or not CRC errors
// {CC3559E5-993B-4eaa-9BE2-3FA0C10F04FA}
static const GUID IMC_AADEC_IgnoreCRCErrors = 
{ 0xcc3559e5, 0x993b, 0x4eaa, { 0x9b, 0xe2, 0x3f, 0xa0, 0xc1, 0xf, 0x4, 0xfa } };




///@brief To get metadata information
// {C28499D7-D44B-48DD-9828-AFFB4A8F6233}
static const GUID IMC_AADEC_Metadata = 
{ 0xc28499d7, 0xd44b, 0x48dd, { 0x98, 0x28, 0xaf, 0xfb, 0x4a, 0x8f, 0x62, 0x33 } };

#endif // MC_AAC_DECODER_CONFIG

