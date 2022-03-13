/**
 * \file   common.h
 * \brief  MainConcept Runtime common declarations
 *
 * \copyright Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.
 * Unauthorized reproduction or distribution of any portion is prohibited by law.
**/

#ifndef UUID_306EB49B_1293_4137_B438_5DE09D1E82A4
#define UUID_306EB49B_1293_4137_B438_5DE09D1E82A4

#include <mcapiext.h>
#include <mcdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Error codes returned by [Multiprocessing Runtime API](\ref MCRUNTIME_GLOSSARY_PUBLIC_API) functions */
typedef enum mcr_error_e
{
    MCR_ERROR_OK = 0,           /**< \brief A function call succeeded */
    MCR_ERROR_OUTOFMEMORY,      /**< \brief Not enough memory to complete a function call */
    MCR_ERROR_INVALIDARG,       /**< \brief One or more arguments passed to a function are invalid */
    MCR_ERROR_UNAVAILABLE,      /**< \brief A resource required to complete a function call is not available or blocked */
    MCR_ERROR_INVALID_AFFINITY  /**< \brief An affinity bitmask is empty or invalid */
} mcr_error_t;

#ifdef __cplusplus
}
#endif

#endif
