/* ----------------------------------------------------------------------------
 * File: dm_reader.h
 * Desc: sample code for reading and parsing descriptive meta data
 *
 * Copyright (c) 2014 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.  Unauthorized
 * reproduction or distribution of any portion is prohibited by law.
 * ----------------------------------------------------------------------------
 */

 void print_dm_framework_type(uint32_t type);

void parse_dms_1_framework(mxfdmux_tt *demuxer, int32_t indent, const char * title, mxfdmux_dm_framework_info_tt *framework_info);

 void read_descriptive_metadata(mxfdmux_tt *demuxer, mxfdmux_file_info *file_info, mxfdmux_dm_track_info_tt *dm_tracks);