/********************************************************************
 Created: 2007/06/07
 File name: sample_demux_mp2_file.cpp
 Purpose: command-line sample for MPEG-2 demuxer in file mode

 Copyright (c) 2007-2009 MainConcept GmbH. All rights reserved.

 This software is the confidential and proprietary information of
 MainConcept GmbH and may be used only in accordance with the terms of
 your license from MainConcept GmbH.

*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "mctypes.h"
#include "auxinfo.h"
#include "buf_fifo.h"
#include "buf_file.h"
#include "buf_wave.h"
#include "sample_common_args.h"
#include "sample_common_misc.h"
#include "demux_mp2.h"
#include "INIReader.h"


bool USE_MP2_API = true;

#define MAX_OUTPUT 32

typedef struct out_bufs
{
  bufstream_tt *out_bs;
  fifo_stream_tt *out_fifo;
}out_bufs_t;


typedef struct app_vars_s
{
  char *in_filename;
  char *out_filename;
	int32_t tid;
	int32_t sid;
	int32_t parser;
	bufstream_tt *in_bs;
	fifo_stream_tt *in_fifo;
	
  out_bufs_t out_bufs[MAX_OUTPUT];

  //Old API only
  mpgdmux_file_info file_info;
  mpgdmux_stream_info stream_info;
} app_vars_t;

std::vector<mp2dmux_CA_TS_Decrypt_s*> g_decryptors;
struct aes_key_s{
	uint8_t key[16];
	uint8_t iv[16];
	uint8_t iv2[16];
};

typedef std::map<std::pair<int, int>, aes_key_s> key_map;
key_map g_keys;
static
struct mp2dmux_CA_TS_Decrypt_s* get_decryptor(context_t context, mp2dmux_program_info_t* prog, mp2dmux_stream_format_t* stream)
{
    key_map::iterator key = g_keys.find(std::make_pair(-1, stream->elementary_PID));
	if (g_keys.end() == key){
		key = g_keys.find(std::make_pair(prog->program_number, -1));//Default key for program
	}
	if (g_keys.end() == key){
		key = g_keys.find(std::make_pair(-1, -1));//Default key for stream
	}
	if (g_keys.end() != key) {
        aes_key_s &aes_key = key->second;
		struct mp2dmux_CA_TS_Decrypt_s *d =
				mp2DemuxCreateStaticTSDecryptor(CA_TS_MODE_AES128_CBC, CA_TS_BT_SCTE52, aes_key.key, 16, aes_key.iv, 16,
												aes_key.iv2, 16);
		if (d)
			g_decryptors.push_back(d);
		return d;
	}
	return NULL;//We can not decrypt this stream
}


bool read_keys(char *name);

void hex2bin(string basic_string, uint8_t (& key)[16]);

// this function is called by the demuxer when a chunk of elementary stream
// data is ready in the output fifo, the ainfo->length field contains the
// length of the chunk placed in the fifo
static uint32_t stream_auxinfo(bufstream_tt *bs, uint32_t offs, uint32_t info_ID, void *info_ptr, uint32_t info_size)
{
	out_bufs_t *bufs = (out_bufs_t*)bs->drive_ptr;
	dmux_chunk_info *chunk_info;
	bufstream_tt *obs;
	uint8_t *ptr;
	int32_t i;

	if (info_ID == DMUX_CHUNK_INFO)
	{

		if (info_size != sizeof(dmux_chunk_info))
			return BS_ERROR;

		chunk_info = (dmux_chunk_info*)info_ptr;
		obs = &bufs->out_fifo->output;	
		
		ptr = obs->request(obs, chunk_info->length);
		if (!ptr)
		{
			return BS_ERROR;
		}

		i = bufs->out_bs->copybytes(bufs->out_bs, ptr, chunk_info->length);
		if (i != chunk_info->length)
		{
			return BS_ERROR;
		}

		obs->confirm(obs, chunk_info->length);

		// do this in case open_wave_buf_write was used, it needs these
		// messages to work correctly
		i = bufs->out_bs->auxinfo(bufs->out_bs, offs, info_ID, info_ptr, info_size);
		if (i != BS_OK)
		{
			return i;
		}

		return BS_OK;
	}

	return BS_OK;
}

// stream detection will be limited to the chunk size
// so it must be large enough to detect all the relevant streams
#define CHUNK_SIZE	4*1024 * 1024
#define FIFO_SIZE	CHUNK_SIZE * 4


static void add_file_extension(char * filename, mcmediatypes_t stream_mediatype)
{
  switch (stream_mediatype)
  {
  case mctMPEG1V: strcat(filename, ".m1v"); break;
  case mctMPEG2V: strcat(filename, ".m2v"); break;
  case mctMPEG4V: strcat(filename, ".m4v"); break;
  case mctH263:   strcat(filename, ".h263"); break; 
  case mctH264:   strcat(filename, ".avc"); break; 
  case mctMVCSub: strcat(filename, ".mvc_sub"); break;
  case mctMVCMux: strcat(filename, ".mvc"); break;
  case mctHEVC:   strcat(filename, ".hevc"); break;
  case mctVC1:    strcat(filename, ".vc1"); break;
  case mctMPEG1A:
  case mctMPEG2A:
    strcat(filename, ".mpa"); break;
  case mctAAC_Generic:
  case mctAAC_RAW:
  case mctAAC_LATM:
  case mctAAC_ADTS: 
    strcat(filename, ".aac"); break;
  case mctAC3:
  case mctAC3_DDPlus:
    strcat(filename, ".ac3"); break;
  case mctAES3_302M:
  case mctDVD_LPCM:
  case mctHDMV_LPCM:
    strcat(filename, ".wav"); break;
  case mctDTS:
    strcat(filename, ".dts"); break;
  default:
    strcat(filename, ".dat"); break;
  }
}

static void display_stream_info_new_api(mp2dmux_tt *demuxer, mp2dmux_file_info_t file_info)
{
  mp2dmux_program_info_t program_info;
  mp2dmux_stream_format_t stream_info;


  printf("Number of titles: %d\n\n", file_info.program_count);

  for (int i = 0; i < file_info.program_count; i++)
  {
    // get info about the program (title)
    if (mp2DemuxGetProgramInfo(demuxer, &program_info, i))
      return;

    printf("  Title: %d\n", i);

    for (int j = 0; j < program_info.stream_count; j++)
    {
      char str[256];

      // get info about the stream
      if (mp2DemuxGetStreamFormat(demuxer, &stream_info, i, j))
        return;

      sprintf(str, "    Stream: %d, ", j);

      switch(stream_info.format.stream_mediatype)
      {
      case mctMPEG1V: strcat(str, "MPEG-1 video"); break;
      case mctMPEG2V: strcat(str, "MPEG-2 video"); break;
      case mctMPEG4V: strcat(str, "MPEG-4 video"); break;
      case mctH263:   strcat(str, "H263 video"); break;
      case mctH264:   strcat(str, "AVC video"); break;
      case mctMVCSub: strcat(str, "MVC substream video"); break;
      case mctMVCMux: strcat(str, "MVC video"); break;
      case mctHEVC:   strcat(str, "HEVC video"); break;
      case mctVC1:    strcat(str, "VC1 video"); break;
      case mctMPEG1A:
      case mctMPEG2A:
        strcat(str, "MPEG audio"); break;
      case mctAES3_337M_MPEG1A:
      case mctAES3_337M_MPEG2A:
        strcat(str, "AES3 SMPTE 337M wrapped MPEG audio"); break;
      case mctAAC_Generic:
      case mctAAC_RAW:
      case mctAAC_LATM:
      case mctAAC_ADTS:
        strcat(str, "AAC audio"); break;
      case mctAC3:              strcat(str, "Dolby Digital audio"); break;
      case mctAES3_337M_AC3:    strcat(str, "AES3 SMPTE 337M wrapped Dolby Digital audio"); break;
      case mctAC3_DDPlus:	      strcat(str, "Dolby Digital Plus audio"); break;
      case mctAES3_337M_DDPlus:	strcat(str, "AES3 SMPTE 337M wrapped Dolby Digital Plus audio"); break;
      case mctAES3_337M_DOLBYE:	strcat(str, "AES3 SMPTE 337M wrapped Dolby E audio"); break;
      case mctAES3_302M:        strcat(str, "AES3 SMPTE 302M LPCM audio"); break;
      case mctDVD_LPCM:         strcat(str, "DVD LPCM audio"); break;
      case mctDTS:              strcat(str, "DTS audio"); break;
      case mctSDDS:             strcat(str, "SDDS audio"); break;
      case mctDVD_Subtitles:    strcat(str, "DVD Subpicture"); break;
      case mctHDMV_LPCM:        strcat(str, "HDMV LPCM audio"); break;
      default:
        if (stream_info.stream_type > 0)
        {
          char str1[64];
          sprintf(str1, "Other, PMT/PSM stream type %d (0x%02X)", stream_info.stream_type, stream_info.stream_type);
          strcat(str, str1);
        }
        else
          strcat(str, "Other");
        break;
      }
      if (stream_info.ts_scrambled)
          strcat(str, ", scrambled at TS level");
      else if (stream_info.PES_scrambled)
          strcat(str, ", scrambled at PES level");
      strcat(str, "\n");
      printf(str);
    }

    printf("\n");
  }

}


#define IDI_CUSTOM_IN_BUFFER_SIZE (IDC_CUSTOM_START_ID + 1)
#define IDI_CUSTOM_PES_OUTPUT     (IDC_CUSTOM_START_ID + 2)
#define IDI_CUSTOM_KEY_FILE     (IDC_CUSTOM_START_ID + 3)

int main_new_api(int argc, char * argv[])
{
	mp2dmux_settings_t demuxer_set;
	mp2dmux_program_info_t program_info;
	mp2dmux_file_info_t file_info;
	mp2dmux_stream_format_t stream_info;
	mp2dmux_sm_init_settings_t sm_set;
	mp2dmux_sm_stream_settings_t sm_stream_set;
	bufstream_tt *ibs;
	int64_t byte_cnt, bytes_left;
	dmux_chunk_info chunk_info;
	int32_t i, j, ret = 1;
	int64_t prg_mod, prg_next;
	uint64_t file_size;
	app_vars_t vars;
	mp2dmux_tt *demuxer = NULL;
	int32_t num_streams_to_demux = 1;
	int32_t input_buffer_size;
	uint32_t pes_output_flag = 0;
	char* key_file_name = NULL;

	char *custom_arg = NULL;

    callbacks_t callbacks;
    callbacks_demux_mp2_t demux_callbacks = {0};
  arg_item_t params[] =
  {
    { IDS_INPUT_FILE,    1,  &vars.in_filename},
    { IDI_C_TITLE_ID,    0,  &vars.tid},
    { IDI_C_STREAM_ID,   0,  &vars.sid},
    { IDS_OUTPUT_FILE,   0,  &vars.out_filename},
    { IDS_CUSTOM_ARG,    0,  &custom_arg},
    // custom args
    { IDI_CUSTOM_IN_BUFFER_SIZE, 0, &input_buffer_size },
    { IDI_CUSTOM_PES_OUTPUT,     0, &pes_output_flag },
	{ IDI_CUSTOM_KEY_FILE,     0, &key_file_name }

  };

  static const arg_item_desc_t custom_args[] =
  {
      { IDI_CUSTOM_IN_BUFFER_SIZE, { "in_buf", 0 },     ItemTypeInt,   0, "set specific size for input buffer" },
      { IDI_CUSTOM_PES_OUTPUT,     { "pes_output", 0 }, ItemTypeNoArg, 0, "output the packetized elementary stream (PES) instead of an elementary one (MPEG2-TS and PS only)" },
	  { IDI_CUSTOM_KEY_FILE,     { "key_file", 0 }, ItemTypeString, 0, "File with descramble keys" }
  };

	memset(&vars, 0, sizeof(app_vars_t));

  if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params, 
      custom_args, sizeof(custom_args) / sizeof(custom_args[0])) < 0)
  {
    printf("\n==== MainConcept MPEG-2 Demuxer buffer sample ====\n\n");
    printf( "Usage:\n");
    printf( " To get programs and streams info only:\n");
    printf( "   sample_demux_mp2_buffer.exe -i file.mpg\n");
    printf( " To demux a stream (sid) from specified program (tid):\n");
    printf( "   sample_demux_mp2_buffer.exe -i file.mpg -tid 1 -sid 2 -o stream.out\n");
    printf( "   Set sid = -1 to demultiplex all streams of the specified program (new API).\n");
    printf( "   Stream ID and a type suffix will be appended to the file name in this case.\n");
    printf( " Use -in_buf to specify input buffer size.\n");
    printf( " Use -pes_output option to produce PES stream instead of elementary one.\n");
    printf( " You can use -weak to force weak detection (new API).\n");
    printf( " Use -old to switch sample to old demux API (depreciated).\n\n");
    return 0;
  }

  if (input_buffer_size == ITEM_NOT_INIT)
  {
      input_buffer_size = CHUNK_SIZE;
  }

  pes_output_flag = (pes_output_flag == ITEM_NOT_INIT) ? 0 : 1;

	// open the input file
	vars.in_bs = open_file_buf_read(vars.in_filename, input_buffer_size + 1, NULL);
	if (!vars.in_bs)
	{
		printf("Unable to open the input file.\n");
		goto clean_exit;
	}

	// get the size of the input file
	if (vars.in_bs->auxinfo(vars.in_bs, 0, FILESIZE_INFO, (void*)&file_size, sizeof(file_size)) != BS_OK)
	{
		printf("Unable to get the size of the input file.\n");
		goto clean_exit;
	}
	bytes_left = file_size;
	byte_cnt = 0;

	// create the input fifo
	vars.in_fifo = new_fifo_buf(FIFO_SIZE, input_buffer_size + 1);
	if (!vars.in_fifo)
	{
		printf("Unable to create an input fifo.\n");
		goto clean_exit;
	}

	// create a demuxer
	memset(&demuxer_set, 0, sizeof(mp2dmux_settings_t));

    init_callbacks(callbacks);
    demux_callbacks.get_ca_decryptor = get_decryptor;

	demuxer = mp2DemuxNew(&callbacks, &demux_callbacks, &demuxer_set);
	if (demuxer == NULL)
	{
		printf("Unable to create a demuxer instance.\n");
		goto clean_exit;
	}

	// initialize the demuxer streaming mode
	memset(&sm_set, 0, sizeof(mp2dmux_sm_init_settings_t));
	sm_set.input_bs = &vars.in_fifo->output;

	if (custom_arg && !strcmp(custom_arg, "-weak")) 
	{
		sm_set.flags = MP2DMUX_SM_INIT_FLAG_WEAK_DETECTION;
	}

	if (key_file_name)
		if (!read_keys(key_file_name))
			goto clean_exit;

	if (mp2DemuxStreamModeInit(demuxer, &sm_set))
		goto clean_exit;

	// setup a pointer to the input side of our
	// input fifo, we send data to the demuxer here
	ibs = &vars.in_fifo->input;

	i = BS_OK;
	memset(&chunk_info, 0, sizeof(dmux_chunk_info));

	// loop until the streams are recognized
	while (i != BS_ERROR)
	{
		uint32_t bytes_avail = input_buffer_size;
		uint8_t *ptr;

		// get a pointer into the input fifo
		ptr = ibs->request(ibs, bytes_avail);
		if (!ptr)
		{
			// there are no bytes_avail available in the fifo
			// see if there is any space in the fifo
			bytes_avail = ibs->usable_bytes(ibs);
			ptr = ibs->request(ibs, bytes_avail);
			if (!ptr)
			{
				printf("Input fifo is full.\n");
				goto clean_exit;
			}
		}

		// read some data from the file into the input fifo
		chunk_info.length = vars.in_bs->copybytes(vars.in_bs, ptr, bytes_avail);
		if (!chunk_info.length)
		{
			printf("Unable to recognize any streams.\n");
			goto clean_exit;
		}

		// confirm the bytes
		ibs->confirm(ibs, chunk_info.length);
	
		// inform the demuxer of the bytes placed in the fifo
		i = ibs->auxinfo(ibs, 0, DMUX_CHUNK_INFO, (uint8_t*)&chunk_info, sizeof(dmux_chunk_info));

		bytes_left -= chunk_info.length;
		byte_cnt += chunk_info.length;

		memset(&file_info, 0, sizeof(mp2dmux_file_info_t));

		// get the info about the file
		if (mp2DemuxGetFileInfo(demuxer, &file_info))
			goto clean_exit;

		if (file_info.program_count > 0)
			break;
	}

	if (file_info.program_count <= 0)
	{
		printf("Unable to recognize any streams.\n");
		goto clean_exit;
	}

	if ((vars.sid == ITEM_NOT_INIT) || (vars.tid == ITEM_NOT_INIT) || !vars.out_filename)
	{
    // just display the stream info and exit
    display_stream_info_new_api(demuxer, file_info);
		ret = 0;
		goto clean_exit;
	}


  if (mp2DemuxGetProgramInfo(demuxer, &program_info, vars.tid))
    goto clean_exit;


  num_streams_to_demux = 1;

  if(vars.sid == -1) // demux all streams.
    num_streams_to_demux = program_info.stream_count;

  for( int i = 0; i < num_streams_to_demux; i++)
  {
	  // get info about the stream to demux
	  if (mp2DemuxGetStreamFormat(demuxer, &stream_info, vars.tid, (vars.sid == -1)? i : vars.sid ))
		  goto clean_exit;

    char filename[2048];
    sprintf(filename, "%s", vars.out_filename);
   
    if(vars.sid == -1) // assign stream names and extension
    {
      sprintf(filename, "%s_%02d", vars.out_filename, i);
      add_file_extension(filename, stream_info.format.stream_mediatype);
    }

	  // setup the output file
	  switch (stream_info.format.stream_mediatype)
	  {
	  case mctAES3_302M:
	  case mctDVD_LPCM:
	  case mctHDMV_LPCM:
		  // wrap the output in wave format
		  vars.out_bufs[i].out_bs = open_wave_buf_write(filename);
		  break;
	  default:
		  // normal bufstream output
		  vars.out_bufs[i].out_bs = open_file_buf_write(filename, 65536 + 1, NULL);
	  }

	  if (!vars.out_bufs[i].out_bs)
	  {
		  printf("Unable to create an output bufstream.\n");
		  goto clean_exit;
	  }

	  // setup the output fifo
	  vars.out_bufs[i].out_fifo = new_fifo_buf(FIFO_SIZE, CHUNK_SIZE);
	  if (!vars.out_bufs[i].out_fifo)
	  {
		  printf("Unable to create an output fifo.\n");
		  goto clean_exit;
	  }

    vars.out_bufs[i].out_fifo->output.auxinfo = stream_auxinfo;
    vars.out_bufs[i].out_fifo->output.drive_ptr = (struct drive_struct*)&vars.out_bufs[i];

	  // add the stream to demux
	  memset(&sm_stream_set, 0, sizeof(mp2dmux_sm_stream_settings_t));

    sm_stream_set.bs = &vars.out_bufs[i].out_fifo->input;
    sm_stream_set.program_idx = vars.tid;
    sm_stream_set.stream_idx = (vars.sid == -1)? i : vars.sid;
    sm_stream_set.output_pes_flag = pes_output_flag;

	  // enable this to output an AVC base stream and all associated MVC substreams as a single stream
    //	sm_stream_set.mvc_reassemble_flag = 1;

	  if (mp2DemuxStreamModeAddStream(demuxer, &sm_stream_set))
		  goto clean_exit;
  }


  // flush demuxer
  mp2DemuxStreamModeFlush(demuxer);

  // we have to "rewind" input bufstream to make sure that all data from input file will be fetched
  close_file_buf(vars.in_bs, 0);
  vars.in_bs = open_file_buf_read(vars.in_filename, input_buffer_size + 1, NULL);


  bytes_left = file_size;
  byte_cnt = 0;

  prg_next = byte_cnt;
	prg_mod = file_size / 100;
  if (!prg_mod)
    prg_mod = 1;

	// main demuxing loop
	ibs = &vars.in_fifo->input;
	i = BS_OK;

	memset(&chunk_info, 0, sizeof(dmux_chunk_info));

	while ((bytes_left > 0) && (i != BS_ERROR))
	{
		uint32_t bytes_avail = input_buffer_size;
		uint8_t *ptr;

		if (bytes_avail > bytes_left)
			bytes_avail = (uint32_t)bytes_left;

		ptr = ibs->request(ibs, bytes_avail);
		if (!ptr)
		{
			bytes_avail = vars.in_bs->usable_bytes(vars.in_bs);
			ptr = ibs->request(ibs, bytes_avail);
			if (!ptr)
			{
				printf("\nUnable to request %u bytes from input fifo.\n", bytes_avail);
				goto clean_exit;
			}
		}

		chunk_info.length = vars.in_bs->copybytes(vars.in_bs, ptr, bytes_avail);
		if (chunk_info.length != bytes_avail)
		{
			printf("\nUnable to copy %u bytes from input file.\n", bytes_avail);
			goto clean_exit;
		}

		ibs->confirm(ibs, chunk_info.length);

		i = ibs->auxinfo(ibs, 0, DMUX_CHUNK_INFO, (uint8_t*)&chunk_info, sizeof(dmux_chunk_info));
		if (i == BS_ERROR)
		{
			printf("\nUnable to send chunk info to the demuxer.\n");
			goto clean_exit;
		}

		byte_cnt += chunk_info.length;
		bytes_left -= chunk_info.length;
		if (byte_cnt >= prg_next)
		{
			printf("\rProcessed %u%% ...", (uint32_t)((double)byte_cnt / (double)file_size * 100.0));
			prg_next += prg_mod;
			if (prg_next > file_size)
				prg_next = file_size;
		}
	}

	ret = 0;

clean_exit:

	// close everything
	if (demuxer)
	{
		mp2DemuxStreamModeDone(demuxer, ret);
		mp2DemuxFree(demuxer);
	}

	if (vars.in_bs)
		close_file_buf(vars.in_bs, 0);

  if (vars.in_fifo)
    free_fifo_buf(vars.in_fifo);

  for (int i = 0; i < MAX_OUTPUT; i++)
  {
    if (vars.out_bufs[i].out_bs)
      close_file_buf(vars.out_bufs[i].out_bs, 0);

    if (vars.out_bufs[i].out_fifo)
      free_fifo_buf(vars.out_bufs[i].out_fifo);
  }

	while(!g_decryptors.empty()){
		mp2DemuxDestroyStaticTSDecryptor(g_decryptors.back());
		g_decryptors.pop_back();
	}


	return ret;
}

bool read_keys(char *name) {
	INIReader reader(name);

	if (reader.ParseError() < 0) {
		return false;
	}
    const std::set<string> &sections = reader.Sections();

    std::set<string>::iterator s;
    for (s = sections.begin(); s != sections.end(); ++s){
        const std::string &section = *s;
        long pid = reader.GetInteger(section, "PID", -1);
		long prog = reader.GetInteger(section, "ProgramNum", -1);
		if (pid>0)
			prog = -1;
		aes_key_s key={0};
		hex2bin(reader.Get(section, "key", std::string()), key.key);
		hex2bin(reader.Get(section, "iv", std::string()), key.iv);
		hex2bin(reader.Get(section, "iv2", std::string()), key.iv2);
		g_keys[std::make_pair(prog, pid)] = key;
	}
	return true;
}

void hex2bin(string basic_string, uint8_t(& key)[16]) {
	//std::fill_n(key, sizeof(key), 0);
	std::istringstream iss(basic_string);
	size_t i=0;
	int c;
	iss >> std::hex;
	while (iss >> c && i < sizeof(key)){
		key[i++] = static_cast<uint8_t>(c);
	}
}


static int32_t get_file_info(app_vars_t *vars, int32_t argc)
{
	int32_t i, j;
	mpgdmux_settings demuxer_set;
	mpgdmux_program_info program_info;
    mpgdmux_tt *demuxer;

	// create a demuxer
	memset(&demuxer_set, 0, sizeof(mpgdmux_settings));
    callbacks_t callbacks;
    init_callbacks(callbacks);
    callbacks_demux_mp2_t demux_callbacks = {0};
    demux_callbacks.get_ca_decryptor = get_decryptor;

    demuxer = mpegDemuxNew(&callbacks, &demux_callbacks, &demuxer_set);
	if (demuxer == NULL)
	{
		printf("Unable to create a demuxer instance.\n");
		return 1;
	}

	// open the input file
	if (mpegDemuxOpen(demuxer, NULL, vars->in_filename))
	{
		mpegDemuxFree(demuxer);
		return 1;
	}

	// get info about the input file
	if (mpegDemuxGetFileInfo(demuxer, &vars->file_info))
	{
		mpegDemuxFree(demuxer);
		return 1;
	}

	if (argc < 4)
	{
		// just display the stream info and exit
		printf("Number of titles: %d\n\n", vars->file_info.program_count);

		for (i = 0; i < vars->file_info.program_count; i++)
		{
			// get info about the program (title)
			if (mpegDemuxGetProgramInfo(demuxer, &program_info, i))
			{
				mpegDemuxFree(demuxer);
				return 1;
			}

			printf("  Title: %d\n", i);

			for (j = 0; j < program_info.stream_count; j++)
			{
				char str[256];

				// get info about the stream
				if (mpegDemuxGetStreamInfo(demuxer, &vars->stream_info, i, j))
				{
					mpegDemuxFree(demuxer);
					return 1;
				}

				sprintf(str, "    Stream: %d, ", j);

				switch(vars->stream_info.sdk_stream_type)
				{
				case MPGDMUX_MPEG_VIDEO:
					strcat(str, "MPEG/AVC/MPEG4 video");
					break;
				case MPGDMUX_HEVC_VIDEO:
					strcat(str, "HEVC video");
					break;
				case MPGDMUX_VC1_VIDEO:
					strcat(str, "VC1 video");
					break;
				case MPGDMUX_MPEG_AUDIO:
					strcat(str, "MPEG audio");
					break;
				case MPGDMUX_AC3_AUDIO:
					strcat(str, "Dolby Digital audio");
					break;
				case MPGDMUX_AES_302M_AUDIO:
					strcat(str, "AES3 SMPTE 302M LPCM audio");
					break;
				case MPGDMUX_DVD_LPCM_AUDIO:
					strcat(str, "DVD LPCM audio");
					break;
				case MPGDMUX_DTS_AUDIO:
					strcat(str, "DTS audio");
					break;
				case MPGDMUX_SDDS_AUDIO:
					strcat(str, "SDDS audio");
					break;
				case MPGDMUX_DVD_SUBPIC:
					strcat(str, "DVD Subpicture");
					break;
				case MPGDMUX_HDMV_LPCM_AUDIO:				
					strcat(str, "HDMV LPCM audio");
					break;
				case MPGDMUX_DVD_PCI:
					strcat(str, "DVD PCI");
					break;
				case MPGDMUX_DVD_DSI:
					strcat(str, "DVD DSI");
					break;
				case MPGDMUX_EAC3_AUDIO:			
					strcat(str, "Dolby Digital Plus audio");
					break;
				case MPGDMUX_AES3_337M_DATA:
					strcat(str, "AES3 SMPTE 337M data");
					break;
				default:
					strcat(str, "Other");
					break;
				}
				if (vars->stream_info.ts_scrambled)
					strcat(str, ", scrambled at TS level");
				else if (vars->stream_info.PES_scrambled)
					strcat(str, ", scrambled at PES level");
				strcat(str, "\n");
				printf(str);
			}

			printf("\n");
		}
	}

	if (argc >= 4)
	{
		// get info about the stream to demux
		if (mpegDemuxGetStreamInfo(demuxer, &vars->stream_info, vars->tid, vars->sid))
		{
			mpegDemuxFree(demuxer);
			return 1;
		}
	}

	// free the demuxer and exit
	mpegDemuxFree(demuxer);

	return 0;
}


int main_old_api(int argc, char * argv[])
{
	mpgdmux_settings demuxer_set;
	mpgdmux_sm_init_settings sm_set;
	mpgdmux_sm_stream_settings sm_stream_set;
	bufstream_tt *ibs;
	int64_t byte_cnt, bytes_left;
	dmux_chunk_info chunk_info;
	int32_t i, ret = 1;
	int64_t prg_mod, prg_next;
	app_vars_t vars;
	mpgdmux_tt *demuxer;
	int32_t input_buffer_size;

    callbacks_t callbacks;
    callbacks_demux_mp2_t demux_callbacks = {0};
  arg_item_t params[] =
  {
    { IDS_INPUT_FILE,    1,  &vars.in_filename},
    { IDI_C_TITLE_ID,    0,  &vars.tid},
    { IDI_C_STREAM_ID,   0,  &vars.sid},
    { IDS_OUTPUT_FILE,   0,  &vars.out_filename},
    // custom args
    { IDI_CUSTOM_IN_BUFFER_SIZE, 0, &input_buffer_size }
  };

  static const arg_item_desc_t custom_args[] =
  {
    { IDI_CUSTOM_IN_BUFFER_SIZE,{ "in_buf", 0 },     ItemTypeInt,    0,  "set specific size for input buffer" },
  };

	memset(&vars, 0, sizeof(app_vars_t));

  if (parse_args(argc - 1, argv + 1, sizeof(params) / sizeof(params[0]), params, 
      custom_args, sizeof(custom_args) / sizeof(custom_args[0])) < 0)
  {
    printf("\n==== MainConcept MPEG-2 Demuxer buffer sample ====\n"
      "Usage:\nsample_demux_mp2_buffer.exe -old -i file.mpg -tid 1 -sid 2 -o stream.out\n\n");
    return 0;
  }

  if (input_buffer_size == ITEM_NOT_INIT)
  {
      input_buffer_size = CHUNK_SIZE;
  }

	// streaming mode with the mpegDemuxXXX API is limited, there are 2 main limitations:
	// 
	// - the app must know the type of the input file or the input file must start with
	//   the appropriate start code
	//
	// - the app must know the id of the stream it wishes to demux
	//
	// to get around this in this sample we first open the file in file mode
	// to get this info, then it is opened in streaming mode to do the
	// actual demuxing
	if (get_file_info(&vars, argc))
	{
		return 1;
	}

	if (argc < 4)
	{
		return 0;
	}

	// now switch to streaming mode

	// setup the input file
	vars.in_bs = open_file_buf_read(vars.in_filename, input_buffer_size + 1, NULL);
	if (!vars.in_bs)
	{
		printf("Unable to open the input file.\n");
		goto clean_exit;
	}

	// setup the input fifo
	vars.in_fifo = new_fifo_buf(FIFO_SIZE, input_buffer_size);
	if (!vars.in_fifo)
	{
		printf("Unable to create an input fifo.\n");
		goto clean_exit;
	}

	// setup the output file

	// Note that open_wave_buf_write will not work with the
	// mpegDemuxXXX API streaming mode!
//	switch (vars.stream_info.sdk_stream_type)
//	{
//	case MPGDMUX_AES_302M_AUDIO:
//	case MPGDMUX_DVD_LPCM_AUDIO:
//	case MPGDMUX_HDMV_LPCM_AUDIO:
		// wrap the output in wave format
//		vars.out_bs = open_wave_buf_write(vars.out_filename);
//		break;
//	default:
		// normal bufstream output
		vars.out_bufs[0].out_bs = open_file_buf_write(vars.out_filename, 65536 + 1, NULL);
//	}

	if (!vars.out_bufs[0].out_bs)
	{
		printf("Unable to create an output bufstream.\n");
		goto clean_exit;
	}

	// setup the output fifo
	vars.out_bufs[0].out_fifo = new_fifo_buf(FIFO_SIZE, CHUNK_SIZE);
	if (!vars.out_bufs[0].out_fifo)
	{
		printf("Unable to create an output fifo.\n");
		goto clean_exit;
	}

	// create a demuxer
	memset(&demuxer_set, 0, sizeof(mpgdmux_settings));

    init_callbacks(callbacks);
    demux_callbacks.get_ca_decryptor = get_decryptor;

	demuxer = mpegDemuxNew(&callbacks, &demux_callbacks, &demuxer_set);
	if (demuxer == NULL)
	{
		printf("Unable to create a demuxer instance.\n");
		goto clean_exit;
	}

	// init streaming mode
	memset(&sm_set, 0, sizeof(mpgdmux_sm_init_settings));

	sm_set.stream_type = vars.file_info.system_stream_flag;
	sm_set.input_bs = &vars.in_fifo->output;
	if (mpegDemuxStreamModeInit(demuxer, &sm_set))
		goto clean_exit;

	vars.out_bufs[0].out_fifo->output.auxinfo = stream_auxinfo;
	vars.out_bufs[0].out_fifo->output.drive_ptr = (struct drive_struct*)&vars.out_bufs[0];

	// add the stream to demux
	memset(&sm_stream_set, 0, sizeof(mpgdmux_sm_stream_settings));

	sm_stream_set.bs = &vars.out_bufs[0].out_fifo->input;
	sm_stream_set.ID = vars.stream_info.ID;
	sm_stream_set.PID = vars.stream_info.elementary_PID; 
	sm_stream_set.sub_ID = vars.stream_info.sub_ID;

	if (mpegDemuxStreamModeAddStream(demuxer, &sm_stream_set))
		goto clean_exit;


	byte_cnt = 0;
  prg_next = 0;
	prg_mod = vars.file_info.file_size / 100;
  if (!prg_mod)
		prg_mod = 1;

	// main demuxing loop
	ibs = &vars.in_fifo->input;
	i = BS_OK;
	bytes_left = vars.file_info.file_size;

	memset(&chunk_info, 0, sizeof(dmux_chunk_info));

	while ((bytes_left > 0) && (i != BS_ERROR))
	{
		uint32_t bytes_avail = input_buffer_size;
		uint8_t *ptr;

		ptr = ibs->request(ibs, bytes_avail);
		if (!ptr)
		{
			bytes_avail = vars.in_bs->usable_bytes(vars.in_bs);
			ptr = ibs->request(ibs, bytes_avail);
			if (!ptr)
			{
				printf("\nUnable to request %u bytes from input file.\n", bytes_avail);
				goto clean_exit;
			}
		}

		chunk_info.length = vars.in_bs->copybytes(vars.in_bs, ptr, bytes_avail);
		if (!chunk_info.length)
		{
			printf("\nUnable to copy %u bytes from input file.\n", bytes_avail);
			goto clean_exit;
		}

		ibs->confirm(ibs, chunk_info.length);

		i = ibs->auxinfo(ibs, 0, DMUX_CHUNK_INFO, (uint8_t*)&chunk_info, sizeof(dmux_chunk_info));
		if (i == BS_ERROR)
		{
			printf("\nUnable to send chunk info to the demuxer.\n");
			goto clean_exit;
		}

		byte_cnt += chunk_info.length;
		bytes_left -= chunk_info.length;
		if (byte_cnt >= prg_next)
		{
			printf("\rProcessed %u%% ...", (uint32_t)((double)byte_cnt / (double)vars.file_info.file_size * 100.0));
			prg_next += prg_mod;
		}
	}

	ret = 0;

clean_exit:

	// close everything
	if (demuxer)
	{
		mpegDemuxStreamModeDone(demuxer, ret);
		mpegDemuxFree(demuxer);
	}

	if (vars.in_bs)
		close_file_buf(vars.in_bs, 0);

  if (vars.in_fifo)
    free_fifo_buf(vars.in_fifo);

  for (int i = 0; i < MAX_OUTPUT; i++)
  {
    if (vars.out_bufs[i].out_bs)
      close_file_buf(vars.out_bufs[i].out_bs, 0);

    if (vars.out_bufs[i].out_fifo)
      free_fifo_buf(vars.out_bufs[i].out_fifo);
  }

	return ret;
}
                   
int32_t main(int32_t argc, char * argv[])
{
    USE_MP2_API = true;

    int32_t iArgc = 0;
    char* ppArgv[128];

    for ( int32_t i = 0; i < argc; i++ ) {

        if ( strcmp(argv[i], "-old") == 0 ) {
            USE_MP2_API = false;
        } else {
            ppArgv[iArgc] = argv[i];
            iArgc++;
        }
    };

    if ( USE_MP2_API ) {
        return main_new_api(iArgc, ppArgv);
    } else {
        return main_old_api(iArgc, ppArgv);
    }

    return 0;
}
