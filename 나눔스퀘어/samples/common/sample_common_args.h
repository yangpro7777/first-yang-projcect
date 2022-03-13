/********************************************************************
 @file: sample_common_args.h
 @brief Common command line parsing routines

 @verbatim
 Copyright (c) 2019 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.  Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @verbatim
*********************************************************************/

#ifndef SAMPLE_COMMON_ARGS_H_INCLUDED
#define SAMPLE_COMMON_ARGS_H_INCLUDED

#include <algorithm>
#include <string.h>
#include <vector>

#include "mctypes.h"
#include <cstddef>

#define ITEM_PREFIX     "-"

/**
 *@brief Description of the item types
 */
typedef enum item_type_s
{
    ItemTypeNoArg = 0,
    ItemTypeInt,
    ItemTypeInt64,
    ItemTypeDouble,
    ItemTypeString

} item_type_e;

#define ITEM_NOT_INIT   -123456


#define MAX_ITEM_NAME   100                             /**@brief Max. length of item name */
#define MAX_ITEM_DESC   500                             /**@brief Max. length of item description */

/**
 * @name Arguments description structure
 * @{
 **/
 
 /**
 *@brief Argument item description structure used for defining input arguments
 */
typedef struct arg_item_desc_s
{
    int32_t     id;                                     /**<@brief Argument ID */                

    char        name[2][MAX_ITEM_NAME];                 /**<@brief Argument name */

    item_type_e type;                                   /**<@brief Argument type */
    int32_t     value;                                  /**<@brief Argument value */

    char        desc[MAX_ITEM_DESC];                    /**<@brief Argument description */

} arg_item_desc_t;
/**@}*/

/**
 * @name Arguments item structure
 * @{
 **/
 
 /**
 *@brief Argument item structure
 */
typedef struct arg_item_s
{
    int32_t     id;                                     /**<@brief Argument ID */            
    int32_t     mandatory;                              /**<@brief Mandatory flag */    
    void *      dst;                                    /**<@brief Destination pointer */

} arg_item_t;
/**@}*/

class ArgItem : public arg_item_t
{
public:
    ArgItem(ArgItem&&) = default;
    ArgItem(const ArgItem&) = delete;
    ArgItem() = delete;
    ArgItem(int32_t id, int32_t mandatory, void* dst)
    {
        this->id = id;
        this->mandatory = mandatory;
        this->dst = dst;
    }
};

class ArgItemDescription : public arg_item_desc_t
{
public:
    ArgItemDescription(ArgItemDescription&&) = default;
    ArgItemDescription(const ArgItemDescription& other): ArgItemDescription(other.id, other.name[0], other.name[1], other.type, other.value, other.desc)
    {
    }
    ArgItemDescription() = delete;
    ArgItemDescription(int32_t id, char const* short_name, char const* full_name, item_type_e type, int32_t value, char const* desc)
    {
        this->id = id;
        memcpy(name[0], short_name, std::min<size_t>(MAX_ITEM_NAME, strlen(short_name) + 1));
        memcpy(name[1], full_name, std::min<size_t>(MAX_ITEM_NAME, strlen(full_name) + 1));
        memcpy(this->desc, desc, std::min<size_t>(MAX_ITEM_DESC, strlen(desc) + 1));
        this->type = type;
        this->value = value;
    }

    ArgItemDescription(int32_t id, char const* short_name, item_type_e type, int32_t value, char const* desc)
        : ArgItemDescription(id, short_name, "", type, value, desc)
    {
    }
};

/**
 * @name Common arguments defines 
 * @brief Used for common arguments definition
 * @{
 **/
#define IDS_VIDEO_FILE      0x00                        /**<@brief Video file @hideinitializer */
#define IDS_AUDIO_FILE      0x01                        /**<@brief Audio file @hideinitializer */
#define IDS_INPUT_FILE      0x02                        /**<@brief Input file @hideinitializer */
#define IDS_OUTPUT_FILE     0x03                        /**<@brief Output file @hideinitializer */
#define IDI_BITRATE         0x04                        /**<@brief Bitrate define @hideinitializer */
#define IDI_BITS_PER_SAMPLE 0x05                        /**<@brief Bits per sample define @hideinitializer */
#define IDS_CONFIG_FILE     0x06                        /**<@brief Config file @hideinitializer */
#define IDS_CUSTOM_ARG      0x07                        /**<@brief Define for custom argument @hideinitializer */
#define IDS_OUTPUT_FILE_VID 0x08                        /**<@brief Video output file @hideinitializer */
#define IDS_OUTPUT_FILE_AUD 0x09                        /**<@brief Audio output file @hideinitializer */
#define IDI_OUTPUT_BITS_PER_SAMPLE 0x10                 /**<@brief Output bits per sample define @hideinitializer */
#define IDI_PRESET          0x11                        /**<@brief Preset define @hideinitializer */
#define IDS_REC_FILE        0x12                        /**<@brief File for reconstructed frames @hideinitializer */
#define IDS_SERIAL_NUMBER   0x13                        /**<@brief Serial number @hideinitializer */
#define IDS_OPS_FILE        0x14                        /**<@brief Operations list file @hideinitializer */
#define IDI_NUM_THREADS     0x15                        /**<@brief Number of threads @hideinitializer */
#define IDS_ORG_FILE        0x16                        /**<@brief File for source frames @hideinitializer */
#define IDN_QUIET           0x17                        /**<@brief quiet mode @hideinitializer */
#define IDS_STAT_FILE       0x18                        /**<@brief File for statistic @hideinitializer */
#define IDS_EXT_MEM_ALLOC   0x19                        /**<@brief External memory allocator @hideinitializer */
#define IDI_TEMPORAL_ID     0x1A                        /**<@brief Temporal ID @hideinitializer */
#define IDN_HEADERS         0x665                       /**<@brief Print stream headers mode @hideinitializer */
#define IDS_LOG_FILE        0x666                       /**<@brief File name of output log @hideinitializer */
/**@}*/

/**
 * @name Video arguments defines 
 * @brief Used for video arguments definition
 * @{
 **/
#define IDI_V_WIDTH         0x20                        /**<@brief Video width @hideinitializer */
#define IDI_V_HEIGHT        0x21                        /**<@brief Video height @hideinitializer */
#define IDN_V_FOURCC        0x22                        /**<@brief Video colorspace (FOURCC) @hideinitializer */
#define IDD_V_FRAMERATE     0x23                        /**<@brief Video framerate @hideinitializer */
#define IDI_V_PERFORMANCE   0x24                        /**<@brief Video performance @hideinitializer */
#define IDN_V_INTERLACED    0x25                        /**<@brief Video interlaced @hideinitializer */
#define IDI_V_LINE_SIZE     0x26                        /**<@brief Video line size @hideinitializer */
#define IDI_V_NUM_VIEWS     0x27                        /**<@brief Video number of views @hideinitializer */
#define IDN_V_COLOR_MATRIX  0x28                        /**<@brief Video color matrix @hideinitializer */
#define IDI_V_NUM_LAYERS    0x29                        /**<@brief Video number of layers @hideinitializer */
#define IDN_V_SNR_SCALABILITY 0x30                      /**<@brief Video SNR scalability @hideinitializer */
#define IDI_V_SR_MODE         0x31                      /**<@brief Video smart rendering mode @hideinitializer */
#define IDI_V_MULTIPASS_MODE  0x32                      /**<@brief Video multi pass mode @hideinitializer */
#define IDI_V_PAL           0x33                        /**<@brief Video PAL @hideinitializer */
#define IDI_V_START_FRAME   0x34                        /**<@brief Video start frame for processing @hideinitializer */
#define IDI_V_DURATION      0x35                        /**<@brief Video a number of frames for processing @hideinitializer */
#define IDN_V_SWITCH_I      0x36                        /**<@brief Video allow SR <-> non SR switching at I frame(not IDR only) @hideinitializer */
#define IDN_V_PROGRESS      0x37                        /**<@brief Video show progress @hideinitializer */
#define IDN_V_MD5           0x38                        /**<@brief Video calculate MD5 */
#define IDN_V_MD5L          0x39                        /**<@brief Video calculate MD5 (lowercase) */
#define IDN_V_SMP           0x40                        /**<@brief Select video decoder's MT mode */
#define IDN_V_MD5_FRAME     0x41                        /**<@brief Check frame MD5 */
#define IDI_V_FRAMECHANGE   0x42                        /**<@brief Number of frame from which update settings happens*/
/**@}*/

/**
 * @name Audio arguments defines 
 * @brief Used for audio arguments definition
 * @{
 **/
#define ID_A_SAMPLERATE         0x40                    /**<@brief Audio samplerate @hideinitializer */
#define ID_A_CHANNELS           0x41                    /**<@brief Audio number of chanels @hideinitializer */
#define ID_A_OUTPUT_SAMPLERATE  0x42                    /**<@brief Audio output samplerate @hideinitializer */
#define ID_A_OUTPUT_CHANNELS    0x43                    /**<@brief Audio number of output chanels @hideinitializer */
#define ID_A_PRIMING_CUT        0x44                    /**<@brief Audio delay duration (PTS) of priming samples @hideinitializer */
#define ID_A_ENCODER_IDX        0x45                    /**<@brief Index of audio encoder, which was used for creating AAC stream @hideinitializer */
#define ID_A_PLAYBACK_DUR       0x46                    /**<@brief Audio playable duration in PTS @hideinitializer */
#define ID_A_RESTART_INTERVAL   0x47                    /**<@brief Audio restart interval in PTS @hideinitializer */
#define ID_A_OBJECT_TYPE        0x48                    /**<@brief Audio object type @hideinitializer */
/**@}*/

/**
 * @name Container arguments defines 
 * @brief Used for container arguments definition
 * @{
 **/
#define ID_C_TYPE               0x60                        /**<@brief PS or TS @hideinitializer */
#define IDI_C_TITLE_ID          0x61                        /**<@brief Container title ID @hideinitializer */
#define IDI_C_STREAM_ID         0x62                        /**<@brief Container stream ID @hideinitializer */
#define IDI_C_MPD_PROFILE       0x63                        /**<@brief Container MPD profile @hideinitializer */
#define IDS_C_DMF_PROFILE       0x64                        /**<@brief Container MPD profile @hideinitializer */
#define IDS_C_MP2_PCR_OFFSET    0x65                        /**<@brief Container MP2 SCR/PCR offset*/
#define IDS_C_MP2_VBR_MUX       0x66                        /**<@brief Container MP2 set VBR*/
/**@}*/

/**
 * @name Network arguments defines 
 * @brief Used for network arguments definition
 * @{
 **/
#define IDS_NET_IP          0x80                        /**<@brief Network IP @hideinitializer */
#define IDI_NET_PORT        0x81                        /**<@brief Network port @hideinitializer */
#define IDS_NET_URI         0x82                        /**<@brief Network URI @hideinitializer */
/**@}*/

/**
 * @name Network arguments defines 
 * @brief Used for network arguments definition
 * @{
 **/
#define IDI_3D_NUM_VIEWS    0x90                        /**<@brief 3D number of views @hideinitializer */
#define IDI_3D_SPLIT_VIEWS  0x91                        /**<@brief 3D split views @hideinitializer */
#define IDI_TRANSMISSION_FORMAT  0x92                   /**<@brief 3D split views @hideinitializer */
/**@}*/

/**
 * @name Mode arguments defines 
 * @brief Used for mode arguments definition
 * @{
 **/
#define IDI_MODE               0x100                    /**<@brief Mode @hideinitializer */                            
/**@}*/

#define IDI_GUI_MODE            0x101                   /**<@brief GUI mode @hideinitializer */                            
#define IDI_SILENT_MODE         0x102 
#define IDI_ERRCONCEAL_MODE     0x102 
#define IDI_NOMUX               0x110                   /**<@brief No muxing @hideinitializer */

//hwacc
#define IDI_HWACC_MODE	        0x120

//decoders supported hardware acceleration
#define IDI_MC_HWACC_DECODER	0x130

#define IDN_LOOPPLAY            0x160                   /**<@brief Loop play mode @hideinitializer */
#define IDS_BINDIR_PATH         0x170                   /**<@brief Bin dir path @hideinitializer */
#define IDS_LIC_KEYGEN          0x171                   /**<@brief License key gen @hideinitializer */
#define IDS_LIC_FILE            0x172                   /**<@brief License file @hideinitializer */
#define IDS_LIC_DBGLEVEL        0x173                   /**<@brief License debug level @hideinitializer */
#define IDN_LIC_MAKEALL         0x174                   /**<@brief License make all @hideinitializer */
#define IDN_LIC_KEYSIZE         0x175                   /**<@brief License make all @hideinitializer */

// MainConcept DRM in DASH (MPD)
#define IDN_PROTECTION_ENABLE            0x180          /**<@brief MainConcept DRM in DASH (MPD) : Enable Content Protection @hideinitializer */
#define IDS_PROTECTION_SCHEME            0x181          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection Scheme ID @hideinitializer */
#define IDS_PROTECTION_LICENSE           0x182          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection License URL @hideinitializer */
#define IDS_PROTECTION_SESSION           0x183          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection Session URL @hideinitializer */
#define IDS_PROTECTION_UPDATE            0x184          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection Update URL @hideinitializer */
#define IDS_PROTECTION_CONFIG            0x185          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection Config URL @hideinitializer */
#define IDS_PROTECTION_NAMESPACE         0x186          /**<@brief MainConcept DRM in DASH (MPD) : Content Protection Namespace @hideinitializer */

// MainConcept DRM in DASH (MP4)
#define IDS_PROTECTION_BASE_LOCATION     0x190          /**<@brief MainConcept DRM in DASH (MP4) : Content Protection Base Location @hideinitializer */
#define IDS_PROTECTION_PURCHASE_LOCATION 0x191          /**<@brief MainConcept DRM in DASH (MP4) : Content Protection Purchase Location @hideinitializer */
#define IDS_PROTECTION_IV                0x192          /**<@brief MainConcept DRM in DASH (MP4) : Content Protection Initialization Vector, 8 or 16 bytes @hideinitializer */
#define IDS_PROTECTION_KID               0x193          /**<@brief MainConcept DRM in DASH (MP4) : Content Protection Key ID, 16 bytes @hideinitializer */
#define IDS_PROTECTION_KEY               0x194          /**<@brief MainConcept DRM in DASH (MP4) : Content Protection Key, 16 bytes @hideinitializer */

#define IDI_SINGLE_THREAD_MODE           0x195          /**<@brief MPEG-2 muxer : single-thread mode (no locks) @hideinitializer */

// start ID for the custom component's sample-specific IDs
#define IDC_CUSTOM_START_ID     0x1000

/**
 * @brief Parse input arguments
 * @param[in] argc            - Count if input arguments
 * @param[in] argv            - Pointer to input arguments massive
 * @param[in] params_count    - Count of descriptions
 * @param[in] params          - Pointer to arg_item_t structure    
 * @param[in] custom_args     - (opt) Pointer to @ref arg_item_desc_t array for the sample-specific command line parameters of the custom component
 * @param[in] custom_args_count- (opt) Number of items in @ref custom_args
 * @return  -1 if there are only extra arguments
            -2 if there are errors, otherwise 0
 */
int32_t parse_args(int32_t argc, char * argv[], int32_t params_count, const arg_item_t * params, const arg_item_desc_t *custom_args = NULL, int32_t custom_args_count = 0);


/**
 * @brief Parse input arguments
 * @param[in] argc            - Count if input arguments
 * @param[in] argv            - Pointer to input arguments massive
 * @param[in] params          - Vector of the @ref arg_item_t structure    
 * @param[in] custom_params   - (opt) Pointer to @ref arg_item_desc_t array for the sample-specific command line parameters of the custom component
 * @return  -1 if there are only extra arguments
            -2 if there are errors, otherwise 0
 */
int32_t parse_args(int32_t argc, char * argv[], const std::vector<arg_item_t>& params, const std::vector<arg_item_desc_t> &custom_params = {});

/**
 * @brief Parse input arguments
 * @warning  The meaning of parameters argc/argv is different from parse_args function.
 * @param[in] argc            - argc from main 
 * @param[in] argv            - argv from main
 * @param[in] params          - Vector of the @ref arg_item_t structure    
 * @param[in] custom_params   - (opt) Pointer to @ref arg_item_desc_t array for the sample-specific command line parameters of the custom component
 * @return  -1 if there are only extra arguments
            -2 if there are errors, otherwise 0
 */
int32_t parse_program_options(int32_t argc, char * argv[], const std::vector<arg_item_t>& params, const std::vector<arg_item_desc_t> &custom_params = {});
void waitForAnyKey( const char* text );

typedef struct {
    const char *name;
    void *place;
    item_type_e type;	
    double def_val;
    int param_limit;
    double min_limit;
    double max_limit;
} mapparam_t;

const size_t CONFIG_STRING_TYPE_MAX_LEN = 255;

int parse_config_params(char *buf, int len, mapparam_t* param_map, size_t param_num);

void print_help(const arg_item_s *params, int32_t params_cnt,
    const arg_item_desc_t *custom_args /* = NULL */, int32_t custom_args_cnt /* = 0 */);

void print_help(const std::vector<arg_item_s> &params,
    const std::vector<arg_item_desc_t>& custom_params = {});

#endif
