#include "bufstrm.h"

#ifdef __cplusplus
extern "C" {
#endif

bufstream_tt * open_buf_mpd_file(const char * filename, uint32_t chunksize, void (*DisplayError)(char *txt));
void close_mpd_buf(bufstream_tt* bs, int32_t Abort);
#ifdef __cplusplus
}
#endif