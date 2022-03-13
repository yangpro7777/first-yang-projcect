#ifndef UUID_16EBDD36_E066_4A66_B2EF_51FDBF3CE938
#define UUID_16EBDD36_E066_4A66_B2EF_51FDBF3CE938

#ifndef __APPLE__
#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif
#endif

typedef struct mc_video_dec_features_s
{
    int struct_size;
    /* major features */
    int smp_modes;
    int max_num_threads;
    int I_stream;
    int IP_stream;
    int reserved1[8];
    int hwacc_modes;
    int reserved2[40];
    int mcvesa;
    int reserved3[40];
    int loop_filter;
    int postprocessing_filter;
    int reserved4[100];
    /* minor features */
    int brightness_control;
    int contrast_control;
    int error_concealment;
    int reserved5[10];
    int error_resilience;
    int transform_precision;
    int mv_precision;
    int low_latency;
    int osd;
    int get_pic_delayed;
    int chroma_upsampling;
    int deinterlacing_modes;
    int ignore_cropping;
    int reserved6[100];
    /* obsolete features */
    int half_resolution;
    int double_rate;
    int reserved7[100];
} mc_video_dec_features_t;

////////////////////////////////
//

/** \brief Group of pictures header structure.

 ISO/IEC 13818-2 section 6.2.2.6 Group of pictures header
 */
struct GOP_Params
{
    int drop_flag; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures
                      header */
    int hour; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures header
               */
    int minute; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures
                   header */
    int sec;   /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures header
                */
    int frame; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures header
                */
    int closed_gop;  /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures
                        header */
    int broken_link; /**< Corresponds to the MPEG-1/2 syntax element of the same name and is described in the ISO/IEC 13818-2 section 6.2.2.6 Group of pictures
                        header */
};

// maximum CableLabs video HD bitrate
#define MAX_CABLELABS_VIDEO_HD_BITRATE  18100000

// maximum CableLabs video SD bitrate
#define MAX_CABLELABS_VIDEO_SD_BITRATE   3180000

// maximum HDV HD2 video bitrate
#define MAX_HDV_HD2_VIDEO_BITRATE       25000000

// maximum HDV HD1 PAL video bitrate
#define MAX_HDV_HD1_PAL_VIDEO_BITRATE   19850000

/**
 * @name Extended option defines (obsolete)
 * @brief for opt_flags parameter legacy components.
 * @{
 **/

#define OPT_INSERT_FRAME (0x00000100L) /**<@brief @brief Pad skipped frame */
/** @} */

/* maximum SVCD video bitrate, Note, this assumes no audio! */
/* If audio is used, subtract the audio bitrate from this   */
/* number, i.e. if the audio is 224 kbps, then the video    */
/* bitrate would be 2422848 bps (2646848 - 224000)          */
#define MAX_SVCD_VIDEO_BITRATE           2646848

/* maximum DVD video bitrate */
#define MAX_DVD_VIDEO_BITRATE            9800000

// maximum MicroMV video bitrate
#define MAX_MMV_VIDEO_BITRATE           12000000

// maximum ATSC video bitrate (normal)
#define MAX_ATSC_VIDEO_BITRATE          19400000

/** \addtogroup video_dec_mpeg_group
\{
*/
/** \brief  subpic frame info

The sp_frame_tt structure is used to inform the decoder where to put decoded subpicture data.

 The subpic is an 8bpp lut.
 The bottom 4 bits are the lut entry into the subpic palette.
 Top 4 bits are the contrast ratio k, where:mpegOutErrNone
 \code
    if (k > 0)
    {
      k += 1
      source pixel = ((16 - k) / 16) * source pixel + (k / 16) * subpic pixel
    }
    else
      source pixel = source pixel
 \endcode
*/
typedef struct
{
    unsigned long width;     /**< \brief  width of subpic  */
    unsigned long height;    /**< \brief  heigth of subpic  */
    unsigned char *plane;    /**< \brief  subpic buffer, must be large enough to handle a subpic as large as the main picture  */
    long stride;             /**< \brief  subpic stride  */
    // destination coordinates: 0,0 = upper left of picture
    long dest_x;             /**< \brief  destination x coordinate of subpic in main pic  */
    long dest_y;             /**< \brief  destination y coordinate of subpic in main pic  */
    unsigned long reserved0;
    unsigned long reserved1;
    unsigned long reserved2;
    unsigned long reserved3;
} sp_frame_tt; // only for dec_spic enc_mp2v

// earlier declaration was #define OPT_EXT_PARAM_FRM_ORG           (0x00001000L)  //show original frame
// changed due to conflict with INIT_OPT_CHAPTER_LIST
#define OPT_EXT_PARAM_FRM_ENC           (0x00002000L)   /**< @brief Show encoded frame @hideinitializer */

/**
 * @brief User data passed by the application.
 *
 * Used to pass data from the application to the encoder.
 * Data may be passed for the entire stream and/or individual frames and also may be of different types:
 * @li data embedded into the output stream;
 * @li metadata for the encoder, used during frames processing;
 * @li auxiliary data, passed by the application with frames and returned back with compressed frames for their further processing at application side. The application can attach to the frame many data including different types immediately.
 *
 * The structure describes the data type, the size in bytes and points to data. Also, there is an usage counter, which allows the application to track the usage of previously sent data and to reuse it when released by encoder.
 * The encoder does not copy received data but uses it exactly as much as necessary. The application is responsible for managing user data: allocation, initialization, re-usage and finally destroying.
 */
typedef struct enc_user_data
{
    uint32_t        type;         /**<@brief  a type ID. Available values are listed in @ref ud_type_t enum. */
    uint32_t        size;         /**<@brief  a size in bytes */
    void           *data;         /**<@brief  a pointer to a memory buffer */
    uint32_t        lock;         /**<@brief  a usage counter */
    int32_t         flags;
    uint32_t        field;        /**<@brief  a field to place user data in case of interlaced video . Available values are listed in @ref ud_field_t enum. */
    uint32_t        reserved[14];
} enc_user_data_tt;

enum {
    SEQ_EXT_HDR_FLAG = 0x00020000
}; /**<  elements of MPEG syntax */


#ifndef __APPLE__
#pragma pack(pop)
#endif

/**
 * @brief Extended option passed from the application level to @ref hevcOutVideoInit, @ref hevcOutVideoPutFrame, @ref hevcOutVideoPutFrameV, @ref h264OutVideoInit, @ref h264OutVideoPutFrame
 * @{
 **/
typedef struct
{
    ext_opt_id option_id;     /**<@brief option ID */
    void*      option_data;   /**<@brief a pointer to an option data, context specific */
} enc_option_t;
/** @} */

#define OPT_EXT_PARAM_TIMESTAMPS        (0x10000000L)   /**< @brief Specify PTS for the frame @hideinitializer */


#define MAX_DEV_TITLE_SIZE                                 160     /**< @brief  Maximum device title size  @hideinitializer*/

/**
* @brief Video encoder acceleration configuration.
* The structure is used together with @ref hevcOutVideoEnumAcceleration and @ref h264OutVideoEnumAcceleration.
*/
struct enc_acceleration_info
{
    char          title[MAX_DEV_TITLE_SIZE];            /**<@brief  Friendly device name */

    int32_t       acceleration;                         /**<@brief  HW acceleration type, corresponds to @ref hevc_v_settings::hw_acceleration and @ref h264_v_settings::hw_acceleration */
    int32_t       mode;                                 /**<@brief  HW acceleration mode, corresponds to @ref hevc_v_settings::hw_acc_mode, should be 0 for AVC/H.264 encoder */
    int32_t       device_idx;                           /**<@brief  Device ordinal, corresponds to @ref hevc_v_layer::hw_acc_indices and @ref h264_v_settings::hw_acc_idx */
    int32_t       reserved[4];                          /**<@brief  Reserved */
};

#endif
