 /** 
\file  mcvesa.h
\brief  VES analyzer declarations

\verbatim
File: mcvesa.h
Desc: VES analyzer declarations

 Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 
 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 This software is protected by copyright law and international treaties.  Unauthorized 
 reproduction or distribution of any portion is prohibited by law.

\endverbatim
**/

#if !defined (__VES_ANALYSIS_H__)
#define __VES_ANALYSIS_H__

/** \brief VESA settings */
typedef struct VESA_settings_s {
	union {
		struct {
			uint32_t	pic_level_analysis_enable_flag		:1;
			uint32_t	pic_level_analysis_reserved_flags	:31;
		};
		uint32_t	pic_level_analysis_flags;
	};

	union {
		struct {
			uint32_t	slice_level_analysis_enable_flag	:1;
			uint32_t	slice_level_analysis_reserved_flags	:31;
		};
		uint32_t	slice_level_analysis_flags;
	};

	union {
		struct {
			uint32_t	mb_level_analysis_enable_flag				:1;
			uint32_t	mb_level_analysis_quantized_coeffs_flag		:1;
			uint32_t	mb_level_analysis_dequantized_coeffs_flag	:1;
			uint32_t	mb_level_analysis_residual_difference_flag	:1;
			uint32_t	mb_level_analysis_spatial_pred_params_flag	:1;
			uint32_t	mb_level_analysis_temporal_pred_params_flag	:1;
			uint32_t	mb_level_analysis_reserved_flags			:26;
		};
		uint32_t	mb_level_analysis_flags;
	};

	uint32_t	keep_bitstream_flag;
}VESA_settings_t;
/** \} */

/** \brief VESA buffer description */
typedef struct VESA_buffer_description_s {
	uint32_t	buffer_type;
	uint8_t		*data;
	uint32_t	data_size;
	uint32_t	num_units_in_buffer;
	uint32_t	fixed_unit_size_flag;
	uint32_t	unit_size;
	uint32_t	arbitrary_order_flag;
	void		*syntax;
	uint32_t	syntax_size;
} VESA_buffer_description_t;

/** \brief VESA buffers structure */
typedef struct VESA_buffers_s {
    VESA_buffer_description_t	*buffer;
    uint32_t					num_buffers;
}VESA_buffers_t;
/** \} */

//
//    Layout of "buffer type" value.
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---------------+-------+-------+-------------------------------+
//  |    Reserved   | Layer |  Type |              Code             |
//  +---------------+-------+-------+-------------------------------+


/** \name VESA buffer types */
#define	VESA_MAKE_BUFFERTYPE_CODE(layer, type, code)	((layer<<20) | (type<<16) | code)	 /**< \hideinitializer */

#define	VESA_TYPE_CONTROL		0	 /**< \brief Data which describes video elementary stream in VESA API context. \hideinitializer */// 
#define	VESA_TYPE_HEADER		1	 /**< \brief Original headers. \hideinitializer */// 
#define	VESA_TYPE_COMPDATA		2	 /**< \brief Compressed data (raw bitstream).  \hideinitializer */// 
#define	VESA_TYPE_UNCOMDATA		3	 /**< \brief Uncompressed data. \hideinitializer */// 

#define VESA_LAYER_SEQ			0	 /**< \hideinitializer */
#define VESA_LAYER_GOP			1	 /**< \hideinitializer */
#define VESA_LAYER_PIC			2	 /**< \hideinitializer */
#define VESA_LAYER_SLICE		3	 /**< \hideinitializer */
#define VESA_LAYER_MB			4	 /**< \hideinitializer */

/** \defgroup vesa_buffers_group VESA buffers 
\{
*/
// Sequence layer
#define	VESA_BUFFER_SEQ_PARAMS					VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_SEQ,	VESA_TYPE_CONTROL,		0) /**< \hideinitializer */
#define	VESA_BUFFER_GOP_PARAMS					VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_SEQ,	VESA_TYPE_CONTROL,		1) /**< \hideinitializer */

#define	VESA_BUFFER_H264_SEQ_HEADER				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_SEQ,	VESA_TYPE_HEADER,		0) /**< \hideinitializer */
#define	VESA_BUFFER_MPEG2_SEQ_HEADER			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_SEQ,	VESA_TYPE_HEADER,		1) /**< \hideinitializer */
#define	VESA_BUFFER_MPEG2_GOP_HEADER			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_SEQ,	VESA_TYPE_HEADER,		2) /**< \hideinitializer */


// Picture layer
#define	VESA_BUFFER_PIC_PARAMS					VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_CONTROL,		0) /**< \hideinitializer */
#define	VESA_BUFFER_SLICE_PARAMS				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_CONTROL,		1) /**< \hideinitializer */
#define	VESA_BUFFER_QMATRIX_PARAMS				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_CONTROL,		2) /**< \hideinitializer */
#define	VESA_BUFFER_MB_PARAMS					VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_CONTROL,		3) /**< \hideinitializer */

#define	VESA_BUFFER_QMATRIX_COEF				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_UNCOMDATA,	0) /**< \hideinitializer */
#define	VESA_BUFFER_YUV							VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_UNCOMDATA,	1) /**< \hideinitializer */

#define	VESA_BUFFER_H264_PIC_HEADER				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_HEADER,		0) /**< \hideinitializer */
#define	VESA_BUFFER_H264_SLICE_HEADER			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_HEADER,		1) /**< \hideinitializer */

#define	VESA_BUFFER_MPEG2_PIC_HEADER			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_HEADER,		2) /**< \hideinitializer */
#define	VESA_BUFFER_MPEG2_PIC_CODING_EXT_HEADER	VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_PIC,	VESA_TYPE_HEADER,		3) /**< \hideinitializer */

// MB layer
#define	VESA_BUFFER_PRED_BLOCK_PARAMS			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_CONTROL,		0) /**< \hideinitializer */
#define	VESA_BUFFER_CODED_BLOCK_PARAMS			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_CONTROL,		1) /**< \hideinitializer */

#define	VESA_BUFFER_MV							VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	2) /**< \hideinitializer */
#define	VESA_BUFFER_MVD							VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	3) /**< \hideinitializer */
#define	VESA_BUFFER_REFIDX						VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	4) /**< \hideinitializer */
#define	VESA_BUFFER_IPRED						VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	5) /**< \hideinitializer */
#define	VESA_BUFFER_QUANTIZED_COEF				VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	6) /**< \hideinitializer */
#define	VESA_BUFFER_DEQUANTIZED_COEF			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	7) /**< \hideinitializer */
#define	VESA_BUFFER_RESIDUAL_DIFFERENCE			VESA_MAKE_BUFFERTYPE_CODE(VESA_LAYER_MB,	VESA_TYPE_UNCOMDATA,	8) /**< \hideinitializer */
/** \} */
/** \} */

//
// Hierarchy of VES.
//
// Sequence
//  |
//  +--Group of pictures
//      |
//      +--Frame
//          |
//          +--Picture
//          |   |
//          |   +--Qauntization maxtixes
//          |   |
//          |   +--Slices
//          |   |
//          |   +--Macroblock
//          |       |
//          |       +--Predicted block
//          |       |   |
//          |       |   +--Motion vector
//          |       |   |
//          |       |   +--Reference index
//          |       |   |
//          |       |   +--Motion vector difference
//          |       |   |
//          |       |   +--Intra prediction mode
//          |       |
//          |       +--Coded block
//          |           |
//          |           +--Quantized coefficients
//          |           |
//          |           +--Dequantized coefficients
//          |           |
//          |           +--Residual difference
//          |
//          +--YUV
//

/** \brief VESA slice types */
typedef struct VESA_macroblock_params_s {
	uint32_t	mb_nr;								/**< \brief Macroblock address. */
	uint32_t	slice_id;							/**< \brief The number of own slice. */
	uint16_t	pos_x;								/**< \brief Horizontal position of top-left luma sample. In units of luma samples. */
	uint16_t	pos_y;								/**< \brief Vertical position of top-left luma sample. In units of luma samples. */
	union {
		struct {
			uint32_t	field_pred_flag			:1;		/**< \brief Prediction is applied in field manner. */
			uint32_t	field_transform_flag	:1;		/**< \brief Transformation is applied in field manner. */
            uint32_t    transform_size_8x8_flag :1;     /**< \brief Transform mode for makroblocks 1 - 8x8, 0 - 4x4*/
			uint32_t	reserved_flags			:29;
		};
		uint32_t	mb_type;
	};
	uint16_t	qp;									/**< \brief Quantiation parameter; */

	uint8_t		num_pred_blocks;					/**< \brief Number of predcited blocks. */
	uint8_t		num_coded_blocks;					/**< \brief Number of coded blocks. */

	uint32_t	offset_pred_block;					/**< \brief Offset to location of parameters of prediction. In units of 1 byte. */
	uint32_t	offset_coded_block;					/**< \brief Offset to location of coded residual data. In units of 1 byte. */
} VESA_macroblock_params_t;
/** \} */

/** \brief VESA slice types */
typedef struct VESA_pred_block_params_s {
	uint8_t		pos_x;								/**< \brief Horizontal position of top-left sample. In units of component samples. */
	uint8_t		pos_y;								/**< \brief Vertical position of top-left sample. In units of component samples. */
	uint8_t		width;								/**< \brief Width of the block. In units of component samples. */
	uint8_t		height;								/**< \brief Height of the block. In units of component samples. */
	union {
		struct {
			uint32_t	colour_plane_idc	:2;		/**< \brief Specifies the colour plane to which that block belongs. */
			uint32_t	skip_block_flag		:1;		/**< \brief The block is skipped. */
			uint32_t	forward_pred_flag	:1;		/**< \brief The block is predicted from forward reference. */
			uint32_t	backward_pred_flag	:1;		/**< \brief The block is predicted from backward reference. */
			uint32_t	bottom_field_flags	:2;		/**< \brief The block is predicted from bottom field. */
			uint32_t	reserved_flags		:25;
		};
		uint32_t	block_type;
	};
	uint32_t	offset_block_data;					/**< \brief Offset to location of prediction parameters. In units if 1 byte. */
}VESA_pred_block_params_t;
/** \} */

/** \brief VESA slice types */
typedef struct VESA_coded_block_params_s {
	uint8_t		pos_x;								/**< \brief Horizontal position of top-left sample. In units of component samples. */
	uint8_t		pos_y;								/**< \brief Vertical position of top-left sample. In units of component samples. */
	uint8_t		width;								/**< \brief Width of the block. In units of component samples. */
	uint8_t		height;								/**< \brief Height of the block. In units of component samples. */
	union {
		struct {
			uint32_t	colour_plane_idc	:2;		/**< \brief Specifies the colour plane to which that block belongs. */
			uint32_t	reserved_flags		:30;
		};
		uint32_t	block_type;
	};
	uint32_t	offset_block_data;					/**< \brief Offset to location of residual different data. In units of 8 bytes. */
}VESA_coded_block_params_t;
/** \} */

/** \name VESA colour planes */
#define	VESA_COLOUR_PLANE_Y					0
#define	VESA_COLOUR_PLANE_Cb				1
#define	VESA_COLOUR_PLANE_Cr				2
#define	VESA_COLOUR_PLANE_RESERVED			3
/** \} */

/** \brief VESA sequence information */
typedef struct VESA_seq_params_s {
	uint32_t	vertical_size;
	uint32_t	horizontal_size;
	uint32_t	bit_depth_luma;
	uint32_t	bit_depth_chroma;
	uint32_t	chroma_format_idc;
	uint64_t	time_per_frame;		/**< \brief in 100 nsec units */
}VESA_seq_params_t;
/** \} */

/** \name VESA chroma formats */
#define	VESA_CHROMA_FORMAT_400				0
#define	VESA_CHROMA_FORMAT_420				1
#define	VESA_CHROMA_FORMAT_422				2
#define	VESA_CHROMA_FORMAT_444				3
/** \} */

/** \brief VESA dpb information */
typedef	struct VESA_dpb_entry_s {
	uint32_t	sufrace_index;
}VESA_dpb_entry_t;
/** \} */

/** \brief VESA picture information */
typedef struct VESA_pic_params_s {
	VESA_dpb_entry_t	dpb_entry;
	union {
		struct {
			uint32_t	field_pic_flag		:1;
			uint32_t	bottom_field_flag	:1;
			uint32_t	ref_pic_flags		:2;
			uint32_t	long_term_ref_flags	:2;
			uint32_t	q_scale_type		:1;
			uint32_t	reserved_pic_flags	:25;
		};
		uint32_t	pic_type;
	};
	uint32_t			frame_num;
	uint32_t			field_order_cnt[2];
}VESA_pic_params_t;
/** \} */

/** \brief VESA slice information */
typedef struct VESA_slice_params_s {
	uint32_t			slice_id;
	uint32_t			slice_type;
	uint32_t			num_ref_pics[2];
	VESA_pic_params_t	ref_pic[2][32];
}VESA_slice_params_t;
/** \} */

/** \name VESA slice types */
#define VESA_SLICE_TYPE_I	0
#define VESA_SLICE_TYPE_P	1
#define VESA_SLICE_TYPE_B	2
#define VESA_SLICE_TYPE_SP	3
#define VESA_SLICE_TYPE_SI	4
/** \} */

/** \brief VESA qmatrix information*/
typedef struct VESA_qmatrix_params_s {
	uint8_t		width;
	uint8_t		height;
	union {
		struct {
			uint32_t	colour_plane_idc	:2;
			uint32_t	intra_qmatrix_flag	:1;
			uint32_t	reserved_flags		:29;
		};
		uint32_t	qmatrix_type;
	};
	uint32_t	offset_qmatrix_data;
}VESA_qmatrix_params_t;
/** \} */

#endif //#if !defined (__VES_ANALYSIS_H__)