#ifndef UUID_5D51DD14_6C64_4E62_969F_34F358BF123A
#define UUID_5D51DD14_6C64_4E62_969F_34F358BF123A


/**
 * @brief Context info
 * @{
 **/
typedef struct
{
    void *p;	/**<@brief a pointer to an context structure */
} context_t;
/** @} */

//! a pointer to the applications printf routine for messages
typedef void(MC_EXPORT_API* log_printf_t)(context_t context, const char *fmt, ...);

//! a pointer to the applications printf routine for progress messages
typedef void(MC_EXPORT_API* prg_printf_t)(context_t context, int32_t percent, const char * fmt, ...);

//! a pointer to the applications memory allocation, like the standard C malloc function
typedef void* (MC_EXPORT_API* malloc_t)(context_t context, size_t size);
//! a pointer to the applications memory free routine, like the standard C free function
typedef void (MC_EXPORT_API* free_t)(context_t context, void* ptr);

// dead {
//! a pointer to the applications memory allocation, like the standard C realloc function
typedef void* (MC_EXPORT_API* realloc_t)(context_t context, void* ptr, size_t size);

//! a pointer to the applications memory allocation for numa
typedef void*(MC_EXPORT_API* malloc_numa_t)(context_t context, size_t size, int32_t node);
//! a pointer to the applications memory free routine for numa
typedef void(MC_EXPORT_API* free_numa_t)(context_t context, void* ptr);
// }
 
//! a pointer to the applications yield routine to abort an operation
typedef int32_t(MC_EXPORT_API* yield_t)(context_t context);
//! a pointer to the applications license routine to provide license buffer
typedef int32_t(MC_EXPORT_API* get_license_data_t)(context_t context, void * pData);

//---------------------------------- callbacks_t typedef -----------------------------------------
// declared for convenience
/**
@brief Get callbacks functions typedef

Used to provide pointers to message handling, memory management functions for components and etc.
For more information see the @ref GET_CALLBACK_PAGE "dedicated page".

@param[in] context        Pointer to user data
@param[in] name           The name of the function to get the pointer to
@return The pointer to the function
*/
typedef struct
{
    //! User defined instance identifier
    context_t context;
    //! a pointer to the applications memory allocation, like the standard C malloc function
    malloc_t  malloc;
    //! a pointer to the applications memory free routine, like the standard C free function
    free_t    free;
    //! a pointer to the applications memory allocation, like the standard C realloc function
    realloc_t realloc;

    //! a pointer to the applications memory allocation for numa
    malloc_numa_t malloc_numa;
    //! a pointer to the applications memory free routine for numa
    free_numa_t free_numa;

    //! a pointer to the applications printf routine for messages
    log_printf_t err_printf;
    //! a pointer to the applications printf routine for progress messages
    prg_printf_t prg_printf;
    //! a pointer to the applications printf routine for warning messages
    log_printf_t wrn_printf;
    //! a pointer to the applications printf routine for info messages
    log_printf_t inf_printf;
    //! a pointer to the applications printf routine for statistic messages
    log_printf_t stat_printf;

    //! a pointer to the applications license routine to copy license data
    /** @brief get_license_data
    get_license_data has specific user contract. First call of get_license_data callback function is invoked with parameter pData equal to nullptr. 
    For callback's side it means the following - it should return size of license buffer. 
    If zero is returned at the first call it means that there is no license information and MC component will stop license checking. 
    Second call of get_license_data callback is invoked with pData not equal to nullptr. pData should be pointing to an allocated memory handled by caller side. 
    Callback function should fill the memory buffer pointed by pData with license information. 
    Any negative return value from get_license_data callback will be treated as an error on the callback's side and MC component will stop license checking.
    */
    get_license_data_t get_license_data;

    //! reserved, should be zero
    void* reserved[16]; 
} callbacks_t;




// external I/O callbacks for mfimport and demuxers

// called to open an external file
//
// p_app_ptr      - input, the p_app_ptr field from the mc_external_io_t structure
// p_filename     - input, the filename to open
// pp_file_handle - output, handle for the opened file
// return         - 0 if ok, else non-zero

typedef int32_t (*pfn_mc_external_open)(void *p_app_ptr, char *p_filename, void **pp_file_handle);
typedef int32_t (*pfn_mc_external_openW)(void *p_app_ptr, uint16_t *p_filename, void **pp_file_handle);


// called to seek in an external file
//
// p_app_ptr     - input, the p_app_ptr field from the mc_external_io_t structure
// p_file_handle - input, the file handle returned by the open function
// position      - input, the seek position
// return        - 0 if ok, else non-zero

typedef int32_t (*pfn_mc_external_seek)(void *p_app_ptr, void *p_file_handle, int64_t position);


// called to read from an external file
//
// p_app_ptr     - input, the p_app_ptr field from the mc_external_io_t structure
// p_file_handle - input, the file handle returned by the open function
// p_buffer      - input, a buffer for the data
// buffer_size   - input, the number of bytes to read
// return        - number of bytes actually read or 0 to indicate an error occurred

typedef int32_t (*pfn_mc_external_read)(void *p_app_ptr, void *p_file_handle, uint8_t *p_buffer, int32_t buffer_size);


// called to close an external file
//
// NOTE the user must set the file handle to an invalid value before
//      returning. The freed handle may be passed back to this function,
//      the user should check for an invalid value before using the handle
//
// p_app_ptr      - input, the p_app_ptr field from the mc_external_io_t structure
// pp_file_handle - input/output, the file handle returned by the open function
//                  be sure to set this to an invalid value during the close!

typedef void (*pfn_mc_external_close)(void *p_app_ptr, void **pp_file_handle);

#endif