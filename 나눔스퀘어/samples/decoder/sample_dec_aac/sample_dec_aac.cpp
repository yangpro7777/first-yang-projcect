// what2ever.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#include "mctypes.h"
#include "bufstrm.h"
#include "mcdefs.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "dec_aac.h"

#define INPUT_BUFF_SIZE         (2048*10)

int get_bitstream_format_from_string(const char * stream_format);

int main(int argc, char * argv[])
{
    char * in_file;
    char * out_file;
    char * stream_format = NULL;

    int32_t sample_rate = -1;
    int32_t num_channels = -1;
    int32_t priming = -1;
    int32_t encoder_type = -1;
    int64_t playback = -1;
    int32_t lp_sbr = -1;

	wav_hdr_param wav_hdr = {0};
	int wav_flag = 0;

    constexpr int32_t ID_LP_SBR = IDC_CUSTOM_START_ID + 1;

    const std::vector<arg_item_t> params = {
        {IDS_INPUT_FILE, 1, &in_file},        //
        {IDS_OUTPUT_FILE, 1, &out_file},      //
        {ID_A_SAMPLERATE, 0, &sample_rate},   //
        {ID_A_CHANNELS, 0, &num_channels},    //
        {IDS_CUSTOM_ARG, 0, &stream_format},  //
        {ID_A_PRIMING_CUT, 0, &priming},      //
        {ID_A_ENCODER_IDX, 0, &encoder_type}, //
        {ID_A_PLAYBACK_DUR, 0, &playback},    //
        {ID_LP_SBR, 0, &lp_sbr}               //
    };

    const std::vector<arg_item_desc_t> custom_params{arg_item_desc_t{ID_LP_SBR, {"lp_sbr", ""}, ItemTypeInt, 0, "Enable (1) or disable (0) the Low Power SBR"}};

    if (parse_program_options(argc, argv, params, custom_params) < 0) {
        printf("\n==== MainConcept AAC decoder sample ====\n"
               "Usage:\nsample_dec_aac.exe -i audio.aac -o audio.pcm -s 48000 -ch 2 [-ADTS | -ADIF | -RAW | -LOAS | -LATM] [-priming n ...] [-encoder_idx idx] "
               "[-playback n] [-lp_sbr n]\n\n"
               "Options:\n"
               "-s\tsample rate\n"
               "-ch\tnumber of channels\n"
               "-priming\tpriming duration in PTS\n"
               "-encoder_idx\tencoder index (0 - MC, 1 - Fraunhofer)\n"
               "-lp_sbr\tenable (1) or disable (0) the Low Power SBR. Enabled by default.\n");
        return 0;
    }

    FILE * fp_in  = fopen(in_file, "rb");
    if (!fp_in)
    {
        printf("Error opening input file.\n");
        return 1;
    }

    FILE * fp_out = fopen(out_file, "wb");
    if (!fp_out)
    {
		fclose(fp_in);
        printf("Error opening output file.\n");
        return 1;        
    }

	char *s;
	if (((s = strstr(out_file, ".wav")) || 
		(s = strstr(out_file, ".WAV"))) && strlen(s) == 4)
	{
		wav_flag = 1;
	}

    const int MAX_CHAN = 8;
    const int OUTPUT_BUFF_SIZE = MAX_CHAN * 2048 * sizeof(short);

    unsigned char * mem_area = new unsigned char[INPUT_BUFF_SIZE + OUTPUT_BUFF_SIZE];

    unsigned char * in_buf = mem_area;
    unsigned char * out_buf = mem_area + INPUT_BUFF_SIZE;

    aud_bfr_tt dst_bfr;
    callbacks_t callbacks;
    init_callbacks(callbacks);
    bufstream_tt * pdec = createDecoderAAC(&callbacks, 0, 0);

    if (!pdec)
    {
		fclose(fp_in);
		fclose(fp_out);
        printf("open_AACin_Audio_stream_ex() failed\n");
        return 1;
    }

    aac_decoder_config dec_config;
    // fill config up with -1, this value is used to mark non-change parameters
    memset(&dec_config, AAD_PARAM_DONT_CHANGE, sizeof(dec_config));

	if (num_channels != ITEM_NOT_INIT)
		dec_config.num_channels = num_channels;

	if (sample_rate != ITEM_NOT_INIT)
		dec_config.sampling_rate = sample_rate;
    
    dec_config.bitstream_format = (stream_format != NULL) ? get_bitstream_format_from_string(stream_format + 1) : -1; // skip leading '-' for stream_format
    dec_config.output_format    = AAD_DSF_16LE;
    dec_config.decode_he        = 1;
    if (priming != ITEM_NOT_INIT)
        dec_config.priming_dur = priming;

    if( encoder_type == AAD_ENCODER_DEFAULT ||
        encoder_type == AAD_ENCODER_MC      ||
        encoder_type == AAD_ENCODER_FHG)
    {
        dec_config.encoder_type = encoder_type;
    }

    if (playback > 0)
        dec_config.playback_dur = playback;

    if (lp_sbr >= 0)
        dec_config.disable_low_power_sbr = !lp_sbr;

    dst_bfr.bfr = 0;
    dst_bfr.bfr_size = 0;
    int bytes_read = 0;
    int bytes_decoded = 0;
    int frames_decoded = 0;
    int ret = 0;
    if (dec_config.bitstream_format != AAD_BSF_RAW)
    {
        pdec->auxinfo(pdec, 0, PARSE_AUD_HDR, NULL, 0);
        bytes_read = (int)fread(in_buf, 1, INPUT_BUFF_SIZE, fp_in);    
        ret = pdec->copybytes(pdec, in_buf, bytes_read);

        if (ret == 0 && (num_channels <= 0 || sample_rate <= 0))
        {
            printf("Header is not found. The file can't be decoded.\n");
            ret = -1;
        }
    }
    else if (num_channels <= 0 || sample_rate <= 0)
    {
        printf("Sample rate (-s) and number of channels (-ch) shall be defined for the RAW AAC file.\n");
        ret = -1;
    }

    if (ret >= 0 && (ret = pdec->auxinfo(pdec, 0, INIT_FRAME_PARSER, &dec_config, sizeof(dec_config))))
    {
        printf("Init failed\n");
    }

    if (ret < 0)
    {
        close_bufstream(pdec, 0);
        fclose(fp_in);
        fclose(fp_out);
        return 1;
    }

    while (1)
    {
        if (bytes_decoded >= bytes_read && dst_bfr.bfr_size <= 0)
        {
            bytes_read = (int)fread(in_buf, 1, INPUT_BUFF_SIZE, fp_in);    
            bytes_decoded = 0;

            if (bytes_read == 0)	//EOF
                break;
        }

        dst_bfr.bfr = out_buf;
        dst_bfr.bfr_size = OUTPUT_BUFF_SIZE;
        pdec->auxinfo(pdec, 0, PARSE_FRAMES, &dst_bfr, sizeof(dst_bfr));

        int frame_size = pdec->copybytes(pdec, in_buf + bytes_decoded, bytes_read - bytes_decoded);
        int state = pdec->auxinfo(pdec, 0, GET_PARSE_STATE, 0, 0);
        bytes_decoded += frame_size;

        if (dst_bfr.bfr_size > 0 && (state & PARSE_DONE_FLAG))
        {
            aac_decoded_frame_info * frame_info;
            pdec->auxinfo(pdec, 0, GET_PIC_PARAMSP, &frame_info, sizeof(*frame_info));
            if (wav_flag && frames_decoded == 0)
            {
                wav_hdr.bits_per_sample = 16;
                wav_hdr.sample_rate = frame_info->sampling_rate;
                wav_hdr.num_channels = frame_info->num_channels;
                wav_hdr.block_align = (16 >> 3) * frame_info->num_channels;
                wav_hdr.bytes_per_sec = frame_info->sampling_rate * wav_hdr.block_align;
                wav_hdr.data_size = 0;
                wav_header_write(fp_out, &wav_hdr);
            }
            printf("\r[frame %05d] %dHz %dCh HE=%d", frames_decoded++, frame_info->sampling_rate, frame_info->num_channels, frame_info->he);

		    if (wav_flag)
			    wav_data_write(fp_out, &wav_hdr, dst_bfr.bfr, dst_bfr.bfr_size);
		    else
		    // Write provided pcm samples out to file
                fwrite(dst_bfr.bfr, dst_bfr.bfr_size, 1, fp_out);
        }
    }

	if (wav_flag)
		wav_header_write(fp_out, &wav_hdr);

	if (pdec)
		close_bufstream(pdec, 0);

    if (fp_in)
        fclose(fp_in);

    if (fp_out)
        fclose(fp_out);

    if(mem_area)
        delete [] mem_area;

    return 0;
}

#if defined _MSC_VER
#define strcmp_lower_case _stricmp
#else
#define strcmp_lower_case strcasecmp
#endif

int get_bitstream_format_from_string(const char * stream_format)
{
    if (strcmp_lower_case(stream_format, "adts") == 0) return AAD_BSF_ADTS;
    if (strcmp_lower_case(stream_format, "adif") == 0) return AAD_BSF_ADIF;
    if (strcmp_lower_case(stream_format, "raw")  == 0) return AAD_BSF_RAW;
    if (strcmp_lower_case(stream_format, "loas") == 0) return AAD_BSF_LOAS;
    if (strcmp_lower_case(stream_format, "latm") == 0) return AAD_BSF_LATM;

    return -1;
}

