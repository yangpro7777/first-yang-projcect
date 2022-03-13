#ifndef UUID_A410EBD2_8CC0_47E4_97A3_1CDDE13A0792
#define UUID_A410EBD2_8CC0_47E4_97A3_1CDDE13A0792

#include "mccallbacks.h"

#ifndef __APPLE__
#ifdef __GNUC__
#pragma pack(push,1)
#else
#pragma pack(push)
#pragma pack(1)
#endif
#endif

typedef struct
{
    pfn_mc_external_open p_open;
    pfn_mc_external_openW p_openW;
    pfn_mc_external_seek p_seek;
    pfn_mc_external_read p_read;
    pfn_mc_external_close p_close;

    int64_t file_size; // size of the file in bytes
    void* p_app_ptr;   // application defined pointer passed back to the external I/O callbacks

} mc_external_io_t;

#ifndef __APPLE__
#pragma pack(pop)
#endif

#endif