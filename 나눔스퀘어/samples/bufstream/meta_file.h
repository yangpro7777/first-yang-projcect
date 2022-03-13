/* ----------------------------------------------------------------------------
 * File: meta_file.h
 *
 * Desc: metadata-storage class implementation
 *
 *  Copyright (c) 2015 MainConcept GmbH or its affiliates.  All rights reserved.
 *
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.  
 * This software is protected by copyright law and international treaties.  Unauthorized 
 * reproduction or distribution of any portion is prohibited by law.
 *
 * ----------------------------------------------------------------------------
 */

#ifndef META_FILE_H
#define META_FILE_H

#include "metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BS_UNICODE
metadata_storage_tt* new_metadata_file(wchar_t *name);
#else
metadata_storage_tt* new_metadata_file(char *name);
#endif

bufstream_tt* init_metadata_filter(metadata_storage_tt *md, bufstream_tt *bs);

#ifdef __cplusplus
}
#endif

#endif /* META_FILE_H */

