/**
@file mccolorimetry.h
@brief defines data structures related to video colorimetry used in different video codecs

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/

 #if !defined (__MC_COLORIMETRY_INCLUDED__)
#define __MC_COLORIMETRY_INCLUDED__

#include <string.h> // for memset
#include "mctypes.h"

/**
 * @brief SMPTE ST 2086 Mastering display color volume primaries.
 * Please refer "Mastering display colour volume SEI message semantics" section of Rec. ITU-T H.264 (D.2.29) or Rec. ITU-T H.265 (D.3.28) for more information.
 * @{
 **/
typedef struct smpte_st2086_mastering_display_metadata
{
    int32_t display_primaries_x[3];                     ///< The normalized x-coordinate chromaticity of the mastering display in increments of 0.00002 for 3 color components
    int32_t display_primaries_y[3];                     ///< The normalized y-coordinate chromaticity of the mastering display in increments of 0.00002 for 3 color components
    int32_t white_point_x;                              ///< The normalized x-coordinate chromaticity of the white point of the mastering display in normalized increments of 0.00002
    int32_t white_point_y;                              ///< The normalized y-coordinate chromaticity of the white point of the mastering display in normalized increments of 0.00002
    int64_t max_display_mastering_luminance;            ///< The nominal maximum display luminance of the mastering display in units of 0.0001 candelas per square metre
    int64_t min_display_mastering_luminance;            ///< The nominal minimum display luminance of the mastering display in units of 0.0001 candelas per square metre
} smpte_st2086_mastering_display_metadata_tt, display_metadata_t;
/** @} */

/**
* @brief Content Light Level is an additional Static Metadata item that applies to HDR Content only.
* Please refer "Content light level information SEI message semantics" section of Rec. ITU-T H.264 (D.2.31) or Rec. ITU-T H.265 (D.3.35) for more information.
* @{
**/
typedef struct content_light_level
{
    int32_t max_content_light_level;                    ///< Maximum Content Light Level (MaxCLL) in units of 1 candela per square metre
    int32_t max_pic_average_light_level;                ///< Maximum Picture-Average Light Level (MaxPALL, also known as MaxFALL) in units of 1 candela per square metre
} content_light_level_tt, content_light_level_t;
/** @} */

/** 
Colorimetry color primary values map directly to Table E.3 in Rec. ITU-T H.265 v4 (12/2016). This specification defines color coordinates for the red, green, blue and white points in XYZ color space.
 */
typedef enum color_primaries_e
{
    COLOR_PRIMARIES_AUTO                = -1,                       ///< Auto value
    COLOR_PRIMARIES_BT709_6             = 1,                        ///< R(0.640 0.330) G(0.300 0.600) B(0.150 0.060) W(0.3127 0.3290)
                                                                        ///< HDTV 1125- and 1250-line systems
                                                                        ///< BT.709-6
                                                                        ///< BT.1361
                                                                        ///< SMPTE RP 177
    COLOR_PRIMARIES_UNSPECIFIED         = 2,                        ///< Image characteristics are unknown or are determined by the application.
    COLOR_PRIMARIES_BT470_6_SYSTEM_M    = 4,                        ///< R(0.670 0.330) G(0.210 0.710) B(0.140 0.080) W(0.3101 0.3161)
                                                                        ///< NTSC 1953 color television transmission standard
                                                                        ///< BT.470-6 System M
    COLOR_PRIMARIES_BT470_6_SYSTEM_B_G  = 5,                        ///< R(0.640 0.330) G(0.290 0.600) B(0.150 0.060) W(0.3127 0.3290)
                                                                        ///< BT.470-6 System B, G
                                                                        ///< BT.601 625-line systems
                                                                        ///< BT.1358 625
                                                                        ///< BT.1700 625 PAL and 625 SECAM
    COLOR_PRIMARIES_SMPTE_170M          = 6,                        ///< R(0.630 0.340) G(0.310 0.595) B(0.155 0.070) W(0.3127 0.3290)
                                                                        ///< BT.601-6 525
                                                                        ///< BT.1358 525
                                                                        ///< BT.1700 NTSC
                                                                        ///< SMPTE 170M
                                                                        ///< SMPTE 240M
    COLOR_PRIMARIES_SMPTE_240M          = 7,                        ///< R(0.630 0.340) G(0.310 0.595) B(0.155 0.070) W(0.3127 0.3290)
                                                                        ///< SMPTE 240M
                                                                        ///< BT601 525-line systems
                                                                        ///< BT.601-6 525
                                                                        ///< BT.1358 525
                                                                        ///< BT.1700 NTSC
                                                                        ///< SMPTE 170M
    COLOR_PRIMARIES_GENERIC_FILM        = 8,                        ///< R(0.681 0.319) G(0.243 0.692) B(0.145 0.049) W(0.310 0.316)
                                                                        ///< Generic film (color filters using Illuminant C)
    COLOR_PRIMARIES_BT2020              = 9,                        ///< R(0.708 0.292) G(0.170 0.797) B(0.131 0.046) W(0.3127 0.3290)
                                                                        ///< ITU-R BT.2020-2
                                                                        ///< ITU-R BT.2100-0
    COLOR_PRIMARIES_SMPTE_ST_428_1      = 10,                       ///< Rx(1.0 0.0) Gx(0.0 1.0) Bx(0.0 0.0) W(1 ÷ 3 1 ÷ 3)
                                                                        ///< SMPTE ST 428-1 (CIE 1931 XYZ)
    COLOR_PRIMARIES_SMPTE_RP_431_2      = 11,                       ///< R(0.680 0.320) G(0.265 0.690) B(0.150 0.060) W(0.314 0.351)
                                                                        ///< Society of Motion Picture and Television Engineers RP 431 - 2 (2011)
    COLOR_PRIMARIES_SMPTE_EG_432_1      = 12,                       ///< R(0.680 0.320) G(0.265 0.690) B(0.150 0.060) W(0.3127 0.3290)
                                                                        ///< Society of Motion Picture and Television Engineers EG 432 - 1 (2010)
    COLOR_PRIMARIES_EBU_TECH_3213_E     = 22,                       ///< R(0.630 0.340) G(0.295 0.605) B(0.155 0.077) W(0.3127 0.3290)
                                                                        ///< EBU Tech. 3213-E (1975)
    COLOR_PRIMARIES_CUSTOM              = 256,                      ///< Custom values
} color_primaries_t;

/*! Colorimetry transfer characteristics map directly to Table E.4 in Rec. ITU-T H.265 v4 (02/2018).
HDR to HDR and HDR to SDR conversions are supported via the following transfer characteristics conversions:
- \ref TRANSFER_CHARACTERISTICS_SMPTE_ST_2084 "PQ" / HDR-10 to \ref TRANSFER_CHARACTERISTICS_BT2100_HLG "HLG"
- \ref TRANSFER_CHARACTERISTICS_BT2100_HLG "HLG" to \ref TRANSFER_CHARACTERISTICS_SMPTE_ST_2084 "PQ" / HDR-10
- \ref TRANSFER_CHARACTERISTICS_SMPTE_ST_2084 "PQ" / HDR-10 to SDR

where 
- SDR transfer characteristics are one of the following: \ref TRANSFER_CHARACTERISTICS_BT709_6 "BT.709",
\ref TRANSFER_CHARACTERISTICS_BT601_6 "BT.601",
\ref TRANSFER_CHARACTERISTICS_BT2020_10BIT "BT.2020 10 bit",
\ref TRANSFER_CHARACTERISTICS_BT2020_12BIT "BT.2020 12 bit",

- HDR-10 is BT.2020 (BT.2100) wide color gamut with \ref TRANSFER_CHARACTERISTICS_SMPTE_ST_2084 "SMPTE ST 2084 PQ transfer characteristic", \ref display_metadata_t "SMPTE ST 2086 Mastering Display Color Volume static metadata" and optionally the stream's \ref content_light_level_t "Content Light Level" metadata.
*/
typedef enum transfer_characteristics_e
{
    TRANSFER_CHARACTERISTICS_AUTO               = -1,               ///< Auto value
    TRANSFER_CHARACTERISTICS_BT709_6            = 1,                ///< V = 4.500 * L for 0.018 > L >= 0, V = 1.099 * L^0.45 ? 0.099 for 1 >= Lc >= 0.018
                                                                        ///< ITU-R BT.709-6
                                                                        ///< ITU-R Rec. BT.1361
                                                                        ///< ITU-R Rec. BT.601-6 525 or 625
                                                                        ///< ITU-R Rec. BT.1358 525 or 625
                                                                        ///< ITU-R Rec. BT.1700 NTSC
                                                                        ///< SMPTE 170M
                                                                        ///< ITU-R BT.2020-2
    TRANSFER_CHARACTERISTICS_UNSPECIFIED        = 2,                ///< Image characteristics are unknown or are determined by the application.
    TRANSFER_CHARACTERISTICS_BT470_6_SYSTEM_M   = 4,                ///< V = L ^ 1/2.2
                                                                        ///< ITU-R BT.470-6 System M
                                                                        ///< BT.1700 (2007 revision) 625 PAL and 625 SECAM
    TRANSFER_CHARACTERISTICS_BT470_6_SYSTEM_B_G = 5,                ///< V = L ^ 1/2.8
                                                                        ///< ITU-R Rec. BT.470-6 System B, G
    TRANSFER_CHARACTERISTICS_BT601_6            = 6,                ///< The same as the TRANSFER_CHARACTERISTICS_BT709_6
                                                                        ///< ITU-R Rec. BT.601-6 525 or 625
                                                                        ///< ITU-R BT.709-6
                                                                        ///< ITU-R Rec. BT.1361
                                                                        ///< ITU-R Rec. BT.1358 525 or 625
                                                                        ///< ITU-R Rec. BT.1700 NTSC
                                                                        ///< SMPTE 170M
                                                                        ///< ITU-R BT.2020-2
    TRANSFER_CHARACTERISTICS_SMPTE_240M         = 7,                ///< V = 4.0 * L for 0.0228 > L >= 0, V = 1.1115 * L^0.45 ? 0.1115 for 1 >= L >= 0.0228
                                                                        ///< SMPTE 240M
    TRANSFER_CHARACTERISTICS_LINEAR             = 8,                ///< V = L
    TRANSFER_CHARACTERISTICS_LOGARITHMIC_1      = 9,                ///< V = 1.0 + Log10(Lc) ÷ 2 for 1 >= Lc >= 0.01; V = 0.0 for 0.01 > Lc >= 0
                                                                        ///< Logarithmic transfer characteristic (100:1 range)
    TRANSFER_CHARACTERISTICS_LOGARITHMIC_2      = 10,               ///< V = 1.0 + Log10( Lc ) ÷ 2.5 for 1 >= Lc >= Sqrt( 10 ) ÷ 1000; V = 0.0 for Sqrt( 10 ) ÷ 1000 > Lc >= 0
                                                                        ///< Logarithmic transfer characteristic (100 * Sqrt( 10 ) : 1 range)
    TRANSFER_CHARACTERISTICS_IEC_61966_2_4      = 11,               ///< V = a * Lc0.45 − ( a − 1 ) for Lc >= b
                                                                    ///< V = 4.500 * Lc for b > Lc > −b
                                                                    ///< V = −a * (−Lc)0.45 + (a − 1) for −b >= Lc
                                                                        ///< IEC 61966-2-4
    TRANSFER_CHARACTERISTICS_BT_1361_0          = 12,               ///< V = a * Lc0.45 − ( a − 1 ) for 1.33 > Lc >= b
                                                                    ///< V = 4.500 * Lc for b > Lc >= −g
                                                                    ///< V = −(a * (−4 * Lc)0.45 −(a − 1)) ÷ 4 for −g > Lc >= −0.25
                                                                        ///< ITU-R BT.1361-0 extended color gamut system (historical)
    TRANSFER_CHARACTERISTICS_IEC_61966_2_1      = 13,               ///< V = a * Lc( 1 ÷ 2.4 ) − ( a − 1 ) for 1 >= Lc >= b
                                                                    ///< V = 12.92 * Lc for b > Lc >= 0
                                                                        ///< IEC 61966-2-1 sRGB or sYCC
    TRANSFER_CHARACTERISTICS_BT2020_10BIT       = 14,               ///< The same as the TRANSFER_CHARACTERISTICS_BT709_6
                                                                        ///< ITU-R BT.2020-2, 10 and 12 bit
                                                                        ///< ITU-R Rec. BT.601-6 525 or 625
                                                                        ///< ITU-R BT.709-5
                                                                        ///< ITU-R Rec. BT.1361
                                                                        ///< ITU-R Rec. BT.1358 525 or 625
                                                                        ///< ITU-R Rec. BT.1700 NTSC
                                                                        ///< SMPTE 170M
    TRANSFER_CHARACTERISTICS_BT2020_12BIT       = 15,               ///< The same as the TRANSFER_CHARACTERISTICS_BT709_6
                                                                        ///< ITU-R BT.2020-2, 10 and 12 bit
                                                                        ///< ITU-R Rec. BT.601-6 525 or 625
                                                                        ///< ITU-R BT.709-5
                                                                        ///< ITU-R Rec. BT.1361
                                                                        ///< ITU-R Rec. BT.1358 525 or 625
                                                                        ///< ITU-R Rec. BT.1700 NTSC
                                                                        ///< SMPTE 170M
    TRANSFER_CHARACTERISTICS_SMPTE_ST_2084      = 16,               ///< Society of Motion Picture and Television Engineers ST 2084 for 10, 12, 14, and 16 - bit systems (HDR10)
                                                                        ///< Rec.ITU - R BT.2100 perceptual quantization(PQ) system
    TRANSFER_CHARACTERISTICS_SMPTE_ST_428_1     = 17,               ///< Society of Motion Picture and Television Engineers ST 428 - 1
    TRANSFER_CHARACTERISTICS_BT2100_HLG         = 18,               ///< ITU - R BT.2100, Hybrid Log-Gamma (HLG) system reference non-linear transfer functions
    TRANSFER_CHARACTERISTICS_CUSTOM             = 256,              ///< Custom values, not supported yet

} transfer_characteristics_t;

/** @brief This defines coefficients for getting YUV->RGB or inverse conversion.
* Colorimetry matrix coefficients map directly to Table E.5 in Rec. ITU-T H.265 v4 (02/2018).
*
*   Some of code:
*
*   YUV->RGB
* @par
*     float d = (float)(u-128);<br>
*     float e = (float)(v-128);<br>
*     *r = (uint8_t)clip(y + e*c->Krv             + 0.5f, 0,255);<br>
*     *g = (uint8_t)clip(y + d*c->Kgu + e*c->Kgv  + 0.5f, 0,255);<br>
*     *b = (uint8_t)clip(y + d*c->Kbu             + 0.5f, 0,255);
*
*   RGB->YUV
* @par
*     float luma = c->Kyr*r + c->Kyg*g + c->Kyb*b;<br>
*     *u = (uint8_t)clip((b - luma)*c->Nu + 128.5, 0, 255);<br>
*     *v = (uint8_t)clip((r - luma)*c->Nv + 128.5, 0, 255);<br>
*     *y = (uint8_t)clip(luma + 0.5f, 0, 255);
*
*   And in theory:
* <table>
*   <tr><td>( Y )</td><td>( Kyr Kyg Kyb )</td><td>( R )</td></tr>
* <tr><td>( U ) =</td><td>( Kur Kug Kub ) *</td><td>( G )</td></tr>
*   <tr><td>( V )</td><td>( Kvr Kvg Kvb )</td><td>( B )</td></tr>
* </table>
* , where:
*
* Kur = -Kyr;<br>
* Kug = -Kyg;<br>
* Kub = 1 - Kyb;
*
* Kvr = 1 - Kyr;<br>
* Kvg = -Kyg;<br>
* Kvb = -Kyb;
*/
typedef enum matrix_coefficients_e
{
    MATRIX_COEFFICIENTS_AUTO                    = -1,                           ///< Auto value.
    MATRIX_COEFFICIENTS_IDENTITY                = 0,                            ///< /The identity matrix.
                                                                                    ///< Typically used for GBR(often referred to as RGB); however, may also be used for YZX(often referred to as XYZ)
                                                                                    ///< IEC 61966 - 2 - 1 sRGB
                                                                                    ///< SMPTE ST 428 - 1
    MATRIX_COEFFICIENTS_BT709_6                 = 1,                            ///< Kyr = 0.2126, Kyb = 0.0722
                                                                                    ///< ITU-R Rec. BT.709-6
                                                                                    ///< ITU-R Rec. BT.1361
    MATRIX_COEFFICIENTS_UNSPECIFIED             = 2,                            ///< Image characteristics are unknown or are determined by the application
    MATRIX_COEFFICIENTS_FCC                     = 4,                            ///< Kyr = 0.30, Kyb = 0.11
                                                                                    ///< ITU-T Recommendation H.264 | ISO/IEC 14496-10 AVC
    MATRIX_COEFFICIENTS_BT470_6_SYSTEM_B_G      = 5,                            ///< Kyr = 0.299, Kyb = 0.114
                                                                                    ///< ITU-R Rec. BT.470-6 System B, G (historical)
                                                                                    ///< ITU-R Rec. BT.601 - 6 625
                                                                                    ///< ITU-R Rec. BT.1358 - 1 625 (historical)
                                                                                    ///< ITU-R Rec. BT.1700 - 0 625 PAL and 625 SECAM
                                                                                    ///< IEC 61966 - 2 - 4 xvYCC601
    MATRIX_COEFFICIENTS_BT601_6                 = 6,                            ///< Kyr = 0.299, Kyb = 0.114
                                                                                    ///< ITU-R Rec. BT.601-6 525
                                                                                    ///< ITU-R Rec.BT.1358 - 1 525 (historical)
                                                                                    ///< ITU-R Rec.BT.1700 - 0 NTSC
                                                                                    ///< SMPTE 170M(2004)
    MATRIX_COEFFICIENTS_SMPTE_240M              = 7,                            ///< Kyr = 0.212, Kyb = 0.087
                                                                                    ///< SMPTE 240M
    MATRIX_COEFFICIENTS_YCGCO                   = 8,                            ///< YCgCo
    MATRIX_COEFFICIENTS_BT2020_NON_CONST        = 9,                            ///< Kyr = 0.2627, Kyb = 0.0593
                                                                                    ///< ITU-R Rec. BT.2020-2 non-const luma
    MATRIX_COEFFICIENTS_BT2020_CONST            = 10,                           ///< Kyr = 0.2627, Kyb = 0.0593
                                                                                    ///< ITU-R Rec. BT.2020-2 const luma
    MATRIX_COEFFICIENTS_SMPTE_2085              = 11,                           ///< YDzDx
                                                                                ///< SMPTE ST 2085 (2015)
    MATRIX_COEFFICIENTS_CHROM_DERIVED_NON_CONST = 12,                           ///< Chromaticity-derived non-constant luminance system
    MATRIX_COEFFICIENTS_CHROM_DERIVED_CONST     = 13,                           ///< Chromaticity-derived constant luminance system
    MATRIX_COEFFICIENTS_BT_2100_0               = 14,                           ///< ICTCP
                                                                                    ///< Rec. ITU-R BT.2100-0
    MATRIX_COEFFICIENTS_CUSTOM                  = 256,                          ///< Custom values
} matrix_coefficients_t;

/** @brief This defines colorimetry values for color primaries, transfer characteristics, matrix coefficients.
*/
typedef enum color_description_e
{
    COLOR_DESCRIPTION_AUTO             = -1,    ///< Auto colorimetry values
    COLOR_DESCRIPTION_DEFAULT          = 0,     ///< Default colorimetry values
    COLOR_DESCRIPTION_UNSPECIFIED,              ///< Unspecified color description
    COLOR_DESCRIPTION_BT601_625,                ///< ITU-R Rec. BT.601-6 625
    COLOR_DESCRIPTION_BT601_525,                ///< ITU-R Rec. BT.601-6 525
    COLOR_DESCRIPTION_BT709,                    ///< ITU-R BT.709-6
    COLOR_DESCRIPTION_SMPTE240,                 ///< SMPTE 240M
    COLOR_DESCRIPTION_BT2020_10BIT,             ///< ITU-R BT.2020-2 10 bit
    COLOR_DESCRIPTION_BT2020_12BIT,             ///< ITU-R BT.2020-2 12 bit
    COLOR_DESCRIPTION_BT2100_PQ,                ///< ITU-R BT.2100 perceptual quantization(PQ) system
    COLOR_DESCRIPTION_BT2100_HLG,               ///< ITU-R BT.2100, Hybrid Log-Gamma (HLG) system
} color_description_t;

/**
* @name Picture format flag enumeration
* Use to set values for flag parameters of @ref pic_format_t structure: progressive_frame_flag, video_full_range_flag, top_field_first.
* @{
**/
typedef enum pic_signal_range_flag
{
    PIC_SIGNAL_RANGE_AUTO = 0,           ///< Signal range auto
    PIC_SIGNAL_RANGE_FULL = 1,           ///< Signal range full (0-255)
    PIC_SIGNAL_RANGE_SHORT = 2           ///< Signal range short (16-235)
} pic_signal_range_flag_t;
/**
* @}
**/

/**
* @brief A set of common mastering display color primaries that can be used to configure the values in @ref smpte_st2086_mastering_display_metadata
via the @ref set_mastering_display_parameters function.
* @{
**/
typedef enum mastering_display_color_primaries
{
    MASTERING_DISPLAY_COLOR_PRIMARIES_BT2020 = 1,   ///< BT.2020 color primaries
    MASTERING_DISPLAY_COLOR_PRIMARIES_DCIP3 = 2     ///< DCI-P3 color primaries
} mastering_display_color_primaries_t;
/**
* @}
**/

/**
* @brief A set of common mastering display white points that can be used to configure the values in the @ref smpte_st2086_mastering_display_metadata
via the @ref set_mastering_display_parameters function.
* @{
**/
typedef enum mastering_display_white_point
{
    MASTERING_DISPLAY_WHITE_POINT_D65 = 1   ///< D65 white point
} mastering_display_white_point_t;
/**
* @}
**/

///  @brief Colorimetry.
/// Call @ref set_colorimetry_from_color_description function to initialize the structure parameters by color description
typedef struct
{
    color_primaries_t   color_primaries;                    ///< Chrominance coordinates of the source primaries. One of @ref color_primaries_t.
                                                            /**
                                                            * @name Custom coordinates
                                                            * If @ref color_primaries is equal to @ref COLOR_PRIMARIES_CUSTOM.
                                                            * @{
                                                            **/
    float    coordinate_red_x;                              ///< X coordinate of red color.
    float    coordinate_red_y;                              ///< Y coordinate of red color.
    float    coordinate_green_x;                            ///< X coordinate of green color.
    float    coordinate_green_y;                            ///< Y coordinate of green color.
    float    coordinate_blue_x;                             ///< X coordinate of blue color.
    float    coordinate_blue_y;                             ///< Y coordinate of blue color.
    float    coordinate_reference_white_x;                  ///< X coordinate of reference white color.
    float    coordinate_reference_white_y;                  ///< Y coordinate of reference white color.
                                                            /** @} */

    transfer_characteristics_t  transfer_characteristics;   ///< Gamma correction function. One of @ref transfer_characteristics_t.

    matrix_coefficients_t   matrix_coefficients;            ///< Matrix coefficients used in deriving luminance and chrominance signals from the green, blue, and red primaries. One of @ref matrix_coefficients_t.

                                                            /**
                                                            * @name Custom matrix
                                                            * If @ref matrix_coefficients is equal to @ref MATRIX_COEFFICIENTS_CUSTOM.
                                                            * @{
                                                            **/
    float       matrix_coeff_Kyr;                           ///< Coefficient of R signal.
    float       matrix_coeff_Kyb;                           ///< Coefficient of B signal.
                                                            /** @} */

    pic_signal_range_flag_t signal_range;                   ///< Indicates that frame is full range or short range.
    display_metadata_t display_metadata;                    ///< SMPTE ST 2086 Mastering display color volume primaries
    content_light_level_t content_light_level;              ///< Content Light Level is an additional Static Metadata item that applies to HDR Content only.
    uint8_t     content_light_level_present;                ///< Indicates that content light level is presented.
    uint8_t     display_metadata_present;                   ///< Indicates that mastering display metadata is presented.
    uint8_t     reserved[6];
    int32_t     *p_reserved[4];
} colorimetry_t;                                            ///< Colorimetry structure

/** @brief Call this function to configure the color primaries, transfer characteristics and matrix coefficients of a @ref colorimetry_t structure from a @ref color_description_t value.
*
* @param[in] color_description value of color_description_t
* @param[out] colorimetry pointer to @ref colorimetry_t structure
* @return 0 if the pointer of colorimetry_t is correct\n
*         1 if not
*/
static __inline int32_t set_colorimetry_from_color_description(colorimetry_t *colorimetry, color_description_t color_description)
{
    if(!colorimetry)
        return 1;

    memset(colorimetry, 0, sizeof(colorimetry_t));
    switch (color_description)
    {
    case (COLOR_DESCRIPTION_AUTO):
        colorimetry->color_primaries = COLOR_PRIMARIES_AUTO;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_AUTO;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_AUTO;
        break;
    case (COLOR_DESCRIPTION_UNSPECIFIED):
        colorimetry->color_primaries = COLOR_PRIMARIES_UNSPECIFIED;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_UNSPECIFIED;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_UNSPECIFIED;
        break;
    case (COLOR_DESCRIPTION_BT2020_10BIT):
        colorimetry->color_primaries = COLOR_PRIMARIES_BT2020;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2020_10BIT;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT2020_NON_CONST;
        break;
    case (COLOR_DESCRIPTION_BT2020_12BIT):
        colorimetry->color_primaries = COLOR_PRIMARIES_BT2020;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2020_12BIT;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT2020_NON_CONST;
        break;
    case (COLOR_DESCRIPTION_BT2100_PQ):
        colorimetry->color_primaries = COLOR_PRIMARIES_BT2020;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_ST_2084;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT2020_NON_CONST;
        break;
    case (COLOR_DESCRIPTION_BT2100_HLG):
        colorimetry->color_primaries = COLOR_PRIMARIES_BT2020;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT2100_HLG;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT2020_NON_CONST;
        break;
    case (COLOR_DESCRIPTION_BT601_625):
        colorimetry->color_primaries = COLOR_PRIMARIES_BT470_6_SYSTEM_B_G;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT601_6;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT470_6_SYSTEM_B_G;
        break;
    case (COLOR_DESCRIPTION_BT601_525):
        colorimetry->color_primaries = COLOR_PRIMARIES_SMPTE_170M;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT601_6;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT601_6;
        break;
    case (COLOR_DESCRIPTION_SMPTE240):
        colorimetry->color_primaries = COLOR_PRIMARIES_SMPTE_240M;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_SMPTE_240M;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_SMPTE_240M;
        break;
    case (COLOR_DESCRIPTION_BT709):
    case (COLOR_DESCRIPTION_DEFAULT):
    default:
        colorimetry->color_primaries = COLOR_PRIMARIES_BT709_6;
        colorimetry->transfer_characteristics = TRANSFER_CHARACTERISTICS_BT709_6;
        colorimetry->matrix_coefficients = MATRIX_COEFFICIENTS_BT709_6;
        break;
    }
    return 0;
}

/** @brief Helper function to more easily configure the display colour primaries and white point chromaticity of a @ref display_metadata_t structure
using the @ref mastering_display_color_primaries_t and @ref mastering_display_white_point_t enumerations of common master display configurations.
* @note This does not set the max_display_mastering_luminance and min_display_mastering_luminance values, which must be configured separately.
*
* @param[in] color_primaries value of @ref mastering_display_color_primaries_t
* @param[in] white_point value of @ref mastering_display_white_point_t
* @param[out] display metadata pointer to @ref display_metadata_t structure
* @return 0 if input parameters are correct\n
*         1 if not, or unsupported color_primaries or white_point value is given
*/
static __inline int32_t set_mastering_display_parameters(
    display_metadata_t* metadata, mastering_display_color_primaries_t color_primaries, mastering_display_white_point_t white_point)
{
    if (!metadata || white_point != MASTERING_DISPLAY_WHITE_POINT_D65 ||
        (color_primaries != MASTERING_DISPLAY_COLOR_PRIMARIES_BT2020 && color_primaries != MASTERING_DISPLAY_COLOR_PRIMARIES_DCIP3)) {
        return 1;
    }

    switch (color_primaries) {
    case MASTERING_DISPLAY_COLOR_PRIMARIES_BT2020:
        metadata->display_primaries_x[0] = 35400;
        metadata->display_primaries_x[1] = 8500;
        metadata->display_primaries_x[2] = 6550;
        metadata->display_primaries_y[0] = 14600;
        metadata->display_primaries_y[1] = 39850;
        metadata->display_primaries_y[2] = 2300;
        break;
    case MASTERING_DISPLAY_COLOR_PRIMARIES_DCIP3:
        metadata->display_primaries_x[0] = 34000;
        metadata->display_primaries_x[1] = 13250;
        metadata->display_primaries_x[2] = 7500;
        metadata->display_primaries_y[0] = 16000;
        metadata->display_primaries_y[1] = 34500;
        metadata->display_primaries_y[2] = 3000;
        break;
    }

    metadata->white_point_x = 15635;
    metadata->white_point_y = 16450;

    return 0;
}

static __inline void adjust_colorimetry(colorimetry_t* colorimetry, const colorimetry_t* adjuster)
{
    if(adjuster->color_primaries != COLOR_PRIMARIES_AUTO)
    {
        colorimetry->color_primaries = adjuster->color_primaries;
        if (colorimetry->color_primaries == COLOR_PRIMARIES_CUSTOM)
        {
            colorimetry->coordinate_red_x = adjuster->coordinate_red_x;
            colorimetry->coordinate_red_y = adjuster->coordinate_red_y;
            colorimetry->coordinate_green_x = adjuster->coordinate_green_x;
            colorimetry->coordinate_green_y = adjuster->coordinate_green_y;
            colorimetry->coordinate_blue_x = adjuster->coordinate_blue_x;
            colorimetry->coordinate_blue_y = adjuster->coordinate_blue_y;
            colorimetry->coordinate_reference_white_x = adjuster->coordinate_reference_white_x;
            colorimetry->coordinate_reference_white_y = adjuster->coordinate_reference_white_y;
        }
    }
    if(adjuster->transfer_characteristics != TRANSFER_CHARACTERISTICS_AUTO)
        colorimetry->transfer_characteristics = adjuster->transfer_characteristics;
    if(adjuster->matrix_coefficients != MATRIX_COEFFICIENTS_AUTO)
    {
        colorimetry->matrix_coefficients = adjuster->matrix_coefficients;
        if (colorimetry->matrix_coefficients == MATRIX_COEFFICIENTS_CUSTOM)
        {
            colorimetry->matrix_coeff_Kyr = adjuster->matrix_coeff_Kyr;
            colorimetry->matrix_coeff_Kyb = adjuster->matrix_coeff_Kyb;
        }
    }

    if (adjuster->display_metadata_present)
    {
        colorimetry->display_metadata_present = 1;
        colorimetry->display_metadata = adjuster->display_metadata;
    }

    if (adjuster->content_light_level_present)
    {
        colorimetry->content_light_level_present = 1;
        colorimetry->content_light_level = adjuster->content_light_level;
    }
    if(adjuster->signal_range != PIC_SIGNAL_RANGE_AUTO)
        colorimetry->signal_range = adjuster->signal_range;
}

/**
* @name Picture colorimetry structure
* @{
**/
typedef struct
{
    uint8_t                                         mastering_display_metadata_present;     ///< Indicates that mastering display metadata is presented.
    smpte_st2086_mastering_display_metadata_tt      mastering_display_metadata;
    uint8_t                                         content_light_level_present;            ///< Indicates that content light level is presented.
    content_light_level_tt                          content_light_level;
    uint8_t                                         colorimetry_present;                    ///< Indicates that colorimetery is presented.
    colorimetry_t                                   colorimetry;
    uint8_t                                         reserved[16];
} colorimetry_info_t;
/**
* @}
**/

#endif
