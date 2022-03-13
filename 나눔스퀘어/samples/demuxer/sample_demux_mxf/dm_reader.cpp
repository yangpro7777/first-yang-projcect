
/* ----------------------------------------------------------------------------
 * File: dm_reader.cpp
 * Desc: sample code for reading and parsing descritpive meta data
 *
 * Copyright (c) 2014 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "demux_mxf.h"
#include "dm_reader.h"


void print_dm_framework_type(uint32_t type)
{
  switch (type)
  {
  case MXF_DM_FRAMEWORK_TYPE_DARK:                   printf("        Framework type: Dark Metadata\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_DMS_1_PRODUCTION:       printf("        Framework type: DMS-1 Production (SMPTE 380)\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_DMS_1_CLIP:             printf("        Framework type: DMS-1 Clip (SMPTE 380)\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_DMS_1_SCENE:            printf("        Framework type: DMS-1 Scene (SMPTE 380)\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_TEXT_BASED:             printf("        Framework type: Text base (SMPTE RP 2057)\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_AMWA_AS03:              printf("        Framework type: AMWA AS-03\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_CORE:         printf("        Framework type: AMWA AS-11 Core\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_SEGMENTATION: printf("        Framework type: AMWA AS-11 Segmentation\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_UKDDP:        printf("        Framework type: AMWA AS-11 UKDDP\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_P2_STATIC:              printf("        Framework type: Panasonic P2 Static (Clip Metadata)\n"); break;
  case MXF_DM_FRAMEWORK_TYPE_P2_EVENT:               printf("        Framework type: Panasonic P2 Event (Thumbnail)\n"); break;
  default: printf("        Framework type: Unknown\n"); 
  }
}

static void print_int( uint32_t indent, const char *title, uint8_t *buffer, uint64_t len)
{
  int64_t int_val = 0;
  int8_t shift = len -1;

  char ch_indent[256] ="";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s", ch_indent, title);

  for(int i = 0; i < len; i++)
  {
    int_val += (buffer[i] << (shift * 8));
    shift--;
  }
  printf("%lld\n", int_val);

}

static void print_utf16( uint32_t indent, const char *title, uint8_t *buffer, uint64_t len)
{
  char ch_indent[256] ="";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s", ch_indent, title);

  for(int i = 0; i < len; i +=2)
  {
    char ch[2] = "";
    ch[0] = buffer[i+1];
    printf(ch);
  }
  printf("\n");
}

static void print_utf8(uint32_t indent, const char *title, uint8_t *buffer, uint64_t len)
{
  char ch_indent[256] ="";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s", ch_indent, title);

  for(int i = 0; i < len; i++)
  {
    char ch[2] = "";
    ch[0] = buffer[i];
    printf(ch);
  }
  printf("\n");
}

static void print_uid(uint32_t indent, const char *title, uint8_t *buffer, uint64_t len)
{
  char ch_indent[256] ="";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s", ch_indent, title);

  for(int i = 0; i < len; i++)
    printf(" %02X", buffer[i]);
  printf("\n");
}

static void print_data( uint32_t indent, const char *title, uint8_t *buffer, uint64_t len)
{
  uint64_t int_val = 0;
  uint8_t shift = len -1;

  char ch_indent[256] ="";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s %d Bytes", ch_indent, title, len);

 // dump the data
 // for(int i = 0; i < len; i++)
 //   printf("%02X", buffer[i]);

  printf("\n\n");

}

////////// Descriptive meta data parsing samples //////


static void do_dms_ident_array(mxfdmux_tt *demuxer, uint32_t indent, const char *title, uint8_t *buffer)
{
  // an array of identifiers, each referring to another framework set
  
  uint8_t *buf = buffer;
  uint32_t count_items = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
  uint32_t items_size = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + buf[7];
  buf += 8;

  for(uint32_t i = 0; i < count_items; i++)
  {
    mxfdmux_dm_framework_info_tt ref_framework_info;
    uint8_t object_id[16];
    memcpy(object_id, buf, 16);
    char item_title[256];
    sprintf(item_title, "%s #%d",title, i);
    // get this framework and parse it
    if(mxfDemuxGetDMFrameworkInfo(demuxer, &ref_framework_info, object_id, 0) == 0)
      parse_dms_1_framework(demuxer, indent, item_title, &ref_framework_info);

    buf += 16;
  }
}

void parse_dms_1_framework(mxfdmux_tt *demuxer, int32_t indent, const char * title, mxfdmux_dm_framework_info_tt *framework_info)
{
  // excerpt from SMPTE 380 Annex A

  static const uint8_t DMS1_Participant_Sets[16]  = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x05, 0x40, 0x13, 0x00}; // Batch of referenced UID's
  static const uint8_t DMS1_Person_Sets[16]       = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x03, 0x40, 0x14, 0x00}; // Batch of referenced UID's
  static const uint8_t DMS1_Title_Sets[16]        = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x05, 0x40, 0x04, 0x00}; // Batch of referenced UID's
  static const uint8_t DMS1_Location_Sets[16]     = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x03, 0x40, 0x16, 0x00}; // Batch of referenced UID's
  static const uint8_t DMS1_Address_Sets[16]      = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x05, 0x40, 0x17, 0x00}; // Batch of referenced UID's
  static const uint8_t DMS1_Annotation_Sets[16]   = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x06, 0x01, 0x01, 0x04, 0x05, 0x40, 0x0D, 0x00}; // Batch of referenced UID's

  static const uint8_t DMS1_Job_Funktion[16]      = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x02, 0x30, 0x05, 0x01, 0x01, 0x00, 0x00, 0x00}; // UTF 16
  static const uint8_t DMS1_Family_Name[16]       = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x02, 0x30, 0x06, 0x03, 0x01, 0x01, 0x01, 0x00}; // UTF 16
  static const uint8_t DMS1_First_Given_Name[16]  = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x02, 0x30, 0x06, 0x03, 0x01, 0x02, 0x01, 0x00}; // UTF 16
  static const uint8_t DMS1_Main_Title[16]        = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x01, 0x05, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00}; // UTF 16
  static const uint8_t DMS1_Framework_Title[16]   = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x00, 0x01, 0x05, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00}; // UTF 16
  

  char ch_indent[256] = "";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s\n", ch_indent, title);

  indent += 2;

  uint8_t * dm_buffer = (uint8_t*)malloc((size_t)framework_info->length);

  //read the raw opaque framework data
  if(mxfDemuxGetDMFrameworkData(demuxer, framework_info->instance_id, dm_buffer, framework_info->length))
  {
    free(dm_buffer);
    return;
  }

  // parse the framework. read tags, get identifiers and interpret them
  uint8_t *buffer = dm_buffer;
  uint64_t length = framework_info->length;

  MXF_UUID Identifier;

  while(length)
  {
    uint16_t tag;
    uint16_t len;

    tag = (buffer[0] << 8) + buffer[1];
    len = (buffer[2] << 8) + buffer[3];
    buffer += 4;
    length -= 4;

    // look up the identifier
    if( mxfDemuxTranslateLocalTag(demuxer, tag, Identifier ) == 0) 
    {
      // sets
      if(0 == memcmp(&Identifier[8], &DMS1_Participant_Sets[8], 8))       
        do_dms_ident_array(demuxer, indent, "DMS-1 Participant Set", buffer);
      else if(0 == memcmp(&Identifier[8], &DMS1_Person_Sets[8], 8)) 
        do_dms_ident_array(demuxer, indent, "DMS-1 Person Set", buffer);
      else if(0 == memcmp(&Identifier[8], &DMS1_Title_Sets[8], 8)) 
        do_dms_ident_array(demuxer, indent, "DMS-1 Title Set", buffer);
      else if(0 == memcmp(&Identifier[8], &DMS1_Location_Sets[8], 8)) 
        do_dms_ident_array(demuxer, indent, "DMS-1 Location Set", buffer);
      else if(0 == memcmp(&Identifier[8], &DMS1_Address_Sets[8], 8)) 
        do_dms_ident_array(demuxer, indent, "DMS-1 Address Set", buffer);
      else if(0 == memcmp(&Identifier[8], &DMS1_Annotation_Sets[8], 8)) 
        do_dms_ident_array(demuxer, indent, "DMS-1 Annotation Set", buffer);

      // parameters
      else if(0 == memcmp(&Identifier[8], &DMS1_Job_Funktion[8], 8))
        print_utf16(indent, "DMS-1 Participant: Job Function = ", buffer, len);
      else if(0 == memcmp(&Identifier[8], &DMS1_Family_Name[8], 8))
        print_utf16(indent, "DMS-1 Person: Family Name = ", buffer, len);
      else if(0 == memcmp(&Identifier[8], &DMS1_First_Given_Name[8], 8))
        print_utf16(indent, "DMS-1 Person: First Given Name = ", buffer, len);
      else if(0 == memcmp(&Identifier[8], &DMS1_Main_Title[8], 8))
        print_utf16(indent, "DMS-1 Title: Main Title = ", buffer, len);
      else if(0 == memcmp(&Identifier[8], &DMS1_Framework_Title[8], 8))
        print_utf16(indent, "DMS-1 Title: Framework Title = ", buffer, len);

    }

    buffer += len;
    length -= len;
  }

  free( dm_buffer );
}


static void parse_as11_framework(mxfdmux_tt *demuxer, uint32_t indent, const char * title, mxfdmux_dm_framework_info_tt *framework_info)
{

  // from AS-11 Appendix F meta data definitions
  static const uint8_t AS_11_Series_Title[16] = { 0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x01}; // a UTF 16 string
  static const uint8_t AS_11_Programme_Title[16] =  { 0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x02}; // a UTF 16 string

  char ch_indent[256] = "";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s\n", ch_indent, title);

  indent += 2;

  uint8_t * dm_buffer = (uint8_t*)malloc((size_t)framework_info->length);
 
  //read the raw opaque framework data
  if(mxfDemuxGetDMFrameworkData(demuxer, framework_info->instance_id, dm_buffer, framework_info->length))
  {
    free(dm_buffer);
    return;
  }

  // parse the framework. read tags, get identifiers and interpret them
  uint8_t *buffer = dm_buffer;
  uint64_t length = framework_info->length;

  MXF_UUID Identifier;

  while(length)
  {
    uint16_t tag;
    uint16_t len;

    tag = (buffer[0] << 8) + buffer[1];
    len = (buffer[2] << 8) + buffer[3];
    buffer += 4;
    length -= 4;

    // look up the identifier
    if( mxfDemuxTranslateLocalTag(demuxer, tag, Identifier ) == 0) 
    {
      if(0 == memcmp(Identifier, AS_11_Series_Title, 16))
        print_utf16(indent, "Series Title = ", buffer, len);
      else if(0 == memcmp(Identifier, AS_11_Programme_Title, 16))
        print_utf16(indent, "Programme Title = ", buffer, len);    
    }

    buffer += len;
    length -= len;
  }

  free( dm_buffer );
}

static void parse_p2_framework(mxfdmux_tt *demuxer, uint32_t indent, const char * title, mxfdmux_dm_framework_info_tt *framework_info)
{
  // Panasonic DM Framework proprietary framework sets
  static const uint8_t P2_Clip_Start_Position[16] = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x02, 0x01, 0x01}; //UINT_8
  static const uint8_t P2_Clip_MetaData_Text[16]  = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x02, 0x01, 0x02}; // UTF-8 string  
  static const uint8_t P2_Thumbnail_Format[16]    = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x03, 0x02, 0x01}; // UINT_8
  static const uint8_t P2_Thumbnail_Width[16]     = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x03, 0x02, 0x02}; // UINT_16
  static const uint8_t P2_Thumbnail_Height[16]    = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x03, 0x02, 0x03}; // UINT_16
  static const uint8_t P2_Thumbnail_Data[16]      = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x09, 0x0E, 0x0E, 0x01, 0x01, 0x11, 0x03, 0x02, 0x04}; // DATA


  char ch_indent[256] = "";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s\n", ch_indent, title);

  indent += 2;

  uint8_t * dm_buffer = (uint8_t*)malloc((size_t)framework_info->length);

  //read the raw opaque framework data
  if(mxfDemuxGetDMFrameworkData(demuxer, framework_info->instance_id, dm_buffer, framework_info->length))
  {
    free(dm_buffer);
    return;
  }

  // parse the framework. read tags, get identifiers and interpret them
  uint8_t *buffer = dm_buffer;
  uint64_t length = framework_info->length;

  MXF_UUID Identifier;

  while(length)
  {
    uint16_t tag;
    uint16_t len;

    tag = (buffer[0] << 8) + buffer[1];
    len = (buffer[2] << 8) + buffer[3];
    buffer += 4;
    length -= 4;

    // look up the identifier
    if( mxfDemuxTranslateLocalTag(demuxer, tag, Identifier ) == 0) 
    {
      if(0 == memcmp(Identifier, P2_Clip_Start_Position, 16))
        print_int(indent, "Clip Start Position = ", buffer, len);
      else if(0 == memcmp(Identifier, P2_Clip_MetaData_Text, 16))
        print_utf8(indent, "Clip Metadata Text = \n\n", buffer, len);  
      else if(0 == memcmp(Identifier, P2_Thumbnail_Format, 16))
        print_int(indent, "Thumbnail Format = ", buffer, len);  
      else if(0 == memcmp(Identifier, P2_Thumbnail_Width, 16))
        print_int(indent, "Thumbnail Width = ", buffer, len);  
      else if(0 == memcmp(Identifier, P2_Thumbnail_Height, 16))
        print_int(indent, "Thumbnail Height = ", buffer, len); 
      else if(0 == memcmp(Identifier, P2_Thumbnail_Data, 16))
        (print_data)(indent, "Thumbnail Data = ", buffer, len);  
    }

    buffer += len;
    length -= len;
  }

  free( dm_buffer );
}


static void parse_text_based_framework(mxfdmux_tt *demuxer, uint32_t indent, const char * title, mxfdmux_dm_framework_info_tt *framework_info)
{
  static const uint8_t Text_based_Object[16] = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x0D, 0x06, 0x01, 0x01, 0x04, 0x05, 0x41, 0x01, 0x00}; // UID
  static const uint8_t Text_based_Meta_Data_Payload_Scheme_ID[16] = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x0D, 0x04, 0x06, 0x08, 0x06, 0x00, 0x00, 0x00, 0x00}; //UID
  static const uint8_t Text_MIME_Media_Type[16] = { 0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x0D, 0x04, 0x09, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00 }; // UTF 16
  static const uint8_t RFC_5646_Text_Language_Code[16] = { 0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x0D, 0x03, 0x01, 0x01, 0x02, 0x02, 0x14, 0x00, 0x00 }; // UTF 16
  static const uint8_t UTF_8_Text_Data[16] = {0x06, 0x0E, 0x2B, 0x34, 0x01, 0x01, 0x01, 0x0D, 0x03, 0x01, 0x02, 0x20, 0x03, 0x01, 0x00, 0x00}; // UTF 8

  
  char ch_indent[256] = "";
  for( int32_t i = 0; i < indent; i++ )
    strcat(ch_indent, " ");

  printf("%s%s\n", ch_indent, title);

  indent += 2;

  uint8_t * dm_buffer = (uint8_t*)malloc((size_t)framework_info->length);
 
  //read the raw opaque framework data
  if(mxfDemuxGetDMFrameworkData(demuxer, framework_info->instance_id, dm_buffer, framework_info->length) != 0)
  {
    free(dm_buffer);
    return;
  }

  // parse the framework. read tags, get identifiers and interpret them
  uint8_t *buffer = dm_buffer;
  uint64_t length = framework_info->length;

  MXF_UUID Identifier;

  while(length)
  {
    uint16_t tag;
    uint16_t len;

    tag = (buffer[0] << 8) + buffer[1];
    len = (buffer[2] << 8) + buffer[3];
    buffer += 4;
    length -= 4;

    // look up the identifier
    if( mxfDemuxTranslateLocalTag(demuxer, tag, Identifier ) == 0) 
    {
      if(0 == memcmp(Identifier, Text_based_Object, 16)) 
      {
        // A reference to another framework !
        mxfdmux_dm_framework_info_tt ref_framework_info;
        uint8_t object_id[16];
        memcpy(object_id, buffer, 16);

        // get this framework and parse it
        if(mxfDemuxGetDMFrameworkInfo(demuxer, &ref_framework_info, object_id, 0) == 0)
          parse_text_based_framework(demuxer, indent, "Text-based Object", &ref_framework_info);
      }
      else if(0 == memcmp(Identifier, Text_based_Meta_Data_Payload_Scheme_ID, 16)) 
        print_uid(indent, "Text based Meta Data Payload Scheme ID = ", buffer, len); 
      else if(0 == memcmp(Identifier, Text_MIME_Media_Type, 16)) 
        print_utf16(indent, "Text MIME Media Type = ", buffer, len);
      else if(0 == memcmp(Identifier, RFC_5646_Text_Language_Code, 16)) 
        print_utf16(indent, "RFC 5646 Text Language_Code = ", buffer, len);
      else if(0 == memcmp(Identifier, UTF_8_Text_Data, 16)) 
        print_utf8(indent, "UTF-8 Text Data = \n\n ", buffer, len);

    }

    buffer += len;
    length -= len;
  }

  free( dm_buffer );
}



//
// Demo how to get DM framework as opaque data and parse it
//
void read_descriptive_metadata(mxfdmux_tt *demuxer, mxfdmux_file_info *file_info, mxfdmux_dm_track_info_tt *dm_tracks)
{
  uint32_t i, j;
  
  if(file_info->dm_framework_set_count)
  {
    printf("\n\n");
    printf("********************************************************************************\n");
    printf("**       A little demo of retrieval and parsing of Descriptive Meta Data      **\n");
    printf("**                                                                            **\n");
    printf("**   If you don't see much output, this is because this sample application    **\n");
    printf("**  understands only a few DM schemes and a few parameters of these schemes.  **\n");
    printf("**      Extend this sample code for DM Schemes you're interested in.          **\n");
    printf("********************************************************************************\n\n");

    mxfdmux_dm_framework_info_tt framework_info;

    // Lookup a framework by index
    // get info of the first dm framework.
    //if(mxfDemuxGetDMFrameworkInfo(demuxer, &framework_info, NULL, 0) == 0)
    //{
    //  printf("DM Framework, index 0, type %d, length %d\n\n", framework_info.framework_type, framework_info.length);
    //}

    // call that once before reading framework data
    mxfDemuxOpenDMParser(demuxer);

    // Lookup frameworks by instance UID.
    // Get info of frameworks referenced by the segments of the DM Tracks
    if(file_info->dm_tracks_count)
    {
      for(i = 0; i < file_info->dm_tracks_count; i++)
      {
        for( j = 0; j < dm_tracks[i].num_segments; j++ )
        {
           mxfdmux_dm_segment_info_tt *segment = dm_tracks[i].segments + j;
           if(mxfDemuxGetDMFrameworkInfo(demuxer, &framework_info, segment->framework_id, 0) == 0)
           {
              printf("\n  DM Track %d, Segment %d, Framework type %d, length %d\n", i, j, framework_info.framework_type, framework_info.length);

              // It's a framework of a DM scheme we know and can deal with? - Get the data and parse it

              if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_DMS_1_PRODUCTION) // SMPTE ST 380 DMS-1
                parse_dms_1_framework(demuxer, 4, "DMS-1 Production Framework", &framework_info); 
              if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_DMS_1_SCENE)  // SMPTE ST 380 DMS-1
                parse_dms_1_framework(demuxer, 4, "DMS-1 Scene Framework", &framework_info); 
              if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_DMS_1_CLIP)  // SMPTE ST 380 DMS-1
                parse_dms_1_framework(demuxer, 4, "DMS-1 Clip Framework", &framework_info); 
              else if (framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_TEXT_BASED) // SMPTE RP 2057 Text based, as found e.g. in Sony XAVC files
                parse_text_based_framework(demuxer, 4, "SMPTE RP 2057 Text based Framework", &framework_info);
              else if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_AMWA_AS11_CORE)  // AMWA AS-11 http://www.amwa.tv/resources/specifications.shtml                   
                parse_as11_framework(demuxer, 4, "AS-11 Core Framework",  &framework_info);
              else if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_P2_STATIC)  // Panasonic P2 AVC-Ultra proprietary DM Scheme, XML                
                parse_p2_framework(demuxer, 4, "P2 Static Framework",  &framework_info);
              else if(framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_P2_EVENT)  // Panasonic P2 AVC-Ultra proprietary DM Scheme, XML                
                parse_p2_framework(demuxer, 4, "P2 Static Event Framework",  &framework_info);
              else if (framework_info.framework_type == MXF_DM_FRAMEWORK_TYPE_DARK)
              {
                // Check mxfdmux_file_info->dm_schemes for meta data schemes you know...
                // Check this framework's key if it is of a meta data scheme you know...
                // Get it and do something with it..

                /*if(0 == memcmp(framework_info.key, a_key_you_know?, 16)
                {
                  get and parse it
                }*/
              }
           }
        }
      }

      // close the DM parser when done with the meta data to save resources!
      mxfDemuxCloseDMParser(demuxer);
    }
    printf("\n\n");
    printf("********************************************************************************\n");
    printf("**                   Done with Descriptive Meta Data                          **\n");
    printf("********************************************************************************\n\n");
  }
}