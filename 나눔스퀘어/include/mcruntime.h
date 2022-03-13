/**
 * \file   mcruntime.h
 * \brief  MainConcept Multiprocessing Runtime Frontend
 *
 * \copyright Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.
 * Unauthorized reproduction or distribution of any portion is prohibited by law.
**/

#ifndef UUID_40FDB791_D395_4A32_A813_00980B4FFF0F
#define UUID_40FDB791_D395_4A32_A813_00980B4FFF0F

#include "mcapiext.h"
#include "mcdefs.h"

/**
 * \brief Runtime object handle
**/
typedef void* mcr_object_t;

/**
 * \brief Thread pool. For additional information, refer to [Threads](\ref MCRUNTIME_GLOSSARY_THREADS) section in the Glossary
 * \details It is the basic unit for [Thread Pool API](\ref MCRUNTIME_FRONTEND_THREADPOOL) and is usually referred to as an <em>unmanaged thread pool</em>.
 * An application can create a thread pool by calling \ref threadpoolCreate function and destroy it at the end by calling \ref threadpoolDestroy function.
 *
 * Thread pool creation can be customized by specifying [thread pool type](\ref tp_type_t). One may want to use [native GCD queue](\ref TP_TYPE_GCD) on macOS, for example.
 *
 * Multiple instances of [MainConcept Components](\ref MCRUNTIME_ADOPTION) may share a single instance of the thread pool.
 * It allows to reduce resource consumption and thread contention which may result in better performance and lower latency.
 * However, all sharing benefits may be invalidated by suboptimal interaction of worker threads running [component processing loop](\ref MCRUNTIME_GLOSSARY_PROCESSING_LOOP) with custom I/O threads running [application control loop](\ref MCRUNTIME_GLOSSARY_CONTROL_LOOP).
 * The more asymmetric the hardware topology is the more noticeable it becomes. The [Session Pool API](\ref MCRUNTIME_FRONTEND_SESSIONPOOL) provides some options to work on this problem.
**/
typedef mcr_object_t mcr_thread_pool_t;

/**
 * \brief Thread pool
 * \deprecated This is the old type definition for the thread pool. Kept until the next major release. Superseded by \ref mcr_thread_pool_t type.
**/
typedef mcr_thread_pool_t threadpool_t;

#include "mcruntime/common.h"
#include "mcruntime/threadpool.h"

#endif
