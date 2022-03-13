/**
\file   dec_hevc.h
\brief  HEVC Decoder API

@copyright
Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.<br>
MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
**/
#ifndef __DEC_HEVC_API_INCLUDED__
#define __DEC_HEVC_API_INCLUDED__

#include "bufstrm.h"
#include "mcdefs.h"
#include "mcapiext.h"
#include <mcruntime.h>
#include <mccolorimetry.h>


/** \name Maximums and minimums */
#define TEMPORAL_LAYERS_COUNT       7         /**< \brief Maximum number of temporal layers \hideinitializer */
#define VPS_LAYER_SETS_COUNT        1024      /**< \brief Maximum number of HRD parameter sets allowed in VPS \hideinitializer */
#define NUH_LAYERS_COUNT            64         /**< \brief NAL Unit layer count \hideinitializer */
#define MAX_NUM_NAL_UNITS           1024      /**< \brief Maximum number of NAL units per picture (access unit) \hideinitializer */
#define MAX_NUM_TILE_COLUMNS        20        /**< \brief Maximum number of tile columns per picture \hideinitializer */
#define MAX_NUM_TILE_ROWS           22        /**< \brief Maximum number of tile rows per picture \hideinitializer */
#define MAX_NUM_TILES               (MAX_NUM_TILE_COLUMNS * MAX_NUM_TILE_ROWS) /**< \brief Maximum number of tiles per picture \hideinitializer */
#define MAX_NUM_ROWS                (32768 / 16) /**< \brief Maximum number of rows (max height divided by min CTU size) \hideinitializer */
#define MAX_NUM_ENTRY_POINT_OFFSETS (MAX_NUM_ROWS + 1) /**< \brief Maximum number of entry point offsets \hideinitializer */

/** \name Auxiliary commands, also see mcdefs.h */
#define SET_CHUNK_PARSE_MODE        DEC_AVC_RESERVED_00010081       /**< \brief Set chunk parse mode \hideinitializer */
#define SET_DBL_FILTER_MODE         0x00010082                      /**< \brief Set deblocking filter mode option \hideinitializer */
#define SET_SAO_FILTER_MODE         0x00010083                      /**< \brief Set sao filter mode option \hideinitializer */
#define SET_ASYNC_INPUT_OUTPUT_MODE DEC_AVC_RESERVED_00010084       /**< \brief Set callback mode only mode offs = 1, in this mode \ref bufstream#copybytes works asynchronous with \ref pic_output_callback_t  \hideinitializer */
#define SET_MAX_TEMPORAL_LAYER      DEC_AVC_RESERVED_0001008d       /**< \brief set maximum layer to be decode \hideinitializer */
#define SET_PREVIEW_MODE            DEC_AVC_RESERVED_0001008f       /**< \brief set decoder preview mode \hideinitializer */
#define GET_HEVC_VID_PAR_SET        0x00010090                      /**< \brief Get VPS information \hideinitializer */
#define GET_HEVC_SEQ_PAR_SET        0x00010091                      /**< \brief Get SPS information \hideinitializer */
#define GET_HEVC_PIC_PAR_SET        0x00010092                      /**< \brief Get PPS information \hideinitializer */
#define SET_MAX_OVERLAPPED_PICTURES DEC_AVC_RESERVED_00010302       /**< \brief Sets maximum number of extra pictures to decode in parallel. See \ref DEC_HEVC_OVERLAPPING. \hideinitializer */
#define GET_PARSED_SEI              0x00010303                      /**< \brief Get parsed SEI message \hideinitializer */
#define SET_HW_ADAPTER              0x00010304                      /**< \brief Select a GPU for HW-accelerated decoding. \hideinitializer */
#define RELEASE_PIC                 DEC_AVC_RESERVED_0001008c       /**< \brief Performs a release operation on the \ref hevc_picture_t. \hideinitializer */
#define ADDREF_PIC                  DEC_AVC_RESERVED_0001008b       /**< \brief Increments the reference count of the \ref hevc_picture_t. \hideinitializer */
#define SET_MAX_HELD_PICTURES       DEC_AVC_RESERVED_00010089       /**< \brief Set the number of pictures that can be locked by \ref ADDREF_PIC \hideinitializer */
#define GET_NATIVE_FOURCC           DEC_AVC_RESERVED_00010305       /**< \brief Return the \ref FourCC of the current picture \ref DEC_HEVC_GET_NATIVE_FOURCC_EXAMPLE \hideinitializer */
#define HEVCVD_SET_DEINTERLACING_MODE DEC_AVC_RESERVED_00010086     /**< \brief Set deinterlacing mode. Look at \ref hevc_deinterlacing_mode_t and \ref hevc_get_pic_t for more details. \hideinitializer */

/** \brief Chunk parse mode  */
typedef enum hevc_chunk_parse_mode_e {
    HEVCVD_CP_RANDOM = 0,           /**< \brief Input chunks are random */
    HEVCVD_CP_NALU = 1,             /**< \brief Input chunk contains set (at least one) of complete NAL units */
    HEVCVD_CP_AU = 2                /**< \brief Input chunk contains set (at least one) of complete access units (frames) */
} hevc_chunk_parse_mode_t;

/** \enum hevc_loop_filter_mode_t
 *  \brief Loop filter modes
 */
typedef enum hevc_loop_filter_mode_e {
    HEVCVD_LF_OFF = 0,              /**< \brief Skips in-loop filter for all pictures – may produce artifacts */
    HEVCVD_LF_STANDARD = 1,         /**< \brief Respects stream settings */
    HEVCVD_LF_REFERENCE_ONLY = 2    /**< \brief Runs in-loop filter for reference pictures only - may produce artifacts */
} hevc_loop_filter_mode_t;

/** \brief SMP mode */
typedef enum hevc_smp_mode_e {
    HEVCVD_SMP_AUTO = -1,                       /**< \brief Let the decoder decide on one of the following */
    HEVCVD_SMP_OFF = 0,                         /**< \brief Use only thread called copybytes for decoding. Do not create more threads */
    HEVCVD_SMP_OVERLAPPED = 1,                  /**< \brief Decode as many pictures in parallel as possible and decode parallel within each picture */
    HEVCVD_SMP_CONCURRENT = 2                   /**< \brief Decode one picture at a time but decode parallel within each picture */
} hevc_smp_mode_t;

/** \brief HEVC decoding toolset */
typedef enum hevc_decoding_toolset_e
{
    HEVCVD_DECODING_TOOLSET_CPU = 0,            /**< \brief Decode on CPU */
    HEVCVD_DECODING_TOOLSET_D3D9 = 1,           /**< \brief Decode on GPU using DirectX 9Ex API */
    HEVCVD_DECODING_TOOLSET_D3D11 = 2           /**< \brief Decode on GPU using DirectX 11 API */
} hevc_decoding_toolset_t;

/** \enum hevc_mv_precision_t
 *  \brief Motion vector precision limits
 */
typedef enum hevc_mv_precision_e {
    HEVCVD_MV_PRECISION_QUARTER = 0,/**< \brief Use any interpolation as it is in stream. */
    HEVCVD_MV_PRECISION_HALF = 1,   /**< \brief Use halfpixel interpolation instead of quarterpixel interpolation. */
    HEVCVD_MV_PRECISION_FULL = 2    /**< \brief Use full pixel prediction for all prediction units, no interpolation. */
} hevc_mv_precision_t;

/** \brief Preview mode. */
typedef enum hevc_preview_mode_e {
    HEVCVD_PREVIEW_OFF = 0,       /**< \brief Decoding in \ref hevc_loop_filter_mode_t::HEVCVD_LF_STANDARD "standard mode". */
    HEVCVD_PREVIEW_LV1 = 1,       /**< \brief This mode disables de-blocking and SAO filtering on \ref hevc_loop_filter_mode_t::HEVCVD_LF_REFERENCE_ONLY "non-reference frames". */
    HEVCVD_PREVIEW_LV2 = 2,       /**< \brief This mode disables de-blocking and SAO filtering on \ref hevc_loop_filter_mode_t::HEVCVD_LF_OFF "all frames". */
    HEVCVD_PREVIEW_LV3 = 3,       /**< \brief This mode disables de-blocking and SAO filtering on \ref hevc_loop_filter_mode_t::HEVCVD_LF_OFF "all frames" and additionally restricts motion vector precision to \ref hevc_mv_precision_t::HEVCVD_MV_PRECISION_HALF "half pel". */
    HEVCVD_PREVIEW_LV4 = 4,       /**< \brief This mode disables de-blocking and SAO filtering on \ref hevc_loop_filter_mode_t::HEVCVD_LF_OFF "all frames" and additionally restricts motion vector precision to \ref hevc_mv_precision_t::HEVCVD_MV_PRECISION_FULL "full pel". */
} hevc_preview_mode_t;

/** \brief Deinterlacing mode */
typedef enum hevc_deinterlacing_mode_e {
    HEVCVD_DEINTERLACING_NONE = 0,                            //!< Do not combine fields, output them separately as independent frames (default).
    HEVCVD_DEINTERLACING_INTERFIELD_INTERPOLATION = 1,        //!< Deinterlacing is done by blending or averaging consecutive fields. This is fine for slow motion, when the image hasn't changed between fields.
    HEVCVD_DEINTERLACING_INTRAFIELD_INTERPOLATION_TOP = 2,    //!< Deinterlacing is done by replacement of samples of bottom field with samples of top field (top field stretching).
    HEVCVD_DEINTERLACING_INTRAFIELD_INTERPOLATION_BOTTOM = 3, //!< Deinterlacing is done by replacement of samples of top field with samples of bottom field (bottom field stretching).
    HEVCVD_DEINTERLACING_WEAVE = 4,                           //!< Combine fields to frames, do not apply deinterlacing.
} hevc_deinterlacing_mode_t;

/*! \brief GET_PIC mode. Can be used with \ref GET_PIC, \ref GET_PIC_PARAMSP, \ref GET_PIC_PARAMSPEX, \ref GET_SEQ_PARAMSP, \ref GET_SEQ_PARAMSPEX and other \ref bufstream::auxinfo "auxinfo" API calls.
 *
 * GET_PIC mode can be passed as the offs argument (second argument) to the \ref bufstream::auxinfo "auxinfo" function. It makes sense to use non-default values of the GET_PIC mode only on interlaced content and only when deinterlacing mode is enabled, refer to \ref hevc_deinterlacing_mode_t for more details.
 * GET_PIC mode can be used with the following \ref bufstream::auxinfo "auxinfo" API calls: \ref GET_PIC, \ref GET_PIC_PARAMSP, \ref GET_PIC_PARAMSPEX, \ref GET_SEQ_PARAMSP, \ref GET_SEQ_PARAMSPEX, \ref GET_USER_DATAP, \ref GET_LAST_PTS, \ref GET_SEI, \ref GET_HEVC_SEQ_PAR_SET, \ref GET_HEVC_PIC_PAR_SET, \ref GET_HEVC_VID_PAR_SET.
 * If deinterlacing mode is enabled GET_PIC mode can be used for accessing separate fields (access units) and information which is associated with them like user data, timestamps, SEI messages and so on.
 * For interlaced content with enabled deinterlacing \ref hevc_get_pic_t::HEVCVD_GET_PIC_FRAME will return information associated with the top field for all calls except \ref GET_PIC.
 *
 * Deinterlacing mode                 | GET_PIC mode                                                      | Description                                                                                                                                                                      |
 * ---------------------------------- | ----------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
 * \ref HEVCVD_DEINTERLACING_NONE     | \ref HEVCVD_GET_PIC_FRAME                                         | The HEVC decoder does nothing specific about interlaced content. All frames are processed independently regardless of interlacing presence.                                      |
 * \ref HEVCVD_DEINTERLACING_NONE     | \ref HEVCVD_GET_PIC_TOP_FIELD or \ref HEVCVD_GET_PIC_BOTTOM_FIELD | Not a valid combination. Decoder will return \ref BS_ERROR code.                                                                                                                 |
 * Not \ref HEVCVD_DEINTERLACING_NONE | \ref HEVCVD_GET_PIC_FRAME                                         | \ref GET_PIC always returns full frames either for progressive or interlaced content. Other calls return information associated with top field (or frame for progressive video). |
 * Not \ref HEVCVD_DEINTERLACING_NONE | \ref HEVCVD_GET_PIC_TOP_FIELD                                     | Can only be used with interlaced content. All calls return information associated with the top field.                                                                            |
 * Not \ref HEVCVD_DEINTERLACING_NONE | \ref HEVCVD_GET_PIC_BOTTOM_FIELD                                  | Can only be used with interlaced content. All calls return information associated with the bottom field.                                                                         |
 */
typedef enum hevc_get_pic_e {
    HEVCVD_GET_PIC_FRAME = 0,        //!< Get full frame; depending on chosen \ref hevc_deinterlacing_mode_t "deinterlacing mode" combining of fields for interlaced video can be performed (default).
    HEVCVD_GET_PIC_TOP_FIELD = 1,    //!< Get top field of the frame. Can be used only with interlaced content and deinterlacing enabled.
    HEVCVD_GET_PIC_BOTTOM_FIELD = 2, //!< Get bottom field of the frame. Can be used only with interlaced content and deinterlacing enabled.
} hevc_get_pic_t;

/** \brief Runtime errors */
typedef enum hevc_runtime_error_e {
    NO_RUNTIME_ERROR = 0,             /**< \brief No error occurred */
    OUT_OF_MEMORY_ERROR = 1,          /**< \brief Decoder faced with out of memory condition */
    HARDWARE_ACCELERATION_ERROR = 2,  /**< \brief Hardware accelerator reported error */
    DECODER_CREATION_ERROR = 3,       /**< \brief The decoder creation failed */
    SLICE_INIT_ERROR = 4,             /**< \brief Slice contains error and cannot be decoded */
    REF_LISTS_UPDATE_ERROR = 5,       /**< \brief Some pictures are missing in reference picture set */
    REF_PICTURE_ERROR = 6,            /**< \brief Error in one or more reference pictures */
    PARSE_ERROR = 7,                  /**< \brief Binary stream contains errors */
    UNSUPPORTED_FEATURE = 8           /**< \brief Stream contains at least one usupported feature */
} hevc_runtime_error_t;

/** \enum slice_type_t
 *  \brief Slice types
 */
typedef enum slice_type_e {
    SLICE_TYPE_B = 0, /**< \brief Slice has B type */
    SLICE_TYPE_P,     /**< \brief Slice has P type */
    SLICE_TYPE_I      /**< \brief Slice has I type */
} slice_type_t;

/** \enum nalu_types_e
 *  \brief NAL unit types enumeration.
 */
typedef enum
{
    NALU_TYPE_SLICE_TRAIL_N = 0,    /**< \brief Coded slice segment of a non-TSA, non-STSA trailing picture non-referenced */
    NALU_TYPE_SLICE_TRAIL_R,        /**< \brief Coded slice segment of a non-TSA, non-STSA trailing picture referenced */

    NALU_TYPE_SLICE_TSA_N,          /**< \brief Coded slice segment of a TSA picture */
    NALU_TYPE_SLICE_TSA_R,          /**< \brief Coded slice segment of a TSA picture */

    NALU_TYPE_SLICE_STSA_N,         /**< \brief Coded slice segment of an STSA picture */
    NALU_TYPE_SLICE_STSA_R,         /**< \brief Coded slice segment of an STSA picture */

    NALU_TYPE_SLICE_RADL_N,         /**< \brief Coded slice segment of a RADL picture */
    NALU_TYPE_SLICE_RADL_R,         /**< \brief Coded slice segment of a RADL picture */

    NALU_TYPE_SLICE_RASL_N,         /**< \brief Coded slice segment of a RASL picture */
    NALU_TYPE_SLICE_RASL_R,         /**< \brief Coded slice segment of a RASL picture */

    NALU_TYPE_RESERVED_10,          /**< \brief Reserved non-IRAP sub-layer non-reference VCL NAL unit types */
    NALU_TYPE_RESERVED_11,          /**< \brief Reserved non-IRAP sub-layer reference VCL NAL unit types */
    NALU_TYPE_RESERVED_12,          /**< \brief Reserved non-IRAP sub-layer non-reference VCL NAL unit types */
    NALU_TYPE_RESERVED_13,          /**< \brief Reserved non-IRAP sub-layer reference VCL NAL unit types */
    NALU_TYPE_RESERVED_14,          /**< \brief Reserved non-IRAP sub-layer non-reference VCL NAL unit types */
    NALU_TYPE_RESERVED_15,          /**< \brief Reserved non-IRAP sub-layer reference VCL NAL unit types */

    NALU_TYPE_SLICE_BLA,            /**< \brief Coded slice segment of a BLA picture */
    NALU_TYPE_SLICE_BLANT,          /**< \brief Coded slice segment of a BLA picture */
    NALU_TYPE_SLICE_BLA_N_LP,       /**< \brief Coded slice segment of a BLA picture */
    NALU_TYPE_SLICE_IDR,            /**< \brief Coded slice segment of an IDR picture */
    NALU_TYPE_SLICE_IDR_N_LP,       /**< \brief Coded slice segment of an IDR picture */
    NALU_TYPE_SLICE_CRA,            /**< \brief Coded slice segment of a CRA picture */
    NALU_TYPE_RESERVED_22,          /**< \brief Reserved IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_23,          /**< \brief Reserved IRAP VCL NAL unit types */

    NALU_TYPE_RESERVED_24,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_25,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_26,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_27,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_28,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_29,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_30,          /**< \brief Reserved non-IRAP VCL NAL unit types */
    NALU_TYPE_RESERVED_31,          /**< \brief Reserved non-IRAP VCL NAL unit types */

    NALU_TYPE_VPS,                  /**< \brief Video parameter set */
    NALU_TYPE_SPS,                  /**< \brief Sequence parameter set */
    NALU_TYPE_PPS,                  /**< \brief Picture parameter set */
    NALU_TYPE_ACCESS_UNIT_DELIMITER,/**< \brief Access unit delimiter */
    NALU_TYPE_EOS,                  /**< \brief End of sequence */
    NALU_TYPE_EOB,                  /**< \brief End of bitstream */
    NALU_TYPE_FILLER_DATA,          /**< \brief Filler data */
    NALU_TYPE_SEI,                  /**< \brief Supplemental enhancement information */
    NALU_TYPE_SEI_SUFFIX,           /**< \brief Supplemental enhancement information */
    NALU_TYPE_RESERVED_41,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_42,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_43,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_44,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_45,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_46,          /**< \brief Reserved */
    NALU_TYPE_RESERVED_47,          /**< \brief Reserved */
    NALU_TYPE_UNSPECIFIED_48,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_49,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_50,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_51,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_52,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_53,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_54,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_55,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_56,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_57,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_58,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_59,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_60,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_61,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_62,       /**< \brief Unspecified */
    NALU_TYPE_UNSPECIFIED_63,       /**< \brief Unspecified */
    NALU_TYPE_INVALID
} nalu_types_e;

/** \brief SEI prefix types */
typedef enum
{
    SEI_TYPE_BUFFERING_PERIOD = 0,                              /**< \brief buffering_period(payloadSize) */
    SEI_TYPE_PIC_TIMING = 1,                                    /**< \brief pic_timing(payloadSize) */
    SEI_TYPE_PAN_SCAN_RECT = 2,                                 /**< \brief pan_scan_rect(payloadSize) */
    SEI_TYPE_FILLER_PAYLOAD = 3,                                /**< \brief filler_payload(payloadSize) */
    SEI_TYPE_USER_DATA_REGISTERED_ITU_T_T35 = 4,                /**< \brief user_data_registered_itu_t_t35(payloadSize) */
    SEI_TYPE_USER_DATA_UNREGISTERED = 5,                        /**< \brief user_data_unregistered(payloadSize) */
    SEI_TYPE_RECOVERY_POINT = 6,                                /**< \brief recovery_point(payloadSize) */
    SEI_TYPE_SCENE_INFO = 9,                                    /**< \brief scene_info(payloadSize) */
    SEI_TYPE_PICTURE_SNAPSHOT = 15,                             /**< \brief picture_snapshot(payloadSize) */
    SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_START = 16,         /**< \brief progressive_refinement_segment_start(payloadSize) */
    SEI_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END = 17,           /**< \brief progressive_refinement_segment_end(payloadSize) */
    SEI_TYPE_FILM_GRAIN_CHARACTERISTICS = 19,                   /**< \brief film_grain_characteristics(payloadSize) */
    SEI_TYPE_POST_FILTER_HINT = 22,                             /**< \brief post_filter_hint(payloadSize) */
    SEI_TYPE_TONE_MAPPING_INFO = 23,                            /**< \brief tone_mapping_info(payloadSize) */
    SEI_TYPE_FRAME_PACKING_ARRANGEMENT = 45,                    /**< \brief frame_packing_arrangement(payloadSize) */
    SEI_TYPE_DISPLAY_ORIENTATION = 47,                          /**< \brief display_orientation(payloadSize) */
    SEI_TYPE_STRUCTURE_OF_PICTURES_INFO = 128,                  /**< \brief structure_of_pictures_info(payloadSize) */
    SEI_TYPE_ACTIVE_PARAMETER_SETS = 129,                       /**< \brief active_parameter_sets(payloadSize) */
    SEI_TYPE_DECODING_UNIT_INFO = 130,                          /**< \brief decoding_unit_info(payloadSize) */
    SEI_TYPE_TEMPORAL_SUB_LAYER_ZERO_INDEX = 131,               /**< \brief temporal_sub_layer_zero_index(payloadSize) */
    SEI_TYPE_DECODED_PICTURE_HASH = 132,                        /**< \brief decoded_picture_hash(payloadSize) */
    SEI_TYPE_SCALABLE_NESTING = 133,                            /**< \brief scalable_nesting(payloadSize) */
    SEI_TYPE_REGION_REFRESH_INFO = 134,                         /**< \brief region_refresh_info(payloadSize) */
    SEI_TYPE_NO_DISPLAY = 135,                                  /**< \brief no_display(payloadSize) */
    SEI_TYPE_TIME_CODE = 136,                                   /**< \brief time_code(payloadSize) */
    SEI_TYPE_MASTER_DISPLAY_COLOUR_VOLUME = 137,                /**< \brief mastering_display_colour_volume(payloadSize) */
    SEI_TYPE_SEGMENTED_RECT_FRAME_PACKING_ARRANGEMENT = 138,    /**< \brief segmented_rect_frame_packing_arrangement(payloadSize) */
    SEI_TYPE_TEMPORAL_MOTION_CONSTRAINED_TILE_SETS = 139,       /**< \brief temporal_motion_constrained_tile_sets(payloadSize) */
    SEI_TYPE_CHROMA_RESAMPLING_FILTER_HING = 140,               /**< \brief chroma_resampling_filter_hint(payloadSize) */
    SEI_TYPE_KNEE_FUNCTION_INFO = 141,                          /**< \brief knee_function_info(payloadSize) */
    SEI_TYPE_COLOUR_REMAPPING_INFO = 142,                       /**< \brief colour_remapping_info(payloadSize) */
    SEI_TYPE_DEINTERLACED_FIELD_INDENTIFICATION = 143,          /**< \brief deinterlaced_field_identification(payloadSize) */
    SEI_TYPE_CONTENT_LIGHT_LEVEL_INFO = 144,                    /**< \brief content_light_level_info(payloadSize) */
    SEI_ALTERNATIVE_TRANSFER_CHARACTERISTICS = 147,             /**< \brief alternative_transfer_characteristics(payloadSize) */
    SEI_TYPE_LAYERS_NOT_PRESENT = 160,                          /**< \brief layers_not_present(payloadSize) - specified in Annex F */
    SEI_TYPE_INTER_LAYER_CONSTRAINED_TILE_SETS = 161,           /**< \brief inter_layer_constrained_tile_sets(payloadSize) - specified in Annex F */
    SEI_TYPE_BSP_NESTING = 162,                                 /**< \brief bsp_nesting(payloadSize) - specified in Annex F */
    SEI_TYPE_BSP_INITIAL_ARRIVAL_TIME = 163,                    /**< \brief bsp_initial_arrival_time(payloadSize) - specified in Annex F */
    SEI_TYPE_SUB_BITSTREAM_PROPERTY = 164,                      /**< \brief sub_bitstream_property(payloadSize) - specified in Annex F */
    SEI_TYPE_ALPHA_CHANNEL_INFO = 165,                          /**< \brief alpha_channel_info(payloadSize) - specified in Annex F */
    SEI_TYPE_OVERLAY_INFO = 166,                                /**< \brief overlay_info(payloadSize) - specified in Annex F */
    SEI_TYPE_TEMPORAL_MV_PREDICTION_CONSTRAINTS = 167,          /**< \brief temporal_mv_prediction_constraints(payloadSize) - specified in Annex F */
    SEI_TYPE_FRAME_FIELD_INFO = 168,                            /**< \brief frame_field_info(payloadSize) - specified in Annex F */
    SEI_TYPE_THREE_DIMENSIONAL_REFERENCE_DISPLAYS_INFO = 176,   /**< \brief three_dimensional_reference_displays_info(payloadSize) - specified in Annex G */
    SEI_TYPE_DEPTH_REPRESENTATION_INFO = 177,                   /**< \brief depth_representation_info(payloadSize) - specified in Annex G */
    SEI_TYPE_MULTIVIEW_SCENE_INFO = 178,                        /**< \brief multiview_scene_info(payloadSize) - specified in Annex G */
    SEI_TYPE_MULTIVIEW_ACQUISITION_INFO = 179,                  /**< \brief multiview_acquisition_info(payloadSize) - specified in Annex G */
    SEI_TYPE_MULTIVIEW_VIEW_POSITION = 180,                     /**< \brief multiview_view_position(payloadSize) - specified in Annex G */
    SEI_TYPE_PICTURE_DIGEST = 256                               /**< \brief picture_digest(payloadSize) */
} sei_types_e;

/** \brief SEI suffix types */
typedef enum
{
    SEI_SUFFIX_TYPE_FILLER_PAYLOAD = 3,                         /**< \brief filler_payload(payloadSize) */
    SEI_SUFFIX_TYPE_USER_DATA_REGISTERED_ITU_T_T35 = 4,         /**< \brief user_data_registered_itu_t_t35(payloadSize) */
    SEI_SUFFIX_TYPE_USER_DATA_UNREGISTERED = 5,                 /**< \brief user_data_unregistered(payloadSize) */
    SEI_SUFFIX_TYPE_PROGRESSIVE_REFINEMENT_SEGMENT_END = 17,    /**< \brief progressive_refinement_segment_end(payloadSize) */
    SEI_SUFFIX_TYPE_POST_FILTER_HINT = 22,                      /**< \brief post_filter_hint(payloadSize) */
    SEI_SUFFIX_TYPE_DECODED_PICTURE_HASH = 132                  /**< \brief decoded_picture_hash(payloadSize) */
} sei_suffix_types_e;

typedef enum
{
    RANGE_PRESERVE = 0,
    RANGE_SRC_TO_SHORT = 1,
    RANGE_SRC_TO_FULL = 2,
    RANGE_FULL_TO_FULL = 3,
    RANGE_FULL_TO_SHORT = 4,
    RANGE_SHORT_TO_SHORT = 5,
    RANGE_SHORT_TO_FULL = 6
} range_conversion_e;

/** \brief User data packet */
typedef struct hevc_user_data_packet_s
{
    uint64_t offset;            /**< \brief Payload data offset */
    uint8_t *data;              /**< \brief Data */
    uint32_t payload_size;      /**< \brief Payload data size */
} hevc_user_data_packet_t;

/** \brief User data */
typedef struct hevc_user_data_s
{
    hevc_user_data_packet_t *user_data_packet;  /**< \brief Vector of user data packets*/
    uint32_t num_user_data_packets;             /**< \brief Number of user data packets*/
} hevc_user_data_t;

/**
 * @name NAL unit header
 * @{
 **/
 /**
 *@brief NALU header structure.
 */
typedef struct hevc_nalu_s
{
    uint64_t offset;            /**< \brief Start offset of the NAL unit. Points to the beginning of the start code or to the zero_byte (if exists). */
    uint64_t size;              /**< \brief Size of the NAL unit including zero_byte, start code and trailing bytes. */
    uint8_t nal_unit_type;      /**< \brief type of RBSP data structure contained in the NAL unit */
    uint8_t temporal_layer_id;  /**< \brief temporal identifier for the NAL unit */
    uint32_t reserved[4];       /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t reserved_ptr[4];  /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_nalu_t;
/** @} */

/**
 * @name Access unit info
 * @{
 **/
 /**
 *@brief Contains information about the structure of the access unit
 */
typedef struct hevc_au_s
{
    uint64_t offset;            /**< \brief Start offset of the access unit. Points to the beginning of the start code or to the zero_byte (if exists). */
    uint64_t size;              /**< \brief Size of the access unit including zero_byte, start code and trailing bytes. */
    uint64_t* nal_unit_sizes;   /**< \brief An array containing sizes of all NAL units within access unit. */
    uint16_t num_nal_units;     /**< \brief Count of NAL units within the access unit. */
    uint32_t reserved[4];       /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t reserved_ptr[4];  /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_au_t;
/** @} */

/** \enum profile_idc_e
 *  \brief Profile idc types
 */
typedef enum
{
    PROFILE_IDC_NONE = 0,                                       /**< \brief */
    PROFILE_IDC_MAIN = 1,                                       /**< \brief Main profile */
    PROFILE_IDC_MAIN_10 = 2,                                    /**< \brief Main 10 profile */
    PROFILE_IDC_MAIN_STILL_PICTURE = 3,                         /**< \brief Main Still Picture profiles */
    PROFILE_IDC_MAIN_REXT = 4,                                  /**< \brief Main Range Extensions profiles */
    PROFILE_IDC_HIGH_THROUGHPUT_REXT = 5,                       /**< \brief Main Range Extensions High Throughput profiles */
    PROFILE_IDC_MULTIVIEW_MAIN = 6,                             /**< \brief Multiview Main profile */
    PROFILE_IDC_SCALABLE_MAIN = 7                               /**< \brief Scalable Main profile */
} profile_idc_e;

/**
 * @name Profile, tier, level
 * @{
 **/
 /**
 *@brief Profiles, tiers and levels specify restrictions on bitstreams and hence limits on the capabilities needed to decode the bitstreams.
 */
typedef struct hevc_ptl_s
{
    uint8_t         profile_space;                  /**< \brief Specifies the context for the interpretation of profile_id and profile_combatibility_flags */
    uint8_t         tier_flag;                      /**< \brief Specifies the tier context for the interpretation of level_id */
    uint8_t         profile_id;                     /**< \brief Indicates a profile to which the CVS conforms */
    uint8_t         level_id;                       /**< \brief Indicates a level to which the CVS conforms */
    uint32_t        profile_compatibility_flags;    /**< \brief Indicates that the CVS conforms to the profile indicated by profile_id */
    uint8_t         progressive_source_flag;        /**< \brief Specifies source scan type of each picture in the CVS */
    uint8_t         interlaced_source_flag;         /**< \brief Specifies source scan type of each picture in the CVS */
    uint8_t         non_packed_constraint_flag;     /**< \brief Specifies that there are no frame packing arrangement SEI messages present in the CVS */
    uint8_t         frame_only_constraint_flag;     /**< \brief Specifies that field_seq_flag is equal to 0 */
    uint8_t         inbld_flag;                     /**< \brief Specifies that INBLD capability is required for decoding the layer */
} hevc_ptl_t;
/** @} */

/**
 * @name Profiles, tiers, levels
 * @{
 **/
 /**
 *@brief Container for general and sub-layers ptl.
 */
typedef struct hevc_profile_tier_s
{
    hevc_ptl_t      general_ptl;                    /**< \brief general profile tier */
    hevc_ptl_t      sublayer_ptl[6];                /**< \brief Sub-layer profile tiers Maximum value of max_sub_layers_minus1 is 6 */
    uint8_t         sublayer_profile_flags[6];      /**< \brief Specifies that profile information is present in the profile tier */
    uint8_t         sublayer_level_flags[6];        /**< \brief Specifies that level information is present in the profile tier */
} hevc_profile_tier_t;
/** @} */

/**
 * @name VPS
 * @{
 **/
 /**
 *@brief Parameter set describes the overall characteristics of coded video sequences, including the dependences between temporal sub-layers.
 */
typedef struct hevc_vid_par_set_s
{
    uint8_t         vps_id;                                                             /**< \brief Video parameter set identifier */
    uint8_t         temporal_id_nesting_flag;                                           /**< \brief Specifies whether inter prediction is additionally restricted for CVSs */
    uint8_t         reserved_three_2bits;                                               /**< \brief Reserved bits */
    uint8_t         reserved_zero_6bits;                                                /**< \brief Reserved bits */
    uint8_t         vps_max_sub_layers;                                                 /**< \brief Maximum number of temporal sub-layers that may be present in the CVS */
    hevc_profile_tier_t  profile_tier;                                                  /**< \brief Profile tier structure */
    uint8_t         vps_sub_layer_ordering_info_present_flag;                           /**< \brief Specifies using independent max_dec_pic_buffering, num_reorder_pics and max_latency_increase */
    uint8_t         vps_max_num_reorder_pics[TEMPORAL_LAYERS_COUNT];                    /**< \brief Maximum allowed number of pictures that can precede any picture in the CVS */
    uint8_t         vps_max_dec_pic_buffering[TEMPORAL_LAYERS_COUNT];                   /**< \brief Specifies the maximum required size of the decoded picture buffer */
    uint32_t        vps_max_latency_increase[TEMPORAL_LAYERS_COUNT];                    /**< \brief Specifies the maximum number of pictures that can precede any picture in the CVS */

    uint8_t         num_hrd_parameters;                                                 /**< \brief Specifies the number of hrd_parameters( ) syntax structures present in the VPS RBSP */
    uint8_t         vps_max_layer_id;                                                   /**< \brief Specifies the maximum allowed value of nuh_layer_id of all NAL units in each CVS referring to the VPS. */
    uint8_t         vps_max_op_sets;                                                    /**< \brief */
    uint8_t         oplayer_id_included_flag[VPS_LAYER_SETS_COUNT][NUH_LAYERS_COUNT];   /**< \brief oplayer_id_included_flag[ i ][ j ] Specifies that the value of nuh_layer_id equal to j is included in the layer identifier list layerSetLayerIdList[ i ] */

    uint8_t         timing_info_present_flag;                                           /**< \brief Specifies that vps_num_units_in_tick, vps_time_scale, vps_poc_proportional_to_timing_flag, and vps_num_hrd_parameters are present in the VPS */
    uint32_t        num_units_in_tick;                                                  /**< \brief The number of time units of a clock operating at the frequency vps_time_scale Hz that corresponds to one increment (called a clock tick) of a clock tick counter */
    uint32_t        time_scale;                                                         /**< \brief The number of time units that pass in one second\hideinitializer */
    uint8_t         poc_proportional_to_timing_flag;                                    /**< \brief Indicates that the picture order count value for each picture in the CVS that is not the first picture in the CVS, in decoding order, is proportional to the output time of the picture relative to the output time of the first picture in the CVS */
    uint8_t         num_ticks_poc_diff_one_minus1;                                      /**< \brief Specifies the number of clock ticks corresponding to a difference of picture order count values equal to 1 */
    uint8_t         hrd_op_set_idx[VPS_LAYER_SETS_COUNT];                               /**< \brief */
    uint8_t         cprms_present_flag[VPS_LAYER_SETS_COUNT];                           /**< \brief */

    uint8_t         extension_flag;                                                     /**< \brief */
} hevc_vid_par_set_t;
/** @} */

/**
 * @name Short term reference picture set
 * @{
 **/
 /**
 *@brief These parameters are used in SPS and slice header.
 */
typedef struct hevc_short_term_ref_pic_set_s
{
    int8_t inter_ref_pic_set_prediction_flag;               /**< \brief Specifies that the candidate short-term RPS is predicted from another candidate short-term RPS */
    int8_t delta_idx;                                       /**< \brief Specifies the difference between the value of rps_idx and the index, into the list of the candidate short-term RPSs specified in the SPS */
    int8_t delta_rps_sign;                                  /**< \brief With abs_delta_rps specify the value of the variable deltaRps */
    int8_t abs_delta_rps;                                   /**< \brief With delta_rps_sign specify the value of the variable deltaRps */

    int32_t delta_poc[16+1];                                /**< \brief Specifies the difference between the picture order count values of the current picture and i-th entry in the stRpsIdx-th candidate short-term RPS that has picture order count */
    int8_t used[16+1];                                      /**< \brief Specifies that the j-th entry in the source candidate short-term RPS is used for reference by the current picture */
    int8_t msb_present[16+1];                               /**< \brief */
    int8_t ref_id[16+1];                                    /**< \brief */

    int8_t num_ref_id;                                      /**< \brief */
    int8_t num_pics;                                        /**< \brief Specifies the number of entries in the stRpsIdx-th candidate short-term RPS */
    int8_t num_negative_pics;                               /**< \brief Specifies the number of entries in the stRpsIdx-th candidate short-term RPS that have picture order count values less than the picture order count value of the current picture */
    int8_t num_positive_pics;                               /**< \brief Specifies the number of entries in the stRpsIdx-th candidate short-term RPS that have picture order count values greater than the picture order count value of the current picture */
    int8_t ref_num_pics;                                    /**< \brief Required by DXVA (num_pics from reference short-term RPS) */
} hevc_short_term_ref_pic_set_t;
/** @} */

/**
 * @name Video usability information
 * @{
 **/
 /**
 *@brief  Some VUI parameters are required to check bitstream conformance and for output timing decoder conformance.
 */
typedef struct hevc_vui_s
{
    uint8_t         aspect_ratio_info_present;              /**< \brief Specifies that aspect_ratio_idc is present */
    uint8_t         aspect_ratio_id;                        /**< \brief Specifies the value of the sample aspect ratio of the luma samples */
    uint16_t        sar_width;                              /**< \brief Indicates the horizontal size of the sample aspect ratio */
    uint16_t        sar_height;                             /**< \brief Indicates the vertical size of the sample aspect ratio */
    uint8_t         overscan_info_present;                  /**< \brief Specifies that the overscan_appropriate_flag is present */
    uint8_t         overscan_appropriate_flag;              /**< \brief Indicates that the cropped decoded pictures output are suitable for display using overscan */
    uint8_t         video_signal_type_present;              /**< \brief Specifies that video_format, video_full_range_flag and colour_description_present_flag are present */
    uint8_t         video_format;                           /**< \brief Indicates the representation of the pictures */
    uint8_t         video_full_range_flag;                  /**< \brief Indicates the black level and range of the luma and chroma signals */
    uint8_t         colour_description_present;             /**< \brief Specifies that colour_primaries, transfer_characteristics and matrix_coefficients are present */
    uint8_t         colour_primaries;                       /**< \brief Indicates the chromaticity coordinates of the source primaries */
    uint8_t         transfer_characteristics;               /**< \brief Indicates the opto-electronic transfer characteristic of the source picture */
    uint8_t         matrix_coefficients;                    /**< \brief Describes the matrix coefficients used in deriving luma and chroma signals from the green, blue, and red primaries */
    uint8_t         chroma_loc_info_present;                /**< \brief Specifies that chroma_sample_loc_type_top_field and chroma_sample_loc_type_bottom_field are present */
    uint16_t        chroma_sample_loc_type_top_field;       /**< \brief Specify the location of chroma samples */
    uint16_t        chroma_sample_loc_type_bottom_field;    /**< \brief Specify the location of chroma samples */
    uint8_t         neutral_chroma_indication_flag;         /**< \brief Indicates that the value of all decoded chroma samples is equal to 1 << ( bit_depth_chroma − 1 ) */
    uint8_t         field_seq_flag;                         /**< \brief Indicates that the CVS conveys pictures that represent fields, and specifies that a picture timing SEI message shall be present in every access unit of the current CVS */
    uint8_t         frame_field_info_present_flag;          /**< \brief Indicates that pic_struct and field coding related values are present in picture timing SEI messages */
    uint8_t         default_display_window_flag;            /**< \brief Indicates that the default display window parameters follow next in the VUI */
    uint8_t         def_disp_win_left_offset;               /**< \brief */
    uint8_t         def_disp_win_right_offset;              /**< \brief */
    uint8_t         def_disp_win_top_offset;                /**< \brief */
    uint8_t         def_disp_win_bottom_offset;             /**< \brief */
    uint8_t         pic_struct_present_flag;                /**< \brief */
    uint8_t         hrd_parameters_present;                 /**< \brief Specifies that the syntax structure HRD parameters is present in the VUI parameters */
    uint8_t         timing_info_present;                    /**< \brief Specifies that num_units_in_tick, time_scale, poc_proportional_to_timing_flag, and hrd_parameters_present_flag are present in the VUI parameters */
    uint32_t        num_units_in_tick;                      /**< \brief Is the number of time units of a clock operating at the frequency vui_time_scale Hz that corresponds to one increment (called a clock tick) of a clock tick counter. */
    uint32_t        time_scale;                             /**< \brief Is the number of time units that pass in one second */
    uint8_t         nal_hrd_parameters_present_flag;        /**< \brief Specifies that NAL HRD parameters (pertaining to Type II bitstream conformance) are present in the HRD parameters */
    uint8_t         vcl_hrd_parameters_present_flag;        /**< \brief Specifies that VCL HRD parameters (pertaining to all bitstream conformance) are present in the HRD parameters */
    uint8_t         sub_pic_hrd_params_present_flag;        /**< \brief Specifies that sub-picture level HRD parameters are present */
    uint8_t         sub_pic_cpb_params_in_pic_timing_sei_flag;/**< */
    uint8_t         dpb_output_delay_du_length;             /**< \brief */

    uint16_t        tick_divisor;                           /**< \brief Is used to specify the clock sub-tick */
    uint8_t         du_cpb_removal_delay_increment_length;  /**< \brief Specifies the length, in bits, of the du_cpb_removal_delay_increment[ i ] and du_common_cpb_removal_delay_increment syntax elements of the picture timing SEI message and the du_spt_cpb_removal_delay_increment syntax element in the decoding unit information SEI message */
    uint8_t         bit_rate_scale;                         /**< \brief Specifies the maximum input bit rate of theCPB */
    uint8_t         cpb_size_scale;                         /**< \brief Specifies the CPB size of the CPB when the CPB operates at the access unit level */
    uint8_t         cpb_size_du_scale;                      /**< \brief Specifies the CPB size of the CPB when the CPB operates at sub-picture level */
    uint8_t         initial_cpb_removal_delay_length;       /**< \brief Specifies the length, in bits, of the nal_initial_cpb_removal_delay[ i ], nal_initial_cpb_removal_offset[ i ], vcl_initial_cpb_removal_delay[ i ], and vcl_initial_cpb_removal_offset[ i ] syntax elements of the buffering period SEI message */
    uint8_t         au_cpb_removal_delay_length;            /**< \brief Specifies the length, in bits, of the cpb_delay_offset syntax element in the buffering period SEI message and the cpb_removal_delay syntax element in the picture timing SEI message */
    uint8_t         dpb_output_delay_length;                /**< \brief Specifies the length, in bits, of the dpb_delay_offset syntax element in the buffering period SEI message and the pic_dpb_output_delay syntax element in the picture timing SEI message */

    uint8_t         fixed_pic_rate_general_flag[8];         /**< \brief */
    uint8_t         fixed_pic_rate_within_cvs_flag[8];      /**< \brief */
    uint16_t        elemental_duration_in_tc[8];            /**< \brief Specifies the temporal distance, in clock ticks, between the elemental units that specify the HRD output times of consecutive pictures in output order */
    uint8_t         low_delay_hrd_flag[8];                  /**< \brief Specifies the HRD operational mode */
    uint8_t         cpb_cnt[8];                             /**< \brief Specifies the number of alternative CPB specifications in the bitstream of the CVS */
    uint32_t        bit_rate_value[8][8][2];                /**< \brief Specifies the maximum input bit rate for the CPB when the CPB operates at the access unit level */
    uint32_t        cpb_size_value[8][8][2];                /**< \brief Is used together with cpb_size_scale to specify the CPB size when the CPB operates at the access unit level */
    uint32_t        cpb_size_du_value[8][8][2];             /**< \brief Is used to specify the CPB size when the CPB operates at sub-picture level */
    uint32_t        bit_rate_du_value[8][8][2];             /**< \brief Is used to specify the maximum input bit rate sub-picture level */
    uint8_t         cbr_flag[8][8][2];                      /**< \brief Specifies that to decode this CVS by the HRD using CPB specification, the hypothetical stream scheduler (HSS) operates in an intermittent bit rate mode */

    uint8_t         poc_proportional_to_timing_flag;        /**< \brief Indicates that the picture order count value for each picture in theCVS that is not the first picture in the CVS, in decoding order, is proportional to the output time of the picture relative to the output time of the first picture in the CVS */
    uint16_t        num_ticks_poc_diff_one;                 /**< \brief Specifies the number of clock ticks corresponding to a difference of picture order count values equal to 1 */
    uint8_t         bitstream_restriction_flag;             /**< \brief Specifies that the bitstream restriction parameters for the CVS are present */
    uint8_t         tiles_fixed_structure_flag;             /**< \brief Indicates that each PPS that is active in the CVS has the same value of the syntax elements num_tile_columns, num_tile_rows, uniform_spacing_flag, column_width[ i ], row_height[ i ] and loop_filter_across_tiles_enabled_flag, when present */
    uint8_t         motion_vectors_over_pic_boundaries_flag;/**< \brief Equal to 0 indicates that no sample outside the picture boundaries and no sample at a fractional sample position for which the sample value is derived using one or more samples outside the picture boundaries is used for inter prediction of any sample */
    uint8_t         restricted_ref_pic_lists_flag;          /**< \brief Indicates that all P and B slices (when present) that belong to the same picture have an identical reference picture list 0, and that all B slices (when present) that belong to the same picture have an identical reference picture list 1 */
    uint8_t         min_spatial_segmentation_id;            /**< \brief Establishes a bound on the maximum possible size of distinct coded spatial segmentation regions in the pictures of the CVS */
    uint8_t         max_bytes_per_pic_denom;                /**< \brief Indicates a number of bytes not exceeded by the sum of the sizes of the VCL NAL units associated with any coded picture in the CVS */
    uint8_t         max_bits_per_min_cu_denom;              /**< \brief Indicates an upper bound for the number of coded bits of coding unit data for any coding block in any picture of the CVS */
    uint8_t         log2_max_mv_length_horizontal;          /**< \brief Indicate the maximum absolute value of a decoded horizontal motion vector component, in quarter luma sample units, for all pictures in the CVS */
    uint8_t         log2_max_mv_length_vertical;            /**< \brief Indicate the maximum absolute value of a decoded vertical motion vector component, in quarter luma sample units, for all pictures in the CVS */
} hevc_vui_t;
/** @} */

/**
 * @name Sequence parameter set
 * @{
 **/
 /**
 *@brief Parameter set, which remains unchanged throughout a coded video sequence.
 */
typedef struct hevc_seq_par_set_s
{
    uint8_t         vps_id;                                             /**< \brief Specifies the value of the vps_video_parameter_set_id of the active VPS */
    uint8_t         sps_max_sub_layers;                                 /**< \brief Specifies the maximum number of temporal sub-layers that may be present in each CVS referring to the SPS */
    uint8_t         temporal_id_nesting_flag;                           /**< \brief Specifies whether inter prediction is additionally restricted for CVSs referring to the SPS */
    hevc_profile_tier_t  profile_tier;                                  /**< \brief Profiles, tiers and levels container */
    uint8_t         sps_id;                                             /**< \brief Provides an identifier for the SPS for reference by other syntax elements */
    uint8_t         chroma_format_idc;                                  /**< \brief Specifies the chroma sampling relative to the luma sampling */
    uint8_t         separate_colour_plane_flag;                         /**< \brief Specifies that the three colour components of the 4:4:4 chroma format are coded separately. */
    uint16_t        pic_width_in_luma_samples;                          /**< \brief Specifies the width of each decoded picture in units of luma samples */
    uint16_t        pic_height_in_luma_samples;                         /**< \brief Specifies the height of each decoded picture in units of luma samples */
    uint8_t         conformance_window_flag;                            /**< \brief Indicates that the conformance cropping window offset parameters follow next in the SPS */
    uint16_t        conf_win_left_offset;                               /**< \brief Specify the samples of the pictures in the CVS that are output from the decoding process */
    uint16_t        conf_win_right_offset;                              /**< \brief Specify the samples of the pictures in the CVS that are output from the decoding process */
    uint16_t        conf_win_top_offset;                                /**< \brief Specify the samples of the pictures in the CVS that are output from the decoding process */
    uint16_t        conf_win_bottom_offset;                             /**< \brief Specify the samples of the pictures in the CVS that are output from the decoding process */
    uint8_t         bit_depth_luma;                                     /**< \brief Specifies the bit depth of the samples of the luma array */
    uint8_t         qp_bd_offset_Y;                                     /**< \brief Specifies the luma quantization parameter range offset */
    uint8_t         bit_depth_chroma;                                   /**< \brief Specifies the bit depth of the samples of the chroma arrays */
    uint8_t         qp_bd_offset_C;                                     /**< \brief Specifies the chroma quantization parameter range offset */
    uint8_t         log2_max_pic_order_cnt_lsb;                         /**< \brief Is used in the decoding process for picture order count */
    uint8_t         sps_sub_layer_ordering_info_present_flag;           /**< \brief Specifies that max_dec_pic_buffering[ i ], num_reorder_pics[ i ], and max_latency_increase[ i ] are present */
    uint8_t         sps_max_num_reorder_pics[TEMPORAL_LAYERS_COUNT];    /**< \brief indicates the maximum allowed number of pictures that can precede any picture in the CVS in decoding order and follow that picture in output order */
    uint8_t         sps_max_dec_pic_buffering[TEMPORAL_LAYERS_COUNT];   /**< \brief Specifies the maximum required size of the decoded picture buffer for the CVS in units of picture storage buffers */
    uint32_t        sps_max_latency_increase[TEMPORAL_LAYERS_COUNT];    /**< \brief Specifies the maximum number of pictures that can precede any picture in the CVS in output order and follow that picture in decoding order */

    uint8_t         log2_min_coding_block_size;                         /**< \brief Specifies the base 2 logarithm of the minimum size of a luma coding block */
    uint8_t         log2_diff_max_min_coding_block_size;                /**< \brief Specifies the base 2 logarithm of the difference between the maximum and minimum luma coding block size */

    uint8_t         log2_min_transform_block_size;                      /**< \brief Specifies the base 2 logarithm of the minimum transform block size */
    uint8_t         log2_diff_max_min_transform_block_size;             /**< \brief Specifies the base 2 logarithm of the difference between the maximum and minimum transform block size */
    uint8_t         max_transform_hierarchy_depth_inter;                /**< \brief Specifies the maximum hierarchy depth for transform units of coding units coded in inter prediction mode */
    uint8_t         max_transform_hierarchy_depth_intra;                /**< \brief Specifies the maximum hierarchy depth for transform blocks of coding blocks coded in intra prediction mode */

    uint8_t         scaling_list_enabled_flag;                          /**< \brief Specifies that a scaling list is used for the scaling process for transform coefficients */
    uint8_t         scaling_list_data_present_flag;                     /**< \brief Specifies that scaling list data are present in the SPS */
    uint8_t         scaling_list_coefs[4][6][64];                       /**< \brief Scaling list data */
    uint8_t         scaling_list_DC[4][6];                              /**< \brief Scaling list DC coefficients */

    uint8_t         amp_enabled_flag;                                   /**< \brief Specifies that asymmetric motion partitions may be used in coding tree blocks */
    uint8_t         sample_adaptive_offset_enabled_flag;                /**< \brief Specifies that the sample adaptive offset process is applied to the reconstructed picture after the deblocking filter process */
    uint8_t         pcm_enabled_flag;                                   /**< \brief Specifies that PCM data are not present in the CVS */
    uint8_t         pcm_bit_depth_luma;                                 /**< \brief Specifies the number of bits used to represent each of PCM sample values of the luma component */
    uint8_t         pcm_bit_depth_chroma;                               /**< \brief Specifies the number of bits used to represent each of PCM sample values of the chroma components */
    uint8_t         log2_min_pcm_coding_block_size;                     /**< \brief Specifies the minimum size of PCM coding blocks */
    uint8_t         log2_diff_max_min_pcm_coding_block_size;            /**< \brief Specifies the difference between the maximum and minimum size of PCM coding blocks */
    uint8_t         pcm_loop_filter_disable_flag;                       /**< \brief Specifies whether the loop filter process is disabled on reconstructed samples in a PCM coding unit */
    uint8_t         num_short_term_ref_pic_sets;                        /**< \brief Specifies the number of short_term_ref_pic_set syntax structures included in the SPS */
    hevc_short_term_ref_pic_set_t short_term_ref_pic_set[64];           /**< \brief short_term_ref_pic_set structure array, 64 instead of 65 as mentioned in the spec.  A slice-header-provided short term reference pic is stored in the slice header structure. */
    uint8_t         long_term_ref_pics_present_flag;                    /**< \brief Specifies that long-term reference picture is used for inter prediction of coded picture in the CVS */
    uint8_t         num_long_term_ref_pics;                             /**< \brief Specifies the number of candidate long-term reference pictures that are specified in the SPS */
    uint16_t        lt_ref_pic_poc_lsb[33];                             /**< \brief Specifies the picture order count modulo max_pic_order_cnt_lsb of the i-th candidate longterm reference picture specified in the SPS */
    uint8_t         used_by_curr_pic_lt_flag[33];                       /**< \brief Specifies that the i-th candidate long-term reference picture specified in the SPS is used for reference by a picture that includes in its long-term RPS the i-th candidate long-term reference picture specified in the SPS */
    uint8_t         sps_temporal_mvp_enabled_flag;                      /**< \brief Specifies that slice_temporal_mvp_enabled_flag is present in the slice headers of non-IDR pictures in the CVS */
    uint8_t         strong_intra_smoothing_enable_flag;                 /**< \brief Specifies that bi-linear interpolation is conditionally used on intra coding blocks 32x32 in the filtering process */
    uint8_t         vui_parameters_present_flag;                        /**< \brief Specifies that the VUI parameters is present */
    hevc_vui_t      vui;                                                /**< \brief VUI parameter set */

    uint8_t         sps_extension_flag;                                 /**< \brief Specifies that sps_range_extension_flag, sps_multilayer_extension_flag and sps_extension_6bits are present in the SPS RBSP syntax structure */
    uint8_t         sps_range_extension_flag;                           /**< \brief Specifies that sps_range_extension( ) syntax structure is present in the SPS RBSP syntax structure */
    uint8_t         sps_multilayer_extension_flag;                      /**< \brief sps_multilayer_extension( ) syntax structure is present in the SPS RBSP syntax structure */
    uint8_t         sps_extension_6bits;                                /**< \brief Specifies that sps_extension_data_flag syntax elements are present in the SPS RBSP syntax structure */
    uint8_t         sps_extension_data_flag;                            /**< \brief May have any value. Its presence and value do not affect decoder conformance to profiles specified in this version of this Specification */

    uint8_t         transform_skip_rotation_enabled_flag;               /**< \brief Specifies that a rotation is applied to the residual data block for intra 4x4 blocks coded using a transform skip operation */
    uint8_t         transform_skip_context_enabled_flag;                /**< \brief Specifies that a particular context is used for the parsing of the sig_coeff_flag for transform blocks with a skipped transform */
    uint8_t         implicit_rdpcm_enabled_flag;                        /**< \brief Specifies that the residual modification process for blocks using a transform bypass may be used for intra blocks in the CVS */
    uint8_t         explicit_rdpcm_enabled_flag;                        /**< \brief Specifies that the residual modification process for blocks using a transform bypass may be used for inter blocks in the CVS */
    uint8_t         extended_precision_processing_flag;                 /**< \brief Specifies that an extended dynamic range is used for coefficient parsing and inverse transform processing */
    uint8_t         intra_smoothing_disabled_flag;                      /**< \brief Specifies that the filtering process of neighbouring samples is unconditionally disabled for intra prediction */
    uint8_t         high_precision_offsets_enabled_flag;                /**< \brief Specifies that weighted prediction offset values are signalled using a bit-depth-dependent precision */
    uint8_t         persistent_rice_adaptation_enabled_flag;            /**< \brief Specifies that the Rice parameter derivation for the binarization of coeff_abs_level_remaining[ ] is initialized at the start of each sub-block using mode dependent statistics accumulated from previous sub-blocks */
    uint8_t         cabac_bypass_alignment_enabled_flag;                /**< \brief Specifies that a context-based adaptive binary arithmetic coding (CABAC) alignment process is used prior to bypass decoding of the syntax elements coeff_sign_flag[ ] and coeff_abs_level_remaining[ ] */

    uint8_t         inter_view_mv_vert_constraint_flag;                 /**< \brief Specifies that vertical component of motion vectors used for inter-layer prediction are constrained in the layers for which this SPS RBSP is the active SPS RBSP */
} hevc_seq_par_set_t;
/** @} */

/**
 * @name Picture parameter set
 * @{
 **/
 /**
 *@brief Parameter set, which remains unchanged within a coded picture.
 */
typedef struct hevc_pic_par_set_s
{
    uint8_t         pps_id;                                     /**< \brief Identifies the PPS for reference by other syntax elements */
    uint8_t         sps_id;                                     /**< \brief Specifies identifier for the active SPS */
    uint8_t         dependent_slice_segments_enabled_flag;      /**< \brief Specifies the presence of dependent slices in the slice segment headers for coded pictures referring to the PPS */
    uint8_t         sign_data_hiding_flag;                      /**< \brief Specifies that sign bit hiding is enabled */
    uint8_t         cabac_init_present_flag;                    /**< \brief Specifies that cabac_init_flag is present in slice headers referring to the PPS */
    int8_t          num_ref_idx_l0_default_active;              /**< \brief Specifies the inferred value of num_ref_idx_l0_active for P and B slices */
    int8_t          num_ref_idx_l1_default_active;              /**< \brief Specifies the inferred value of num_ref_idx_l1_active */
    int8_t          pic_init_qp;                                /**< \brief Specifies the initial value of luma_qp for each slice */
    uint8_t         constrained_intra_pred_flag;                /**< \brief Specifies that intra prediction allows usage of residual data and decoded samples of neighbouring coding blocks coded using either intra or inter prediction modes */
    uint8_t         transform_skip_enabled_flag;                /**< \brief Specifies that transform_skip_flag may be present in the residual coding syntax */
    uint8_t         cu_qp_delta_enabled_flag;                   /**< \brief Specifies that the diff_cu_qp_delta_depth syntax element is present in the PPS and that cu_qp_delta_abs may be present in the transform unit syntax */
    uint8_t         max_cu_dqp_depth;                           /**< \brief Specifies the difference between the luma coding tree block size and the minimum luma coding block size of coding units that convey cu_qp_delta_abs and cu_qp_delta_sign_flag */
    uint8_t         cu_to_qp_shift;
    uint8_t         min_qp_group_size;
    int8_t          cb_qp_offset;                               /**< \brief Specify offsets to the luma quantization parameter luma_qp used for deriving chroma_qp[0] */
    int8_t          cr_qp_offset;                               /**< \brief Specify offsets to the luma quantization parameter luma_qp used for deriving chroma_qp[1] */
    uint8_t         slicelevel_chroma_qp_flag;                  /**< \brief */
    uint8_t         weighted_pred_flag;                         /**< \brief Specifies that weighted prediction is applied to P slices */
    uint8_t         weighted_bipred_flag;                       /**< \brief Specifies that weighted prediction is applied to B slices */
    uint8_t         output_flag_present_flag;                   /**< \brief Indicates that the pic_output_flag syntax element is present in the associated slice headers */
    uint8_t         transquant_bypass_enabled_flag;             /**< \brief Specifies that cu_transquant_bypass_flag is present */
    uint8_t         tiles_enabled_flag;                         /**< \brief Specifies that there is more than one tile in each picture referring to the PPS */
    uint8_t         entropy_coding_sync_enabled_flag;           /**< \brief Specifies that a specific synchronization process for context variables */
    uint8_t         uniform_spacing_flag;                       /**< \brief Specifies that tile column boundaries and likewise tile row boundaries are distributed uniformly across the picture */
    uint8_t         num_tile_columns;                           /**< \brief Specifies the number of tile columns partitioning the picture */
    uint8_t         num_tile_rows;                              /**< \brief Specifies the number of tile rows partitioning the picture */
    uint16_t        column_width[MAX_NUM_TILE_COLUMNS];         /**< \brief Specifies the width of the i-th tile column in units of coding tree blocks */
    uint16_t        row_height[MAX_NUM_TILE_ROWS];              /**< \brief Specifies the height of the i-th tile row in units of coding tree blocks */
    uint8_t         loop_filter_across_tiles_enabled_flag;      /**< \brief Specifies that in-loop filtering operations may be performed across tile boundaries in pictures referring to the PPS */
    uint8_t         loop_filter_across_slices_enabled_flag;     /**< \brief Specifies that in-loop filtering operations may be performed across left and upper boundaries of slices referring to the PPS */
    uint8_t         deblocking_filter_control_present_flag;     /**< \brief Specifies the presence of deblocking filter control syntax elements in the PPS */
    uint8_t         deblocking_filter_override_enabled_flag;    /**< \brief Specifies the presence of deblocking_filter_override_flag in the slice headers for pictures referring to the PPS */
    uint8_t         pps_deblocking_filter_disabled_flag;        /**< \brief Specifies that the operation of deblocking filter is not applied for slices referring to the PPS in which slice_deblocking_filter_disabled_flag is not present */
    int8_t          beta_offset;                                /**< \brief Specify the default deblocking parameter offset for β which is applied for slices referring to the PPS */
    int8_t          tc_offset;                                  /**< \brief Specify the default deblocking parameter offset for tC which is applied for slices referring to the PPS */
    uint8_t         scaling_list_data_present_flag;             /**< \brief Specifies that parameters are present in the PPS to modify the scaling lists specified in the active SPS */
    uint8_t         scaling_list_coefs[4][6][64];               /**< \brief PPS scaling list data */
    uint8_t         scaling_list_DC[4][6];                      /**< \brief PPS scaling list DC coefficients */

    uint8_t         lists_modification_present_flag;            /**< \brief Specifies that the syntax structure ref_pic_lists_modification( ) is present in the slice segment header */
    int8_t          log2_parallel_merge_level;                  /**< \brief Is used in the derivation process for luma motion vectors for merge mode */
    uint8_t         num_extra_slice_header_bits;                /**< \brief Specifies that no extra slice header bits are present in the slice header RBSP for coded pictures referring to the PPS */
    uint8_t         slice_header_extension_present_flag;        /**< \brief Specifies that no slice segment header extension syntax elements are present in the slice segment headers for coded pictures referring to the PPS */

    uint8_t         pps_extension_flag;                         /**< \brief Specifies that pps_extension_data_flag syntax elements are present in the PPS RBSP syntax structure */
    uint8_t         pps_range_extension_flag;                   /**< \brief */
    uint8_t         pps_multilayer_extension_flag;              /**< \brief */
    uint8_t         pps_extension_6bits;                        /**< \brief */
    uint8_t         pps_extension_data_flag;                    /**< \brief Its presence and value do not affect decoder conformance to profiles specified in this version of this Specification */

    uint8_t         log2_max_transform_skip_block_size;         /**< \brief */
    uint8_t         cross_component_prediction_enabled_flag;    /**< \brief */
    uint8_t         chroma_qp_offset_list_enabled_flag;         /**< \brief */
    uint8_t         diff_cu_chroma_qp_offset_depth;             /**< \brief */
    uint8_t         chroma_qp_offset_list_len;                  /**< \brief */
    int8_t          cb_qp_offset_list[6];                       /**< \brief */
    int8_t          cr_qp_offset_list[6];                       /**< \brief */
    uint32_t        log2_sao_offset_scale_luma;                 /**< \brief */
    uint32_t        log2_sao_offset_scale_chroma;               /**< \brief */
} hevc_pic_par_set_t;
/** @} */

/**
 * @name Weighted prediction data
 * @{
 **/
 /**
 *@brief These parameters are used for weighted prediction.
 */
typedef struct hevc_weight_data_s
{
    int16_t o;                                         /**< \brief Auxiliary parameter */
    int16_t shift;                                     /**< \brief Auxiliary parameter */
    int16_t round;                                     /**< \brief Auxiliary parameter */
    int16_t weight;                                    /**< \brief Specifies coefficient multiplier */
    uint8_t present_flag;                              /**< \brief Specifies that the weighted prediction is enabled */
    uint8_t log2_weight_denom;                         /**< \brief Is the base 2 logarithm of the denominator for all weighting factors */
} hevc_weight_data_t;
/** @} */

/**
 * @name Slice header
 * @{
 **/
 /**
 *@brief Parameters for slice description.
 */
typedef struct hevc_slice_hdr_s
{
    uint8_t         first_slice_in_pic_flag;            /**< \brief Specifies that the slice segment is the first slice segment of the picture in decoding order */
    uint8_t         no_output_of_prior_pics_flag;       /**< \brief Ignored */
    uint8_t         slice_pic_parameter_set_id;         /**< \brief Specifies the value of identifier for the PPS in use */
    uint8_t         dependent_slice_segment_flag;       /**< \brief Specifies that the value of each slice segment header syntax element that is not present is inferred to be equal to the value of the corresponding slice segment header syntax element in the slice header */
    uint32_t        slice_segment_address;              /**< \brief Specifies the address of the first coding tree block in the slice segment */
    uint32_t        entry_point_offset[MAX_NUM_ENTRY_POINT_OFFSETS]; /**< \brief specifies the i-th entry point offset in bytes, and is represented by offset_len bits */    // or substream_sizes!
    uint16_t        num_entry_point_offsets;            /**< \brief specifies the number of entry_point_offset[ i ] syntax elements in the slice header */
    uint8_t         offset_len;                         /**< \brief specifies the length, in bits, of the entry_point_offset[ i ] syntax elements */

    uint8_t         slice_type;                         /**< \brief Specifies the coding \ref slice_type_t "type" of the slice */
    uint8_t         pic_output_flag;                    /**< \brief Affects the decoded picture output and removal processes */
    hevc_short_term_ref_pic_set_t local_rps;            /**< \brief Specifies short-term RPS of current picture */
    hevc_short_term_ref_pic_set_t* p_rps;               /**< \brief Specifies pointer to short-term RPS in the active SPS */
    int32_t         pic_order_cnt_lsb;                  /**< \brief Specifies the picture order count modulo max_pic_order_cnt_lsb for the current picture */
    uint8_t         short_term_ref_pic_set_sps_flag;    /**< \brief Specifies that the short-term RPS of the current picture is derived based on one of the short-term RPS in the active SPS */

    uint8_t         delta_poc_msb_present_flag[16];     /**< \brief Specifies thatdelta_poc_msb_cycle_lt[ i ] is present */
    int16_t         delta_poc_msb_cycle_lt[16];         /**< \brief Is used to determine the value of the most significant bits of the picture order count value of the i-th entry in the long-term RPS of the current picture */

    uint8_t         sao_luma_flag;                      /**< \brief Specifies that SAO is enabled for the luma component in the current slice */
    uint8_t         sao_chroma_flag;                    /**< \brief Specifies that SAO is enabled for the chroma component in the current slice */
    uint8_t         enable_temporal_mvp_flag;           /**< \brief Specifies whether temporal motion vector predictors can be used for inter prediction */

    uint8_t         num_ref_idx_active_override_flag;   /**< \brief Specifies that the syntax element num_ref_idx_active[ 0 ] is present for P and B slices and that the syntax element num_ref_idx_active[ 1 ] is present for B slices */
    uint8_t         num_ref_idx_active[2];              /**< \brief Specifies the maximum reference index for reference picture list 0 and list 1 that may be used to decode the slice */
    uint8_t         ref_pic_list_modification_flag_l0;  /**< \brief */
    uint8_t         ref_pic_list_modification_flag_l1;  /**< \brief */
    uint8_t         list_entry_l0[32];                  /**< \brief */
    uint8_t         list_entry_l1[32];                  /**< \brief */

    uint8_t         mvd_l1_zero_flag;                   /**< \brief */
    uint8_t         cabac_init_flag;                    /**< \brief Specifies the method for determining the initialization table used in the initialization process for context variables */
    uint8_t         collocated_from_l0_flag;            /**< \brief Specifies that the collocated picture used for temporal motion vector prediction is derived from reference picture list 0 */
    uint8_t         collocated_ref_idx;                 /**< \brief Specifies the reference index of the collocated picture used for temporal motion vector prediction */
    uint8_t         max_num_merge_cand;                 /**< \brief Specifies the maximum number of merging MVP candidates supported in the slice */

    // Weighted prediction data.
    hevc_weight_data_t   weight_data[2][16][3];         /**< \brief Weighted prediction structure */    // [REF_PIC_LIST][NUM_MAX_REF][0:Y, 1:U, 2:V]

    int8_t          slice_qp;                           /**< \brief Specifies the initial value of luma_qp to be used for the coding blocks in the slice */
    int8_t          slice_qp_delta_cb;                  /**< \brief Specifies a difference to be added to the value of cb_qp_offset from pps when determining the value of the Cb quantization parameter */
    int8_t          slice_qp_delta_cr;                  /**< \brief Specifies a difference to be added to the value of cb_qp_offset from pps when determining the value of the Cr quantization parameter */

    uint8_t         cu_chroma_qp_offset_enabled_flag;   /**< \brief Specifies that the cu_chroma_qp_offset_flag may be present in the transform unit syntax */
    uint8_t         deblocking_filter_override_flag;    /**< \brief Specifies that deblocking parameters are present in the slice header */
    uint8_t         slice_deblocking_filter_disabled_flag; /**< \brief Specifies that the operation of the deblocking filter is not applied for the current slice */
    int8_t          beta_offset;                        /**< \brief Specify the default deblocking parameter offset for β for the current slice */
    int8_t          tc_offset;                          /**< \brief Specify the default deblocking parameter offset for tC for the current slice */
    uint8_t         loop_filter_across_slices_enabled_flag; /**< \brief Specifies that in-loop filtering operations may be performed across the left and upper boundaries of the current slice */

    uint32_t        reserved[8];                        /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t       reserved_ptr[8];                    /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_slice_hdr_t;
/** @} */

/**
* @name Supplemental enhancement information
* @{
**/
/**
*@brief These parameters is used to create SEI messages.
*/
typedef struct hevc_sei_payload_s
{
    uint8_t *payload;                   /**< \brief SEI data buffer */
    uint32_t payload_type;              /**< \brief specifies type of SEI payload */
    uint32_t payload_size;              /**< \brief specifies size of SEI payload */
} hevc_sei_payload_t;
/** @} */

/**
*@brief Vector of SEI messages.
*/
typedef struct hevc_sei_messages_s
{
    hevc_sei_payload_t *sei_payload;    /**< \brief Vector of SEI messages */
    uint16_t num_messages;              /**< \brief Number of messages */
} hevc_sei_messages_t;

/** \brief HEVC picture structure. Refer to the Table D.2 of the H.265 standard for more details. */
typedef enum hevc_pic_struct_e
{
    HEVCVD_FRAME_PROGRESSIVE = 0, /**< \brief Progressive frame. */
    HEVCVD_FRAME_INTERLACED_TOP_FIELD = 1, /**< \brief Top field of the interlaced frame. */
    HEVCVD_FRAME_INTERLACED_BOTTOM_FIELD = 2, /**< \brief Bottom field of the interlaced frame. */
    HEVCVD_FRAME_PROGRESSIVE_TOP_FIELD_BOTTOM_FIELD = 3, /**< \brief Top field, bottom field, in that order */
    HEVCVD_FRAME_PROGRESSIVE_BOTTOM_FIELD_TOP_FIELD = 4, /**< \brief Bottom field, top field, in that order */
    HEVCVD_FRAME_PROGRESSIVE_TOP_FIELD_BOTTOM_FIELD_TOP_FIELD_REPEATED = 5, /**< \brief Top field, bottom field, top field repeated, in that order */
    HEVCVD_FRAME_PROGRESSIVE_BOTTOM_FIELD_TOP_FIELD_BOTTOM_FIELD_REPEATED = 6, /**< \brief Bottom field, top field, bottom field repeated, in that order */
    HEVCVD_FRAME_PROGRESSIVE_FRAME_DOUBLING = 7, /**< \brief Frame doubling */
    HEVCVD_FRAME_PROGRESSIVE_FRAME_TRIPLING = 8, /**< \brief Frame tripling */
    HEVCVD_FRAME_INTERLACED_TOP_FIELD_PAIRED_WITH_PREVIOUS_BOTTOM_FIELD = 9, /**< \brief Top field paired with previous bottom field in output order. */
    HEVCVD_FRAME_INTERLACED_BOTTOM_FIELD_PAIRED_WITH_PREVIOUS_TOP_FIELD = 10, /**< \brief Bottom field paired with previous top field in output order. */
    HEVCVD_FRAME_INTERLACED_TOP_FIELD_PAIRED_WITH_NEXT_BOTTOM_FIELD = 11, /**< \brief Top field paired with next bottom field in output order. */
    HEVCVD_FRAME_INTERLACED_BOTTOM_FIELD_PAIRED_WITH_NEXT_TOP_FIELD = 12, /**< \brief Bottom field paired with next top field in output order. */
} hevc_pic_struct_t;

/** \brief Texture info for d3d11. */
typedef struct hevc_d3d11_info_s
{
    struct ID3D11Texture2D *texture;
    uint32_t subresource_id;
    uint32_t reserved[4];           /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t reserved_ptr[4];      /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_d3d11_info_t;

/**
 * @name Picture information
 * @{
 **/
 /**
 *@brief These parameters are used to access pictures as the enter or exit the decoding process.
 */
typedef struct hevc_picture_s
{
    const hevc_vid_par_set_t *vps; /**< \brief Pointer to active VPS. */
    const hevc_seq_par_set_t *sps; /**< \brief Pointer to active SPS. */
    const hevc_pic_par_set_t *pps; /**< \brief Pointer to active PPS. */

    const uint8_t *pixel[3];       /**< \brief Pointers to Y, U, V planes of decoded picture. Will be NULL if hardware acceleration is used.*/
    uint32_t stride[3];            /**< \brief Strides for Y, U, V planes in bytes. */
    uint32_t fourcc;               /**< \brief FourCC of decoded picture. */

    hevc_au_t access_unit_info;    /**< \brief Information about current access unit (picture). Contains information about all NAL units of the access unit. */
    uint32_t segments_count;       /**< \brief Count of slice segments of the picture (including dependent and independent). */
    uint32_t slices_count;         /**< \brief Count of independent slice segments of the picture. */
    const hevc_slice_hdr_t **slice_hdr; /**< \brief Array of all slice headers the picture. */

    int32_t poc;                   /**< \brief Picture order count (POC). */
    uint8_t error;                 /**< \brief Signals that picture has been decoded with errors. */
    uint8_t skipped;               /**< \brief A picture is skipped from output. */

    void * user_data;              /**< \brief Pointer to user data which was attached to frame using \ref bufstream::auxinfo "auxinfo" with info_ID=\ref SET_USER_DATA. */
    uint16_t user_data_size;       /**< \brief Size of attached user data packet. */
    void * ts_data;                /**< \brief Pointer to time stamp which was attached to frame using \ref bufstream::auxinfo "auxinfo" with info_ID=\ref SET_PIC_PTS. */
    uint16_t ts_data_size;         /**< \brief Size of attached time stamp. */
    uint16_t width;                /**< \brief Width of picture in pixels. */
    uint16_t height;               /**< \brief Height of picture in pixels. */
    hevc_sei_messages_t sei;       /**< \brief Vector of SEI messages. */
    union
    {
        struct IDirect3DSurface9* surface;  /**< \brief Pointer to hardware surface. Will be NULL if software decoding is used. */
        const hevc_d3d11_info_t* d3d11_info;
    };

    const void* context;           /**< \brief This field is used for internal purposes by decoder. Shall not be modified. */
    colorimetry_t colorimetry;/**< \brief Colorimetry for render hevc_picture_t */

    hevc_pic_struct_t pic_struct;  /**< \brief Indicates whether a picture should be displayed as a frame or as one or more fields. */
    uint8_t full_frame_available;  /**< \brief Flag indicating availability of full frame. For progressive content this flag is set for each frame. For interlaced content this flag is set on fields which have counterpart field. */
    struct hevc_picture_s* counterpart_field; /**< \brief Pointer to the counterpart field. Is set only on interlaced streams. */

    uint32_t reserved[8];           /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t reserved_ptr[8];      /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_picture_t;
/** @} */

/** \brief Stream parameter flags. Can be provided to stream_params_t::flags. */
/** \name Auxiliary commands, also see mcdefs.h */
#define STREAM_PARAMS_FLAG_WORKER_BINDING_NONE      1   /**< \brief Disable binding on internal threadpool workers (may reduce performance, unsupported on Windows) \hideinitializer */

/**
 * @name Stream parameters
 * @{
 **/
 /**
 *@brief These parameters is used to initialize bufstream interface.
 */
typedef struct stream_params_s
{
    uint64_t nodeset;               /**< \brief Specifies NUMA nodes (0 forces to use single NUMA node of the current thread) */
    uint32_t flags;                 /**< \brief Specifies set of flags to be used for internal threadpool creation */
    uint32_t reserved[3];           /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    mcr_thread_pool_t threadpool;   /**< \brief Specifies mcruntime threadpool instance (if 0 an internal threadpool instance is used) */
    uintptr_t reserved_ptr[3];      /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} stream_params_t;
/** @} */

/**
 * @name HEVC hardware adapter
 * @{
 **/
/**
 * \brief This structure contains description of a hardware adapter capable of HEVC decoding.
 * 
 * \ref hevc_hardware_adapter_t::handle "handle" field can used to choose required hardware accelerator using \ref bufstream::auxinfo "auxinfo" API call with info_ID=\ref SET_HW_ADAPTER.
 * \snippet decoder.cpp Choose video adapter if selected
 */
typedef struct {
    uintptr_t handle;               /**< \brief Indicates index of hardware accelerated adapter. If system has only adapter one this field will be equal to zero. */
    profile_idc_e profile;          /**< \brief Specifies highest \ref profile_idc_e "profile" supported by hardware accelerator. */
    profile_idc_e stable_profile;   /**< \brief Specifies highest \ref profile_idc_e "profile" that can be decoded without known driver side issues leading to crashes. */
    wchar_t description[512];       /**< \brief Human readable name of the hardware accelerator. */
    uint32_t reserved[4];           /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
    uintptr_t reserved_ptr[4];      /**< \brief Shall be zero. Otherwise it may cause undefined behavior. */
} hevc_hardware_adapter_t;
/** @} */

/**
 * @name HEVC hardware adapters
 * @{
 **/
/**
 * @brief This structure enumerates installed GPUs, which support hardware acceleration for HEVC decoding.
 * 
 * The complete list of installed GPUs can be obtained using \ref bufstream::auxinfo "auxinfo" API call with info_ID=\ref HWACC_GET_HW_CAPS.
 * \snippet decoder.cpp Enumerate hardware adapters
 */
typedef struct {
    uint8_t count;                        /**< \brief The total number of installed GPUs, which support hardware acceleration for HEVC decoding. */
    const hevc_hardware_adapter_t * set;  /**< \brief The C-style array of installed GPUs, which support hardware acceleration for HEVC decoding. */
} hevc_hardware_adapters_t;
/** @} */


/**
 * \addtogroup hevc_dec_callbacks HEVC Decoder callbacks
 @{
 **/

 /** \brief The optional callback that is called in the input thread in stream order when parsing of a video parameter set (VPS) finished. */
typedef void(*vps_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_vid_par_set_t* vps);

 /** \brief The optional callback that is called in the input thread in stream order when parsing of a sequence parameter set (SPS) finished. */
typedef void(*sps_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_seq_par_set_t* sps);

/** \brief The optional callback that is called in the input thread in stream order when parsing of a picture parameter set (PPS) finished. */
typedef void(*pps_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_pic_par_set_t* pps);

/** \brief The optional callback that is called in the input thread in stream order when parsing of a collection of SEI messages finished. */
typedef void(*sei_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_sei_payload_t* sei, const uint32_t count);

/** \brief The optional callback that is called in the input thread in stream order when parsing of a slice header finished. */
typedef void(*slice_header_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_slice_hdr_t *slice_hdr);

/** \brief The optional callback that is called in the input thread in stream order when parsing of a NAL unit finished. */
typedef void(*nalu_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_nalu_t* nalu);

/** \brief The optional callback that is called in the input thread in stream order when parsing of an access unit finished. */
typedef void(*pic_callback_t)(context_t context, const hevc_picture_t* picture);

/** \brief The optional callback that is called in a worker thread in arbitrary order when decoding of an access unit finished. */
typedef void(*pic_decoded_callback_t)(context_t context, const hevc_picture_t* picture);

/** \brief The optional callback that is called in an input/output/worker thread in stream/output order when it is time to output an access unit. */
typedef void(*pic_output_callback_t)(context_t context, const hevc_picture_t* picture);

/** \brief The optional callback that is called in an input/output/worker thread in stream/output order when an output access unit is skipped from display. */
typedef void(*pic_skipped_callback_t)(context_t context, const hevc_picture_t* picture);

/** \brief The optional callback that is called in the input thread in stream order when an error occurred. */
typedef void(*runtime_error_callback_t)(context_t context, const hevc_picture_t* picture, const hevc_runtime_error_t code);

/** \brief The optional callback that is called when the pool of decoder resources is empty. */
typedef void (*decoder_empty_callback_t)(context_t context);

/** \brief The optional callback that is called when the decoder yields resources to the empty pool. */
typedef void (*decoder_yield_callback_t)(context_t context);

/** \brief The optional callback that is called when the decoder releases all resources to the pool. */
typedef void (*decoder_flush_callback_t)(context_t context);
/** \} */

typedef struct callbacks_decoder_hevc_t
{
/**    \brief \copybrief vps_callback_t */
    vps_callback_t              vps_callback;
/**    \brief \copybrief sps_callback_t */
    sps_callback_t              sps_callback;
/**    \brief \copybrief pps_callback_t */
    pps_callback_t              pps_callback;
/**    \brief \copybrief sei_callback_t */
    sei_callback_t              sei_callback;
/**    \brief \copybrief slice_header_callback_t */
    slice_header_callback_t     slice_header_callback;
/**    \brief \copybrief nalu_callback_t */
    nalu_callback_t             nalu_callback;
/**    \brief \copybrief pic_callback_t */
    pic_callback_t              pic_callback;
/**    \brief \copybrief pic_decoded_callback_t */
    pic_decoded_callback_t      pic_decoded_callback;
/**    \brief \copybrief pic_output_callback_t */
    pic_output_callback_t       pic_output_callback;
/**    \brief \copybrief pic_skipped_callback_t */
    pic_skipped_callback_t      pic_skipped_callback;
/**    \brief \copybrief runtime_error_callback_t */
    runtime_error_callback_t    error_callback;
/**    \brief \copybrief decoder_empty_callback_t */
    decoder_empty_callback_t    empty_callback;
/**    \brief \copybrief decoder_yield_callback_t */
    decoder_yield_callback_t    yield_callback;
/**    \brief \copybrief decoder_flush_callback_t */
    decoder_flush_callback_t    flush_callback;
/**    \brief Shall be zero. Otherwise it may cause undefined behavior. */
    void* reserved1;
/**    \brief Shall be zero. Otherwise it may cause undefined behavior. */
    void* reserved2;
/**    \brief Shall be zero. Otherwise it may cause undefined behavior. */
    void* reserved[12];
} callbacks_decoder_hevc_t;

#ifdef __cplusplus
extern "C" {
#endif

/*!
\brief
Create HEVC Video Decoder
\param[in] callbacks - callbacks to external message handler and memory manager functions.
\param[in] callbacks_hevc - Pointer that is passed to the callback functions.
\param[in] stream_params - Stream parameters.
\return
Pointer to new instance of the MC HEVC Video Decoder or nullptr in case of error.
\note
For resource management used functions form \ref callbacks_t parameter.
*******************************************************************************/
    bufstream_tt* MC_EXPORT_API createDecoderHEVC(const callbacks_t* callbacks, const callbacks_decoder_hevc_t* callbacks_hevc, stream_params_t* stream_params);

/**
\brief Provides access to extended module API.
\return  Pointer to requested function or NULL
\param[in] func Identifier of module extended function
 */
    APIEXTFUNC MC_EXPORT_API getApiDecoderHEVC(uint32_t func);

#ifdef __cplusplus
}
#endif

#endif  /* __DEC_HEVC_API_INCLUDED__ */
