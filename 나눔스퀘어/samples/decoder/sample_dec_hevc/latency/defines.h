#ifndef UUID_42781C18_AE7B_46FD_B586_412C7D285E6C
#define UUID_42781C18_AE7B_46FD_B586_412C7D285E6C

#include <inttypes.h>
#include <stddef.h>

#define MAX_COUNT_VIEWS 255                // The maximum possible number of elements in a latency measurement view
#define MAX_COUNT_PICTURES MAX_COUNT_VIEWS // The maximum possible number of pictures in DPB
#define MAX_COUNT_INSTANCES 64             // The maximum possible number of decoder instances running in parallel
#define MAX_COUNT_STREAMS 16               // The maximum possible number of elementary streams decoded in parallel
#define DEFAULT_CHUNK_SIZE 64 * 1024       // Default chunk size used in the random parsing mode

static const char* PICTURE_TYPE[] = { "B", "P", "I" };

struct Chunk
{
    Chunk(size_t size) noexcept : size(size) {}

    size_t size{}; // Data chunk (NALU, AU, etc.) size in bytes
};

struct NalUnit : public Chunk
{
    NalUnit(size_t size, size_t offset) noexcept : Chunk(size), offset(offset) {}

    size_t offset{}; // NAL unit offset from the beginning of an input bitstream in bytes
};

struct AccessUnit : public Chunk
{
    AccessUnit(size_t size, size_t offset, uint32_t pdc, uint32_t poc, uint32_t lat) noexcept : Chunk(size), offset(offset), pdc(pdc), poc(poc), lat(lat) {}

    size_t offset{}; // AU offset from the beginning of an input bitstream in bytes
    uint32_t pdc{};  // AU decoding order counter
    uint32_t poc{};  // AU output order counter
    uint32_t lat{};  // AU reordering latency
};

struct Picture
{
    enum
    {
        B,
        P,
        I
    }; // Types of decoded pictures

    uint32_t pid{};         // Picture identifier
    uint32_t poc{};         // Picture output order counter
    uint32_t pdc{};         // Picture decoding order counter
    uint32_t prc{};         // Picture release order counter
    uint32_t hits{};        // The number of picture hits
    uint32_t rows{};        // The number of CTU rows in a picture
    uint32_t tiles{};       // The number of tiles in a picture
    uint32_t slices{};      // The number of slices in a picture
    uint32_t segments{};    // The number of segments in a picture
    uint32_t messages{};    // The number of SEI messages in a picture
    uint32_t filters{};     // The number of filters (DBF and/or SAO) enabled in a picture
    uint32_t latency{};     // The number of subsequent decoded pictures preceding the current one in the output order
    uint32_t reordering{};  // The reordering from the picture's SPS
    uint32_t buffering{};   // The buffering from the picture's SPS
    uint32_t capacity{};    // The total number of pictures in DPB
    uint32_t consumption{}; // The number of pictures consumed from DPB
    bool first = false;     // The indicator of the first picture in the measurements range

    // Basic parameters of a picture replacing the current one in DPB
    struct
    {
        int32_t poc = -1; // Picture's output order counter
        int32_t pdc = -1; // Picture's decoding order counter
    } replacement;
};
#endif
