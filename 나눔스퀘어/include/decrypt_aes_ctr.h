/**
@file: decrypt_aes_ctr.h
@brief AES CTR decryptor API

@verbatim
File: decrypt_aes_ctr.h
Desc: AES CTR decryptor API

Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
This software is protected by copyright law and international treaties.  Unauthorized
reproduction or distribution of any portion is prohibited by law.
@endverbatim
**/

#pragma once
#ifndef __DECRYPT_AES_CTR_H__
#define __DECRYPT_AES_CTR_H__

#include "mctypes.h"
#include "mcmediatypes.h"

#ifdef __GNUC__
#pragma pack(push, 1)
#else
#pragma pack(push)
#pragma pack(1)
#endif

/**
* @brief opaque AES CTR decryptor instance pointer
*/
typedef void * aes_decryptor_t;

/**
* @brief get resource function

* @param get_rc pointer to the resource function
* @param name resource name

* @return pointer to the resource if successful, NULL otherwise
*/
typedef void *(MC_EXPORT_API *get_rc_t)(const char* name);

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

    /**
    * @brief allocates AES CTR decryptor instance

    * @param get_rc pointer to the resource function

    * @return non-NULL pointer to AES CTR decryptor instance on success, or NULL pointer on failure
    */
    aes_decryptor_t MC_EXPORT_API aesctrNew(get_rc_t get_rc);

    /**
    * @brief cleanup AES CTR decryptor instance allocated via aesctrNew API

    * @param instance pointer to AES CTR decryptor instance allocated via aesctrNew API

    * @return none
    */

    void MC_EXPORT_API aesctrFree(aes_decryptor_t instance);

    /**
    * @brief sets initialization vector into the AES CTR decrpyptor

    * @param instance pointer to AES CTR decryptor instance allocated via aesctrNew API
    * @param data pointer to the initialization vector
    * @param size length of initialization vector, shall be either 8 or 16 bytes

    * @return 0 if successful, non-zero otherwise
    */

    int32_t MC_EXPORT_API aesctrSetIV(aes_decryptor_t instance, uint8_t * data, uint32_t size);

    /**
    * @brief sets encryption key into the AES CTR decrpyptor

    * @param instance pointer to AES CTR decryptor instance allocated via aesctrNew API
    * @param data pointer to the key
    * @param size length of key in bytes

    * @return 0 if successful, non-zero otherwise
    */

    int32_t MC_EXPORT_API aesctrSetKey(aes_decryptor_t instance, uint8_t * data, uint32_t size);

    /**
    * @brief performs in-place decryption of data

    * @param instance pointer to AES CTR decryptor instance allocated via aesctrNew API
    * @param data pointer to the actual data to be decrypted, same memory is used as output, so should be readable and writable
    * @param size length of key data to be decrypted

    * @return 0 if successful, non-zero otherwise
    */

    int32_t MC_EXPORT_API aesctrDecrypt(aes_decryptor_t instance, uint8_t * data, uint32_t size);
    
#ifdef __cplusplus
}
#endif

#endif /* __DECRYPT_AES_CTR_H__ */
