/**
 * \file   threadpool.h
 * \brief  MainConcept Runtime thread, task and workload management
 *
 * \copyright Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.
 * MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 * This software is protected by copyright law and international treaties.
 * Unauthorized reproduction or distribution of any portion is prohibited by law.
**/

#ifndef UUID_0C2652B8_223A_47EA_89D5_C1016749005D
#define UUID_0C2652B8_223A_47EA_89D5_C1016749005D

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_session_s;
struct mcr_session_pool_s;

/** \brief [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) attributes */
typedef enum mcr_attributes_e
{
    NN_MAX_CAPACITY = 64,                       /**< \brief The maximum number of NUMA nodes */
    HT_MAX_CAPACITY = 64,                       /**< \brief The maximum number of hardware threads (CPUs) in a single NUMA node */
    SP_MAX_CAPACITY = 256                       /**< \brief The maximum number of [slave sessions](\ref mcr_session_pool_t::slave) in a [session pool](\ref mcr_session_pool_t). This value is also used as an index for [the master session](\ref mcr_session_pool_t::master) */
} mcr_attributes_t;

/**
 * \brief Thread pool type. For additional information, refer to [Threads](\ref MCRUNTIME_GLOSSARY_THREADS) section in the Glossary.
 * \details This is the primary configuration setting defining the core capabilities of [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) such as task binding, ordering, prioritization, grouping and stealing.
 * Thread pool types differ in hardware topology awareness, used synchronization primitives, applied lock-free data structures and algorithms, and native OS support.
 * As a result, their performance may significantly vary depending on the workload complexity, available hardware resources and operating system.
 *
 * [The scalable thread pool](\ref TP_TYPE_SCALABLE) is the default type of a thread pool created internally.
 * It typically performs better than [the basic thread pool](\ref TP_TYPE_BASIC) as a result of avoiding locking during enqueue and dequeue operations.
 * It also includes a variety of other multi-threading optimizations which result in higher performance in a variety of circumstances.
 * It is designed to scale better in particular on machines with a high number of CPU cores and/or NUMA architecture.
 * Generally, the more complex the work that is being done, the more beneficial [the scalable thread pool](\ref TP_TYPE_SCALABLE) becomes.
 *
 * [The basic thread pool type](\ref TP_TYPE_BASIC) uses locking primitives for enqueue and dequeue.
 * It may have comparable performance to the [scalable version](\ref TP_TYPE_SCALABLE) or even better when the workload is relatively simple and/or the number of CPU cores on the system is low.
 *
 * [The GCD thread pool type](\ref TP_TYPE_GCD) is a wrapper around the [Grand Central Dispatch framework](https://en.wikipedia.org/wiki/Grand_Central_Dispatch).
 * This thread pool type is currently only supported on OSX platform. If \ref threadpoolCreate or \ref sessionCreate is called with this thread pool type on an unsupported platform, the error will be returned and another type should be used.
 **/
typedef enum tp_type_e
{
    TP_TYPE_AUTO = -1,                          /**< \brief Let [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) choose thread pool type */
    TP_TYPE_SCALABLE = 0,                       /**< \brief Advanced thread pool.
                                                     \details Scales better for heavy and skewed [workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) on complex and assymetric hardware topologies.
                                                     [Threads oversubscribing](\ref MCRUNTIME_GLOSSARY_THREAD_OVERSUBSCRIBING) disallowed */
    TP_TYPE_LOCKFREE = TP_TYPE_SCALABLE,        /**< \brief Advanced thread pool.
                                                     \deprecated Kept until the next major release. Superseded by \ref TP_TYPE_SCALABLE */
    TP_TYPE_BASIC,                              /**< \brief Basic thread pool.
                                                     \details Less overhead for lightweight [workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) on simple hardware topologies.
                                                     [Threads oversubscribing](\ref MCRUNTIME_GLOSSARY_THREAD_OVERSUBSCRIBING) allowed */
    TP_TYPE_LEGACY = TP_TYPE_BASIC,             /**< \brief Basic thread pool.
                                                     \deprecated Kept until the next major release. Superseded by \ref TP_TYPE_BASIC */
    TP_TYPE_GCD,                                /**< \brief [GCD](https://en.wikipedia.org/wiki/Grand_Central_Dispatch) thread pool.
                                                     \details Supported on macOS only */
    TP_TYPE_NONE,                               /**< \brief No thread pool created.
                                                     \details The option can be used by [a session](\ref mcr_session_t) sharing a thread pool with another [session](\ref mcr_session_t) */
    TP_TYPE_COUNT                               /**< \brief The number of thread pool types */
} tp_type_t;

/**
 * \brief Pipeline type. For additional information, refer to [Workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) section in the Glossary.
 * \details This descriptive setting reports to [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) how the workload is distributed over threads in [processing](\ref MCRUNTIME_GLOSSARY_PROCESSING_LOOP) [control](\ref MCRUNTIME_GLOSSARY_CONTROL_LOOP) loops.
 * \note The setting have to coincide with [SMP configuration setting](\ref SET_SMP_MODE) used by [auxinfo call](\ref auxinfo_t) to configure a MainConcept Component.
**/
typedef enum pl_type_e
{
    PL_TYPE_AUTO = -1,                          /**< \brief Let [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) choose pipeline type */
    PL_TYPE_SERIAL = 0,                         /**< \brief Serial pipeline
                                                     \details Process single breakdown (e.g., a slice) of a single unit (e.g., a picture) one by one in the context of a single calling thread.
                                                     Zero worker threads created */
    PL_TYPE_OVERLAPPED,                         /**< \brief Overlapped parallel pipeline
                                                     \details Process multiple breakdowns (e.g., slices) of multiple units (e.g., pictures) in parallel in the context of multiple worker threads.
                                                     The maximum number of worker threads depends on [thread pool type](\ref tp_type_t) */
    PL_TYPE_CONCURRENT,                         /**< \brief Solitary parallel pipeline
                                                     \details Process multiple breakdowns (e.g., slices) of a single unit (e.g., a picture) in parallel in the context of multiple worker threads.
                                                     The maximum number of worker threads depends on [thread pool type](\ref tp_type_t) */
    PL_TYPE_COUNT                               /**< \brief The number of pipeline types */
} pl_type_t;

/**
 * \brief Coroutine type. For additional information, refer to [Tasks](\ref MCRUNTIME_GLOSSARY_TASKS) section in the Glossary.
 * \details This configuration setting defines the stack model for [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) tasks.
 *
 * \warning This setting have to be used with care to avoid deadlocks. It also may have no effect if a particular [MainConcept Component](\ref MCRUNTIME_ADOPTION) doesn't support it
**/
typedef enum cr_type_e
{
    CR_TYPE_AUTO = -1,                          /**< \brief Let [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) choose coroutine type */
    CR_TYPE_STACKFUL = 0,                       /**< \brief Stackful coroutines or tasks a.k.a. fibers or user-space threads.
                                                     \details Imply unfair cooperative scheduling and allow to suspend and resume the execution at arbitrary place without switching to kernel.
                                                     Induce small amount of system calls and context switches. Never block a calling thread. Use light atomics and CAS for synchronization.
                                                     By default, fibers reuse stacks from a dedicated pool, thus reducing the memory footprint and fragmentation.
                                                     If the pool is created empty each fiber allocates its own stack and stacks are not reused.
                                                     The decision on the stack size and the pool capacity is made internally. */
    CR_TYPE_STACKLESS,                          /**< \brief Stackless coroutines or tasks.
                                                     \details Imply fair preemptive scheduling based on stack management support from kernel.
                                                     Induce large amount of system calls and context switches. Can block a calling thread. Use heavy system events for synchronization. */
    CR_TYPE_COUNT                               /**< \brief The number of task types */
} cr_type_t;

/**
 * \brief Workload rate control. For additional information, refer to [Workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) section in the Glossary.
 * \details This descriptive setting reports to [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) a target workload processing rate.
 * Based on the setting and [processing rate limit](\ref mcr_session_t::rate) the runtime may optimize CPU usage by reducing CPU stolen/idle cycles.
 * \note The default idling policies, applied when no explicit requirements were provided, are disabled by \ref WL_FLAG_NO_DEFAULT_IDLING flag.
**/
typedef enum rc_type_e
{
    RC_TYPE_AUTO = -1,                          /**< \brief Let [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) to control the workload processing rate */
    RC_TYPE_FIXED = 0,                          /**< \brief Process the workload with a fixed rate */
    RC_TYPE_BELOW,                              /**< \brief Process the workload with a rate below the upper limit. The target rate is set 3 times below the upper limit */
    RC_TYPE_ABOVE,                              /**< \brief Process the workload with a rate above the lower limit. The target rate is set 3 times above the lower limit */
    RC_TYPE_COUNT
} rc_type_t;

/**
 * \brief Workload complexity hints. For additional information, refer to [Workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) section in the Glossary.
 * \details This descriptive setting is a valuable thread and task management subjective hint for [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT).
 * It is complemented by [scheduling requirements](\ref wl_flag_t). By default, the runtime enables tools optimized for complex workloads and hardware topologies.
 * Thus, specifying the complexity hints may become helpful for simple workloads and topologies when advanced tools bring more overhead than improvement and are better to be disabled.
 *
 * The workload complexity hints are described by 32-bit unsigned value divided in 8 ranges each with 4 bits inside.
 * The meaning of each individual bit in the value is not fixed by the runtime. It is up to an application to assign it a meaning with only one requirement: the complexity increases from LSB towards MSB.
 * Bitmasks from the enumeration help to categorize workload complexity bits in terms of system resources consumption. One can use it as a guideline when toggling bits.
 * \warning Complexity hints are optional! They are not required to have precise value or be consistent across different workloads, hardware topologies and/or operating systems.
 * They can be set to \ref WL_TYPE_AUTO if an application has no needs or means to compute hints. The runtime will continue to adaptively handle all input workloads.
 * Just keep in mind that sometimes the performance might be suboptimal in this case.
 **/
typedef enum wl_type_e
{
    WL_TYPE_ZERO   = 0x00000000,                /**< \brief Bitmask for infinitly small workload features<br> Has no effect. Reserved for future use. */
    WL_TYPE_MINOR  = 0x0000000F,                /**< \brief Bitmask for a workload features consuming minimum system resources<br> Require optimal scheduling. Features are described using bits 0-3 */
    WL_TYPE_SMALL  = 0x000000FF,                /**< \brief Bitmask for a workload features consuming small amount of system resources<br> Extra features are described using bits 4-7 */
    WL_TYPE_LIGHT  = 0x00000FFF,                /**< \brief Bitmask for a workload features consuming less than a half of system resources<br> More extra features are described using bits 8-11 */
    WL_TYPE_MODEST = 0x0000FFFF,                /**< \brief Bitmask for a workload features consuming a half of system resources<br> Require optimal load balancing. More extra features are described using bits 12-15 */
    WL_TYPE_HEAVY  = 0x000FFFFF,                /**< \brief Bitmask for a workload features consuming more than a half of system resources<br> More extra features are described using bits 16-19 */
    WL_TYPE_LARGE  = 0x00FFFFFF,                /**< \brief Bitmask for a workload features consuming large amount of system resources<br> More extra features are described using bits 20-23 */
    WL_TYPE_MAJOR  = 0x0FFFFFFF,                /**< \brief Bitmask for a workload features consuming maximum system resources<br> Require optimal prioritization. More extra features are described using bits 24-27 */
    WL_TYPE_AUTO   = WL_TYPE_MAJOR              /**< \brief Bitmask for infinitly large workload<br> Let [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) decide on workload complexity */
} wl_type_t;

/**
 * \brief Workload scheduling preferences. For additional information, refer to [Workloads](\ref MCRUNTIME_GLOSSARY_WORKLOADS) section in the Glossary.
 * \details This bit flags are complementary to [workload complexity hints](\ref wl_type_t) and opposed to them have the concrete and fixed meaning. Flags use bits 28-31
 * \warning The preferences are optional and mostly used to adjust [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) default policies.
 * However, if set they have highest precedence overriding all other settings. Use them with care to avoid speed regressions
**/
typedef enum wl_flag_e
{
    WL_FLAG_NO_DEFAULT_BINDING  = 0x80000000,   /**< \brief Disable implicit binding policies otherwise applied by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) by default */
    WL_FLAG_NO_DEFAULT_SHARING  = 0x40000000,   /**< \brief Disable implicit sharing policies otherwise applied by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) by default */
    WL_FLAG_NO_DEFAULT_IDLING   = 0x20000000,   /**< \brief Disable implicit yielding policies otherwise applied by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) by default */
    WL_FLAG_NO_DEFAULT_AFFINITY = 0x10000000    /**< \brief Disable implicit affinity policies otherwise applied by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) by default */
} wl_flag_t;

/**
 * \brief Control loop entry point. For additional information, refer to [dedicated article](\ref MCRUNTIME_GLOSSARY_CONTROL_LOOP) in the Glossary.
 * \details The entry point function is provided by an application and contains the implementation of a component control loop.
 * The function is invoked asynchronously by the runtime in the context of a thread calling \ref sessionStart function.
 * The created control loop task is scheduled by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) together with processing tasks.
 * So, the control loop is offloaded from the application to the runtime. In typical cases it brings a better control over load balancing and data transfers between control and processing tasks.
 *
 * \warning The control loop occupies one worker thread. If it never suspends execution or [yields control](\ref sessionYield) the worker thread becomes unavailable for other tasks.
 * The more component instances with offloaded control loops are running the more worker threads are endlessly occupied. An application should take care of this to avoid deadlocks.
 * For example, it may periodically [yield control](\ref sessionYield) back to the scheduler or just stop offloading the control loop if yielding is not a choice.
 * Another option is to consider worker threads oversubscribing supported by [the basic thread pool](\ref TP_TYPE_BASIC)
**/
typedef void (*mcr_session_routine_t)(struct mcr_session_s* session);

/**
 * \brief [Session](\ref mcr_session_t) user data
 * \details It is not used or modified by [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT).
 * User is responsible for allocation and deallocation of resources associated with the user data.
**/
typedef void* mcr_session_userdata_t;

/**
 * \brief [Session](\ref mcr_session_t) affinity bitmask
 * \details An affinity bitmask describes the availability of NUMA nodes and/or CPUs for a [session](\ref mcr_session_t).
 * If a node or CPU is available the appropriate bit in logical order is set to 1. Otherwise, it is set to 0.
 * Up to \ref NN_MAX_CAPACITY NUMA nodes and up to \ref HT_MAX_CAPACITY x \ref NN_MAX_CAPACITY CPUs supported.
**/
typedef uint64_t mcr_bitmask_t;

/**
 * \brief Workload processing session. For additional information, refer to [Sessions](\ref MCRUNTIME_GLOSSARY_SESSIONS) section in the Glossary
 * \details It is the basic unit for [Session Pool API](\ref MCRUNTIME_FRONTEND_SESSIONPOOL) and is usually referred to as a <em>managed thread pool</em>.
 * On top of the [unmanaged thread pool](\ref mcr_thread_pool_t) it puts additional options for [processing loop](\ref MCRUNTIME_GLOSSARY_PROCESSING_LOOP) configuration.
 * Sessions can be used in three different scenarious:
 * \arg <em>As an unmanaged thread pool</em>.
 *      Only [threadpool](\ref mcr_session_t::threadpool) member of the structure is initialized with a call to \ref threadpoolCreate function.
 *      All other members are left at default values.
 *      Session itself is NOT created.
 * \arg <em>As a managed thread pool or a standalone session</em>.
 *      Only [pool](\ref mcr_session_t::pool) member of the structure is left uninitialized, which indicates that the session is standalone.
 *      All other members are properly initialized.
 *      Session is created with a call to \ref sessionCreate function.
 * \arg <em>As a member of a session pool</em>.
 *      [pool](\ref mcr_session_t::pool) member of the structure is initialized with a pointer to [a pool of sessions](\ref mcr_session_pool_t).
 *      [index](\ref mcr_session_t::index) member for slave sessions is set to a value reflecting a position in the pool.
 *      Session is created with a call to \ref sessionCreate function.
**/
typedef struct mcr_session_s
{
    mcr_object_t handle;                        /**< \brief Session descriptor
                                                     \details \ref sessionCreate function writes to this member. In some cases a valid session can have a zero descriptor */
    mcr_object_t affinity;                      /**< \brief Session affinity descriptor
                                                     \details \ref sessionCreate function writes to this member. In some cases a valid session can have a zero affinity descriptor */
    struct mcr_session_pool_s* pool;            /**< \brief \copybrief mcr_session_pool_t
                                                     \details \ref sessionCreate function reads this member. To create a standalone session, it must be zero */
    callbacks_t callbacks;                      /**< \brief Resource management callbacks
                                                     \details \ref sessionCreate function reads this member */
    mcr_bitmask_t cpuset[NN_MAX_CAPACITY];      /**< \brief Compound CPU [affinity bitmask](\ref mcr_bitmask_t)
                                                     \details \ref sessionCreate function reads and optionally modifies this member to create and initialize an affinity descriptor for the [thread pool](\ref mcr_thread_pool_t).
                                                     Make sure to update this member appropriately and disable [default affinity policy](\ref WL_FLAG_NO_DEFAULT_AFFINITY) to reflect the desired affinity before calling \ref sessionCreate.
                                                     Otherwise, if \ref WL_FLAG_NO_DEFAULT_AFFINITY is not set [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) applies default affinity policy.
                                                     \note An empty cpuset is equivalent to a complete cpuset. Conflicts between the cpuset and nodeset are resolved towards the narrower affinity range. */
    mcr_bitmask_t nodeset;                      /**< \brief NUMA [affinity bitmask](\ref mcr_bitmask_t)
                                                     \details \ref sessionCreate function reads and optionally modifies this member to create and initialize affinity descriptor and/or [thread pool](\ref mcr_thread_pool_t).
                                                     Make sure to update this member appropriately to reflect the desired affinity before calling \ref sessionCreate.
                                                     Otherwise, if \ref WL_FLAG_NO_DEFAULT_AFFINITY is not set [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) applies default affinity policy.
                                                     Provided \ref WL_FLAG_NO_DEFAULT_BINDING is not set, the worker threads will be individually binded to different nodes within the configured nodeset for efficient workload distribution.
                                                     \note Conflicts between the cpuset and nodeset are resolved towards the narrower affinity range.
                                                     \c nodeset == -1: Global affinity (use all NUMA nodes)
                                                     \c nodeset == 0: Process affinity (use nodes already assigned to a process)
                                                     \c nodeset > 0: Explicit affinity (mask of nodes to use) */
    uint32_t workers;                           /**< \brief The number of worker threads in the session
                                                     \details \ref sessionCreate function reads and optionally modifies this member and applies it on [thread pool](\ref mcr_thread_pool_t) creation */
    uint32_t index;                             /**< \brief Session index in the pool
                                                     \details \ref sessionCreate function reads this member and applies it on [thread pool](\ref mcr_thread_pool_t) creation.
                                                     [The master session](\ref mcr_session_pool_t::master) has the greatest index equal to \ref SP_MAX_CAPACITY.
                                                     [Slave session](\ref mcr_session_pool_t::slave) index must always be less than \ref SP_MAX_CAPACITY */
    double rate;                                /**< \brief Processing rate limit measured in units processed in a second
                                                     \details \ref sessionCreate function reads this member */
    mcr_thread_pool_t threadpool;               /**< \brief \copybrief mcr_thread_pool_t
                                                     \details \ref sessionCreate function writes to this member. It can be zero for a session in [a session pool](\ref mcr_session_pool_t) */
    mcr_session_routine_t routine;              /**< \brief \copybrief mcr_session_routine_t
                                                     \details \ref sessionCreate function reads this member which is later used to offload [a control loop](\ref MCRUNTIME_GLOSSARY_CONTROL_LOOP) from an application.
                                                     This routine must have stack size less or equal to the limit specified by \ref mcr_session_t::stack field. It runs inside a task scheduled by
                                                     [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) together with workload processing tasks which becomes noticeably advantageous in terms of performance when
                                                     the processed workload is sparse and does not consume all hardware resources.
                                                     Should not be used with [serial pipeline](\ref PL_TYPE_SERIAL)\ref */
    mcr_session_userdata_t userdata;            /**< \brief \copybrief mcr_session_userdata_t
                                                     \details \ref sessionCreate function does not touch this member */
    tp_type_t type;                             /**< \brief \copybrief tp_type_t
                                                     \details \ref sessionCreate function reads this member and applies it on [thread pool](\ref mcr_thread_pool_t) creation */
    pl_type_t pipeline;                         /**< \brief \copybrief pl_type_t
                                                     \details \ref sessionCreate function reads this member */
    cr_type_t coroutine;                        /**< \brief \copybrief cr_type_t
                                                     \details \ref sessionCreate function reads this member */
    rc_type_t ratecontrol;                      /**< \brief \copybrief rc_type_t
                                                     \details \ref sessionCreate function reads this member */
    uint32_t workload;                          /**< \brief The conjunction of [workload complexity bits](\ref wl_type_t) and [scheduling preferences bits](\ref wl_flag_t)
                                                     \details \ref sessionCreate function reads this member. [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) may use this member to adjust default policies. */
    uint32_t stack;                             /**< \brief The stack size in bytes of tasks executed in the session
                                                     \details All tasks executed in the session and its [control loop routine](\ref mcr_session_t::routine) must never exceed this limit. Otherwise, the stack will get corrupted.
                                                     \note This member is initialized to the default value by \ref sessionDefault function. If it is later set to zero, [Multiprocessing Runtime](\ref MCRUNTIME_INTRODUCTION_ABSTRACT) still applies the default value. */
    uint8_t reserved[60];                       /**< \brief Shall be zero. Otherwise it may cause undefined behavior */
} mcr_session_t;

/** \brief Pool of workload processing sessions. For additional information, refer to [Sessions](\ref MCRUNTIME_GLOSSARY_SESSIONS) section in the Glossary */
typedef struct mcr_session_pool_s
{
    mcr_session_t master;                       /**< \brief Master session (optionally) shared by multiple slave sessions */
    mcr_session_t slave[SP_MAX_CAPACITY];       /**< \brief Persistent processing sessions */
    uint32_t size;                              /**< \brief The number of slave sessions in the pool */
    uint8_t reserved[60];                       /**< \brief Shall be zero. Otherwise it may cause undefined behavior */
} mcr_session_pool_t;

/**
 * \brief Call this function to create [a thread pool](\ref mcr_thread_pool_t) instance of [a specified type](\ref tp_type_t)
 * \details The thread pool instance should later be destroyed with \ref threadpoolDestroy.
 *
 * \param[in] callbacks [resource management callbacks](\ref callbacks_t)
 * \param[in] type [thread pool type](\ref tp_type_t). In the typical case, \ref TP_TYPE_SCALABLE should be used
 * \param[in] reserved for future usage (currently must be set to 0)
 * \param[out] threadpool created thread pool instance
 * \return \ref MCR_ERROR_OK if thread pool was created
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API threadpoolCreate(const callbacks_t* callbacks, tp_type_t type, void *reserved, mcr_thread_pool_t* threadpool);

/**
 * \brief Call this function to create [a thread pool](\ref threadpool_t) instance of [a specified type](\ref tp_type_t)
 * \deprecated Kept until the next major release. Superseded by \ref threadpoolCreate function
 *
 * \param[in] callbacks [resource management callbacks](\ref callbacks_t)
 * \param[in] type [thread pool type](\ref tp_type_t). In the typical case, \ref TP_TYPE_LOCKFREE should be used
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return a handle to the created threadpool instance if successful
 * \return NULL if unsuccessful
*/
threadpool_t MC_EXPORT_API threadPoolCreate(const callbacks_t* callbacks, tp_type_t type, void* reserved);

/**
 *\brief Call this function to destroy [a thread pool](\ref mcr_thread_pool_t) instance
 *
 * \param[in] threadpool thread pool instance to be destroyed
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_OK if thread pool was destroyed
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API threadpoolDestroy(mcr_thread_pool_t threadpool, void* reserved);

/**
 * \brief Call this function to destroy [a thread pool](\ref threadpool_t) instance
 * \deprecated Kept until the next major release. Superseded by \ref threadpoolDestroy function
 *
 * \param[in] threadpool threadpool instance to be destroyed
*/
void MC_EXPORT_API threadPoolDestroy(threadpool_t threadpool);

/**
 * \brief Call this function to create [a session](\ref mcr_session_t) instance.
 * If a [session pool](\ref mcr_session_pool_t) is used, the master and all slave [sessions](\ref mcr_session_t) in the pool must be configured prior a call to this function.
 * \warning Once the session is created its fields must not be modified! Otherwise, the behavior is undefined.
 *
 * \param[in] callbacks [resource management callbacks](\ref callbacks_t)
 * \param[in, out] session created session instance
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_OK if session was created
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API sessionCreate(const callbacks_t* callbacks, mcr_session_t* session, void* reserved);

/**
 * \brief Call this function to reset [a session](\ref mcr_session_t) instance to default values.
 * \details Call to this function have to preceed a call to \ref sessionCreate function.

 * \param[in, out] session workload processing session
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return [an operation status](\ref mcr_error_t)
 * \warning By default [session index](\ref mcr_session_t::index) is set to \ref SP_MAX_CAPACITY. Set the [index](\ref mcr_session_t::index) to smaller value for a [slave session](\ref mcr_session_pool_t::slave)
**/
mcr_error_t MC_EXPORT_API sessionDefault(mcr_session_t* session, void* reserved);

/**
 * \brief Call this function to start [a session](\ref mcr_session_t).
 * \details For this function to take an effect [the session](\ref mcr_session_t) must be created with [control loop entry point](\ref mcr_session_routine_t).
 * The control loop is offloaded from an application and is scheduled by the runtime together with the processing loop.
 *
 * \param[in] session workload processing session
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_INVALIDARG if [the session](\ref mcr_session_t) was created without [entry point](\ref mcr_session_routine_t)
 * \return \ref MCR_ERROR_OK if the session entry point was invoked
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API sessionStart(mcr_session_t* session, void* reserved);

/**
 * \brief Call this function to wait for [a session](\ref mcr_session_t) to complete.
 * \details For this function to take an effect [the session](\ref mcr_session_t) must be created with [control loop entry point](\ref mcr_session_routine_t).
 *
 * \param[in] session workload processing session
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_OK if the session entry point completed successfully
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API sessionWait(mcr_session_t* session, void* reserved);

/**
 * \brief Call this function to yield execution of [control loop entry point](\ref mcr_session_routine_t).
 * \details For this function to take an effect [the session](\ref mcr_session_t) must be created with [control loop entry point](\ref mcr_session_routine_t).
 * Periodically calling this function an application yields control loop task and lets other processing tasks to resume.
 * The less processing cores are available to the application and/or the more component instances it runs the more critical becomes the call to this function.
 *
 * \param[in] session workload processing session
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_UNAVAILABLE if no work was done
 * \return \ref MCR_ERROR_OK if any work was done
 * \return [error status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API sessionYield(mcr_session_t* session, void* reserved);

/**
 * \brief Call this function to destroy [a session](\ref mcr_session_t).
 * \note [The session](\ref mcr_session_t) should have been created with a call to \ref sessionCreate function which is not always the case. For example, when [a session](\ref mcr_session_t) is used as a wrapper for a [thread pool](\ref mcr_thread_pool_t)
 *
 * \param[in] session session instance to be destroyed
 * \param[in] reserved for future usage (currently must be set to 0)
 * \return \ref MCR_ERROR_OK if session was destroyed
 * \return [an operation status](\ref mcr_error_t), otherwise
**/
mcr_error_t MC_EXPORT_API sessionDestroy(mcr_session_t* session, void* reserved);

#ifdef __cplusplus
}
#endif

#endif
