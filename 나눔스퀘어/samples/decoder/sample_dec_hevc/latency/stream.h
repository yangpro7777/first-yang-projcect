#ifndef UUID_75E66C94_1971_4AC1_B079_0EC1C33E343A
#define UUID_75E66C94_1971_4AC1_B079_0EC1C33E343A

#include <vector>
#include <array>
#include <limits>
#include "defines.h"

class Application;
class Instance;

// The range of GOPs for measuring latencies
struct Range
{
    struct
    {
        uint32_t poc = 0;
        uint32_t pdc = 0;
    } min;

    struct
    {
        uint32_t poc = (std::numeric_limits<uint32_t>::max)();
        uint32_t pdc = (std::numeric_limits<uint32_t>::max)();
    } max;
};

class Stream
{
public:
    typedef std::vector<NalUnit> NalUnits;
    typedef std::vector<AccessUnit> AccessUnits;

    Stream(const char* const filename) noexcept : handle(fopen(filename, "rb")) {}

    ~Stream() noexcept
    {
        if (handle) {
            fclose(handle);
            handle = nullptr;
        }
    }

    bool parse(Application* application);

    uint8_t* read(size_t* const size) noexcept
    {
        if (m_capacity < DEFAULT_CHUNK_SIZE)
            m_buffer = (uint8_t*)realloc(m_buffer, m_capacity = DEFAULT_CHUNK_SIZE);

        *size = fread(m_buffer, 1, m_capacity, handle);
        return m_buffer;
    }

    uint8_t* read(const size_t size) noexcept
    {
        if (m_capacity < size)
            m_buffer = (uint8_t*)realloc(m_buffer, m_capacity = size);

        size_t actual_size = fread(m_buffer, 1, size, handle);
        return m_buffer;
    }

    FILE* handle{};                   // File handle for the input bitstream
    Range range{};                    // The range of GOPs in the input bitstream that have to be processed
    NalUnits nalus{};                 // NALUs of the input bitstream
    AccessUnits aus{};                // AUs of the input bitstream
    std::vector<Instance*> instances; // Decoder instances reading from the input bitstream in the loop
    uint32_t loop{};                  // Loop counter

private:
    size_t m_capacity{};
    uint8_t* m_buffer{};
};
#endif
