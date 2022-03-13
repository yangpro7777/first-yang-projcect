/** 
 @file  dec_aac.h
 @brief  AAC Decoder API
 
 @verbatim
 File: dec_aac.h

 Desc: AAC Decoder API
 
 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/
 

#if !defined (__DEC_AAC_API_INCLUDED__)
#define __DEC_AAC_API_INCLUDED__

#include "mcdefs.h"
#include "common_aac.h"
#include "bufstrm.h"
#include "mcapiext.h"

/////////////////////////////////////////////////////////////////////////////
// API structures & defines
/////////////////////////////////////////////////////////////////////////////

#define AAC_DEC_MAX_CHAN        64		/**< @brief Maximun number of channels */

/**
 * @name Decoded frame information
 * @{
 **/
 
 /**
  * @brief This structure to convey information about the decoded AAC frame
  */
typedef struct
{
  int frame_size;			/**< @brief Size in bytes of the AAC frame including all headers and stuffing */

  int sampling_rate;		/**< @brief Sampling rate of the output audio data */
  int num_channels;			/**< @brief Number of output channels */

  int samples_per_channel;	/**< @brief Number of audio samples decoded for every channel */
							
  int decoded_frame_size;	/**< @brief Size in bytes of the decoded frame */
  int he;					/**< @brief Information whether High Efficient AAC encoding was used in the decoded stream */
  int priming_dur_bytes;    /**< @brief The calculated and skipped bytes according to priming duration */

  int reserved[9];

} aac_decoded_frame_info;
/** @} */


/**
* @name Ancillary data
* @{
**/

/**
  * @brief This structure to convey information about ancillary data
  */
typedef struct
{                   
    uint8_t mpeg_audio_type;                    /**< @brief MPEG audio type*/
    uint8_t dolby_surround_mode;                /**< @brief Dolby surround mode*/
    
    /**
    * @name Downmixing levels
    * @{
    **/
    uint8_t downmixing_levels_pesent;           /**< @brief Indicates that downmixing levels is present*/
    uint8_t center_mix_level_on;                /**< @brief Indicates that @ref center_mix_level_value is set*/
    uint8_t center_mix_level_value;             /**< @brief Indicates nominal down mix level of the centre channel with respect to the left and right front channels*/
    uint8_t surround_mix_level_on;              /**< @brief Indicates that @ref surround_mix_level_value is set*/
    uint8_t surround_mix_level_value;           /**< @brief Indicates nominal down mix level of the surround channels with respect to the left and right front channels*/
    /** @} */   

    /**
    * @name Audio coding mode
    * @{
    **/
    uint8_t audio_coding_mode_present;          /**< @brief Indicates that audio coding mode is present*/
    uint8_t audio_coding_mode;                  /**< @brief Audio coding mode*/
    uint8_t compression_value;                  /**< @brief The heavy compression factor used for monophonic down mix reproduction*/
    /** @} */

    /**
    * @name Coarse grain time code
    * @{
    **/
    uint8_t coarse_grain_timecode_present;      /**< @brief Indicates that coarse grain time code is present*/
    uint8_t coarse_grain_timecode_on;           /**< @brief If this field is set to 2, it indicates that @ref coarse_grain_timecode_value is set*/
    uint16_t coarse_grain_timecode_value;       /**< @brief Coarse grain time code value*/
    /** @} */

    /**
    * @name Fine grain time code
    * @{
    **/
    uint8_t fine_grain_timecode_present;        /**< @brief Indicates that fine grain time code is present*/
    uint8_t fine_grain_timecode_on;             /**< @brief If this field is set to 2, it indicates that @ref coarse_grain_timecode_value is set*/
    uint16_t fine_grain_timecode_value;         /**< @brief Fine grain time code value*/
    /** @} */
} ancillary_data_t;
/** @} */

/**
* @name Dynamic Range Control
* @{
**/

/**
  * @brief This structure to convey information about dynamic range control
  */
typedef struct
{ 
    uint8_t pce_tag_present;                   /**< @brief Indicates that program element tag is present*/
    uint8_t pce_instance_tag;                  /**< @brief Indicates with which program the dynamic range information is associated*/
    uint8_t drc_tag_reserved_bits;             /**< @brief Reserved*/

    uint8_t excluded_chns_present;             /**< @brief Indicates that excluded channels are present*/
    uint8_t exclude_mask[64];                  /**< @brief Flags indicating the audio channels of a program that are excluded from DRC processing using this DRC information*/
    uint8_t additional_excluded_chns;          /**< @brief Number of additional excluded channels*/

    uint8_t drc_bands_present;                 /**< @brief Indicates that DRC multi-band information is present*/
    uint8_t drc_band_incr;                     /**< @brief Number of DRC bands greater than 1 having DRC information*/
    uint8_t drc_interpolation_scheme;          /**< @brief Indicates which interpolation scheme is used for the DRC data in the SBR QMF domain*/
    uint8_t drc_band_top[16];                  /**< @brief The i-th element indicates top of i-th DRC band in units of 4 spectral lines*/

    uint8_t prog_ref_level_present;            /**< @brief Indicates that reference level is present*/
    uint8_t prog_ref_level;                    /**< @brief Reference level. A measure of long-term program audio level for all channels combined.*/
    uint8_t prog_ref_level_reserved_bits;      /**< @brief Reserved*/
    uint8_t dyn_rng_sgn[16];                   /**< @brief Dynamic range control sign information. It indicates the sign of @ref dyn_rng_ctl (0 if positive, 1 if negative)*/
    uint8_t dyn_rng_ctl[16];                   /**< @brief Dynamic range control magnitude information*/
} dynamic_range_control_t;    
/** @} */


/**
 * @name Decoded metadata information
 * @{
 **/

/**
  * @brief This structure to convey information about the decoded metadata
  */
typedef struct 
{
    uint8_t ancillary_data_present;                 /**< @brief A flag specifying if ancillary data is present*/
    ancillary_data_t ancillary_data;                /**< @brief Ancillary data*/

    uint8_t dynamic_range_info_present;             /**< @brief Indicates that dynamic range information is present*/
    dynamic_range_control_t dynamic_range_info;        /**< @brief Dynamic range control information*/

    uint8_t reserved[17];
} metadata_info_t;
/** @} */

/**
 * @name Parsed program config element (PCE)
 * @{
 **/

/**
  * @brief This structure to convey information about the parsed program config element (PCE)
  */
typedef struct
{
    uint8_t is_parsed;                          /**< @brief Indicates that program config element was parsed */
    uint8_t element_instance_tag;               /**< @brief Indicates that program element tag is present */
    uint8_t profile;                            /**< @brief The two-bit profile index */
    uint8_t sampling_frequency_index;           /**< @brief Indicates the sampling rate of the program (and all other programs in this bitstream) */
    uint8_t num_front_channel_elements;         /**< @brief The number of audio syntactic elements in the front channels, front center to back center, 
                                                * symmetrically by left and right, or alternating by left and right in the case of single channel elements */
    uint8_t num_side_channel_elements;          /**< @brief Number of elements to the side as above */
    uint8_t num_back_channel_elements;          /**< @brief As number of side and front channel elements, for back channels */
    uint8_t num_lfe_channel_elements;           /**< @brief Number of LFE channel elements associated with this program */
    uint8_t num_assoc_data_elements;            /**< @brief The number of associated data elements for this program */
    uint8_t num_valid_cc_elements;              /**< @brief The number of CCE's that can add to the audio data for this program */

    uint8_t mono_mixdown_present;               /**< @brief One bit, indicating the presence of the mono mixdown element */
    uint8_t mono_mixdown_element_number;        /**< @brief The number of a specified SCE that is the mono mixdown */

    uint8_t stereo_mixdown_present;             /**< @brief One bit, indicating that there is a stereo mixdown present */
    uint8_t stereo_mixdown_element_number;      /**< @brief The number of a specified CPE that is the stereo mixdown element */

    uint8_t matrix_mixdown_idx_present;         /**< @brief One bit indicating the presence of matrix mixdown information by means of a stereo matrix 
                                                * coefficient index, for all configurations other than the 3/2 format this bit must be zero */
    uint8_t matrix_mixdown_idx;                 /**< @brief Two bit field, specifying the index of the mixdown coefficient to be used 
                                                * in the 5-channel to 2-channel matrix-mixdown */
    uint8_t pseudo_surround_enable;             /**< @brief One bit, indicating the possibility of mixdown for pseudo surround reproduction */

    uint8_t front_element_is_cpe[15];           /**< @brief indicates whether a SCE or a CPE is addressed as a front element,'0' selects an SCE,'1' selects an CPE, 
                                                * the instance of the SCE or CPE addressed is given by front_element_tag_select */
    uint8_t front_element_tag_select[15];       /**< @brief The instance_tag of the SCE/CPE addressed as a front element */

    uint8_t side_element_is_cpe[15];            /**< @brief see @ref front_element_is_cpe, but for side elements */
    uint8_t side_element_tag_select[15];        /**< @brief see @ref front_element_tag_select, but for side elements */

    uint8_t back_element_is_cpe[15];            /**< @brief see @ref front_element_is_cpe, but for back elements */
    uint8_t back_element_tag_select[15];        /**< @brief see @ref front_element_tag_select, but for back elements */

    uint8_t lfe_element_tag_select[3];          /**< @brief instance_tag of the LFE addressed */

    uint8_t assoc_data_element_tag_select[7];   /**< @brief instance_tag of the DSE addressed */

    uint8_t cc_element_is_ind_sw[15];           /**< @brief One bit, indicating that the corresponding CCE is an independently switched coupling channel */ 
    uint8_t valid_cc_element_tag_select[15];    /**< @brief instance_tag of the CCE addressed */

    uint8_t comment_field_bytes;                /**< @brief The length, in bytes, of the @ref comment_field_data */

    char    comment_field_data[256];            /**< @brief The data in the comment field */

    uint8_t reserved[10];
} pce_t;
/** @} */

/**
  * @brief This structure to convey information about stream structure. Call for AACDEC_GET_FRAME_STRUCTURE auxinfo to retrieve this information.
  */
typedef struct
{
    int32_t num_sce;    /**< @brief number of Single Channel Elements */
    int32_t num_cpe;    /**< @brief number of Channel Pair Elements */
    int32_t num_cce;    /**< @brief number of CCE elements*/
    int32_t num_lfe;    /**< @brief number of LFE elements*/
    int32_t num_dse;    /**< @brief number of Data Stream Elements */
    int32_t num_pce;    /**< @brief number of Program Config Elements */
    int32_t num_fil;    /**< @brief number of FIL Elements */

    int32_t reserved[25];
} frame_struct_t;

/**
 * @name Decoder configuration
 * @{
 **/
 
 /**
 * @brief This structure is used to hold settings and input stream information
 * @note The structure consists of two sets of fields: input stream information and decoding options.
 * Input stream information fields could be used to convey parameters of the raw input stream to the
 * decoder. These parameters could also be conveyed with AudioSpecificConfig by means of
 * @ref bufstream.auxinfo(@ref PARSE_AUD_HDR) call. Note that the decoder uses @ref aot, @ref sampling_rate and
 * @ref num_channels fields only when @ref bitstream_format is @ref AAD_BSF_RAW. Decoding options fields could
 * be used to configure decoder output.\n
 * Any of the structure fields can be set to @ref AAD_PARAM_DONT_CHANGE (-1). This value is utilized to
 * signal that the user doesn't intend to alter default settings or input stream information. It's highly
 * recommended to set reserved structure fields to @ref AAD_PARAM_DONT_CHANGE for future compatibility.
 */
typedef struct
{
  /**
 * @name Input stream information
 * @{
 **/
  int bitstream_format;				/**< @brief Format of the input stream */

  int aot;							/**< @brief Audio object type at the input when decoding raw AAC stream */
  int sampling_rate;				/**< @brief Sampling rate at the input when decoding raw AAC stream */
  int num_channels;					/**< @brief Channel configuration (number of audio channels) at the input when decoding raw AAC stream */

  int reserved0[12];
  /** @} */

  /**
 * @name Decoding options
 * @{
 **/
  int max_output_channels;			/**< @brief Maximum number of output channels */
  int output_format;				/**< @brief Output sample format */
  int decode_he;					/**< @brief To enable or disable SBR (Spectral Band Replication, HE AAC v1) and PS (Parametric Stereo, HE AAC v2) decoding */

  int insert_silence_on_error;
  int ignore_crc_errors;
  int priming_dur;                  /**< @brief The delay duration in PTS to cut priming samples from decoded stream. For custom value use values from 0 to 2147483648 (PTS).*/
  int64_t playback_dur;             /**< @brief The playback time duration of audio stream in PTS to cut remainder samples from decoded stream. */
  int encoder_type;                 /**< @brief Encoder type for using of predefined values of the delay duration, see AAD_ENCODER_*. Use it instead of priming_dur for some cases.*/
  int preroll_dur;                  /**< @brief The preroll time duration in PTS to cut preroll samples from decoded stream. For custom value use values from 0 to 2147483648 (PTS).*/
  int disable_pcm_ch_reordering;    /**< @brief Disable PCM channel re-ordering. If set to 1 stream channel order is preserved, otherwise channel order is changed to default PCM order.*/
  int disable_low_power_sbr;        /**< @brief Disable Low Power SBR. If set to 1 low power is disabled, otherwise low power is enabled by default. */
  int reserved1[4];
  /** @} */
} aac_decoder_config;
/** @} */

#define AAD_PARAM_DONT_CHANGE   -1		/**< @brief Do not change the parameter */

/**
 * @name Decode HE AAC or not
 * @{
 **/
#define AAD_HE_DONT_DECODE      0		/**< @brief Do not decode SBR and PS data */
#define AAD_HE_DECODE           1		/**< @brief Decode SBR and PS data */
/** @} */

/**
 * @name Downmix options
 * @{
 **/
#define AAD_DMX_NO_OUTPUT       0		/**< @brief Do not output any decoded data */
#define AAD_DMX_MONO            1		/**< @brief Downmix to mono if needed */
#define AAD_DMX_STEREO          2		/**< @brief Downmix to stereo if needed */
#define AAD_DMX_5POINT1         6		/**< @brief Downmix to 5.1 if needed */
#define AAD_DMX_7POINT1         8		/**< @brief Downmix to 7.1 if needed */
/** @} */

/**
 * @name Input bitstream format
 * @{
 **/
#define AAD_BSF_ADTS            0		/**< @brief Audio data transport stream (ADTS) */
#define AAD_BSF_ADIF            1		/**< @brief Audio Data Interchange Format (ADIF) */
#define AAD_BSF_RAW             2		/**< @brief Raw AAC stream */
#define AAD_BSF_LOAS            3		/**< @brief Low Overhead Audio Stream (LOAS) */
#define AAD_BSF_LATM            4		/**< @brief Low-overhead MPEG-4 Audio Transport Multiplex */
#define AAD_BSF_UNKNOWN         5		/**< @brief Unknown */
 /** @} */
 
 /**
 * @name Decoded sample format (various bit depth + little- / big-endian)
 * @{
 **/
#define AAD_DSF_8U              0		/**< @brief PCM, 8 bits per sample, unsigned */
#define AAD_DSF_8S              1		/**< @brief PCM, 8 bits per sample, signed */
#define AAD_DSF_16LE            2		/**< @brief PCM, 16 bits per sample, signed, little-endian*/
#define AAD_DSF_16BE            3		/**< @brief PCM, 16 bits per sample, signed, big-endian */
#define AAD_DSF_24LE            4
#define AAD_DSF_24BE            5
#define AAD_DSF_IEEE32          6       /**< @brief Not supported */
#define AAD_DSF_IEEE64          7       /**< @brief Not supported */
 /** @} */
 
#define AAD_VER_MPEG4           0
#define AAD_VER_MPEG2           1

/**
 * @name MPEG-4 audio object types
 * @{
 **/
#define AAD_AOT_NULL            0
#define AAD_AOT_AAC_MAIN        1		/**< @brief AAC Main object */
#define AAD_AOT_AAC_LC          2		/**< @brief AAC Low Complexity object, this value should also be used
										* when decoding HE AAC streams, don't use @ref AAD_AOT_SBR */
#define AAD_AOT_AAC_SSR         3
#define AAD_AOT_AAC_LTP         4
#define AAD_AOT_SBR             5
#define AAD_AOT_AAC_SCALABLE    6
#define AAD_AOT_TWINVQ          7
#define AAD_AOT_CELP            8
#define AAD_AOT_HVXC            9
#define AAD_AOT_RESERVED10      10
#define AAD_AOT_RESERVED11      11
#define AAD_AOT_TTSI            12
#define AAD_AOT_MAIN_SYNTHETIC  13
#define AAD_AOT_WAVETABLE       14
#define AAD_AOT_GENERAL_MIDI    15
#define AAD_AOT_ALGORITHMIC_FX  16
#define AAD_AOT_ER_AAC_LC       17
#define AAD_AOT_RESERVED18      18
#define AAD_AOT_ER_AAC_LTP      19
#define AAD_AOT_ER_AAC_SCALABLE 20
#define AAD_AOT_ER_TWINVQ       21
#define AAD_AOT_ER_BSAC         22
#define AAD_AOT_ER_AAC_LD       23
#define AAD_AOT_ER_CELP         24
#define AAD_AOT_ER_HVXC         25
#define AAD_AOT_ER_HILN         26
#define AAD_AOT_ER_PARAMETRIC   27
#define AAD_AOT_SSC             28
#define AAD_AOT_RESERVED29      29
#define AAD_AOT_RESERVED30      30
#define AAD_AOT_RESERVED31      31
 /** @} */

 /**
 * @name Encoder name for predefined duration value of priming samples
 * @{
 **/
#define AAD_ENCODER_DEFAULT     -1		/**< @brief Default value*/
#define AAD_ENCODER_MC          0		/**< @brief Stream was created by MainConcept AAC Encoder*/
#define AAD_ENCODER_FHG         1		/**< @brief Stream was created by MainConcept Fraunhofer AAC Encoder */
 /** @} */

// An array audio_object_types is removed as not used.

/**
 * @name Supported features, call @ref bufstream.auxinfo(dec, 0, AACDEC_FEAT_..., NULL, 0) and check returned value
 * @{
 **/
#define AACDEC_FEAT_HE              0x30000001
#define AACDEC_FEAT_MAXCHAN         0x30000002
#define AACDEC_GET_PCE              0x30000003  /**<   Get program_config_element from AAC decoder */
#define AACDEC_GET_FRAME_STRUCTURE  0x30000004  /**<   Get frame structure in frame_struct_t structure */
 
#define AACDEC_FEAT_UNKNOWN         0		/**< @brief It's unknown if the feature is supported or not, i.e. you use an
												* older version of decoder which doesn't support AACDEC_FEAT_... calls */

#define AACDEC_FEAT_HE_NOTSUPPORTED 100
#define AACDEC_FEAT_HE_SUPPORTED    101
 /** @} */
 
 
/////////////////////////////////////////////////////////////////////////////
// API functions
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/** 
    * @brief Create the AAC Decoder
    * @brief Call this function to get a @ref bufstream interface for the AAC decoder
    * @return  point to a @ref bufstream interface
    * @brief Deprecated
    **/
bufstream_tt* MC_EXPORT_API open_AACin_Audio_stream(void);

/** 
    * @brief Create the AAC Decoder
    * @brief Call this function to get a @ref bufstream interface for the AAC decoder
	* @param[in] get_rc a pointer to a get_rc function
    * @return  point to a @ref bufstream interface
    **/
bufstream_tt* MC_EXPORT_API createDecoderAAC(const callbacks_t* callbacks, long reserved1, long reserved2);

APIEXTFUNC MC_EXPORT_API AACin_Audio_GetAPIExt(uint32_t func);


#ifdef __cplusplus
}
#endif

#endif // #if !defined (__DEC_AAC_API_INCLUDED__)

