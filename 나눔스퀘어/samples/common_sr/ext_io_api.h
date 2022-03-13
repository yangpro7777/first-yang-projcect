/**
 @file  ext_io_api.h
 @brief Common external IO API functionality

 @verbatim
 File: ext_io_api.h

 Desc: Common external IO API functionality

 Copyright (c) 2020 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.  Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#ifndef UUID_7B27DC94_F902_40EC_B27F_1CE1912A1054
#define UUID_7B27DC94_F902_40EC_B27F_1CE1912A1054

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <mutex>
#include <array>

#include <mfimport.h>

// local index structure
struct index_entry_t
{
    int64_t file_pos;
    uint8_t frame_type;
    int32_t coding_order;
    int32_t display_order;

    uint8_t flags;
    int8_t temporal_offset;
    int8_t key_frame_offset;

    int32_t frame_size;
};

enum ExternalIoAPIMode
{
    NoExtIoAPI = 0,
    NewExtIoAPI = 1,
    OldExtIoAPI = 2,
};

class ExtIOAPIFeeder
{
public:
    ExtIOAPIFeeder() = delete;

    static int32_t externalFrameGetFrameInfo(void* data, int64_t storage_frame, externalFrameInfo* frame_info);
    int32_t makeIndex();
    static std::shared_ptr<ExtIOAPIFeeder> createFeeder(
        const callbacks_t& callbacks, mpegInInfo* mfi, const std::string& filename, ExternalIoAPIMode ext_io_api_mode);

    int64_t m_total_frames{0};
    std::vector<index_entry_t> m_index;
    mcmediatypes m_stream_type{mctUnknown};
    std::string m_index_file_name;
    int64_t m_file_size{0};

protected:
    ExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name);
    virtual ~ExtIOAPIFeeder() = default;

    template <typename T>
    long openFile(const T* filename, long id)
    {
        m_files[id].open(filename, std::ifstream::binary);
        if (!m_files[id].is_open() || !m_files[id].good()) {
            return -1;
        }
        return 0;
    };

    virtual void closeFile(long file_id);

    std::string m_file_name;
    callbacks_t m_callbacks;

    // Some STL container having a dynamic size should have been used instead of "std::array".
    // But this is not possible due to incomplete C++11 support in GCC 4.8 which we still support.
    std::array<std::ifstream, 1024> m_files;
};

class NewExtIOAPIFeeder : public ExtIOAPIFeeder
{
public:
    NewExtIOAPIFeeder() = delete;

    NewExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name);
    ~NewExtIOAPIFeeder() override = default;

    static int32_t externalFrameOpen(void* data, externalFrameStreamInfo* stream_info);
    static uint8_t* externalFrameRead(void* data, int64_t storage_frame, int32_t& frame_size);
    static void externalFrameClose(void* data);

private:
    static constexpr long m_id = 0;
    long openFile(const char* filename);

    std::vector<uint8_t> m_frame_buffer;
};

class OldExtIOAPIFeeder : public ExtIOAPIFeeder
{
public:
    OldExtIOAPIFeeder() = delete;

    OldExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name, int64_t scale = DEFAULT_SCALE);
    ~OldExtIOAPIFeeder() override = default;

    template <typename T>
    static long externalIoOpen(void* app_data, T* filename)
    {
        auto instance = reinterpret_cast<OldExtIOAPIFeeder*>(app_data);
        return instance->openFile(filename);
    }

    static int32_t externalIoSeek(void* app_data, long file_handle, int64_t position);
    static int32_t externalIoRead(void* app_data, long file_handle, uint8_t* buffer, int32_t buffer_size);
    static void externalIoClose(void* app_data, long file_handle);
    const int64_t m_scale;
    static constexpr int64_t DEFAULT_SCALE{0x3fffffffffffffff};

private:
    static constexpr long max_file_count{1024};
    std::mutex m_mtx_files;
    long getNextId();
    void closeFile(long id) override;

    template <typename T>
    long openFile(const T* filename)
    {
        std::lock_guard<std::mutex> lock(m_mtx_files);
        long id = getNextId();
        if (id == -1) {
            return -1;
        }
        if (ExtIOAPIFeeder::openFile(filename, id)) {
            return -1;
        }
        return id;
    }
};

#endif
