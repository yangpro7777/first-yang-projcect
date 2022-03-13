/**
 @file  ext_io_api.cpp
 @brief Common external IO API functionality

 @verbatim
 File: ext_io_api.cpp

 Desc: Common external IO API functionality

 Copyright (c) 2020 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.  Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#include <algorithm>
#include <functional>
#include <stdexcept>
#if !defined(_WIN32)
#include <sys/stat.h>
#endif

#include "ext_io_api.h"

void ExtIOAPIFeeder::closeFile(long id)
{
    if (m_files[id].is_open()) {
        m_files[id].close();
    }
}

ExtIOAPIFeeder::ExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name)
  : m_callbacks(callbacks)
  , m_file_name(file_name)
{
}

int32_t ExtIOAPIFeeder::externalFrameGetFrameInfo(void* data, int64_t storage_frame, externalFrameInfo* frame_info)
{
    auto instance = reinterpret_cast<NewExtIOAPIFeeder*>(data);
    if (!frame_info) {
        return 1;
    }

    if (storage_frame < 0 || storage_frame >= instance->m_total_frames) {
        return 1;
    }

    frame_info->flags = instance->m_index[storage_frame].flags;
    frame_info->temporal_offset = instance->m_index[storage_frame].temporal_offset;
    frame_info->key_frame_offset = instance->m_index[storage_frame].key_frame_offset;

    return 0;
}

int32_t ExtIOAPIFeeder::makeIndex()
{
    std::unique_ptr<mpegInInfo, decltype(&mpegInFree)> pInfo{mpegInCreate(&m_callbacks, nullptr), mpegInFree};
    if (!pInfo) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to create an mfimport instance\n");
        return 1;
    }

    pInfo->FastAccess = 0;
    pInfo->use_external_frame_buffer = 1;

    if (mpegInOpen(pInfo.get(), const_cast<char*>(m_file_name.c_str()))) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to open input file\n");
        return 1;
    }

    if (!pInfo->videoAvail) {
        m_callbacks.err_printf(m_callbacks.context, "No video stream in input file\n");
        return 1;
    }

    if (pInfo->program_stream_flag) {
        m_callbacks.err_printf(m_callbacks.context, "Only elementary video streams are supported\n");
        return 1;
    }

    m_stream_type = pInfo->video_streams[0].stream_type;
    m_total_frames = pInfo->totalFrames;

    m_index_file_name = m_file_name + ".idx";

    if (mpegInSaveIdx(pInfo.get(), nullptr, const_cast<char*>(m_index_file_name.c_str()))) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to save index file\n");
        return 1;
    }

#if defined(_WIN32)
    struct _stati64 stat_data = {};
    _stati64(m_file_name.c_str(), &stat_data);
#else
    struct stat stat_data = {};
    stat(m_file_name.c_str(), &stat_data);
#endif

    if (stat_data.st_size <= 0) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to get the size of the input file\n");
        return 1;
    }

    m_file_size = stat_data.st_size;

    std::ifstream idx_file;
    idx_file.open(m_index_file_name, std::ofstream::binary);
    if (!idx_file.is_open() || !idx_file.good()) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to open index file\n");
        return 1;
    }

    if (!idx_file.seekg(sizeof(video_stream_info))) {
        m_callbacks.err_printf(m_callbacks.context, "Unable to seek to start of index in index file\n");
        return 1;
    }

    m_index.resize(m_total_frames);

    for (int64_t idx = 0; idx < m_total_frames; ++idx) {
        video_au_info au{};
        if (!idx_file.read(reinterpret_cast<char*>(&au), sizeof(au))) {
            m_callbacks.err_printf(m_callbacks.context, "Unable to read index entry from index file\n");
            m_index.clear();
            return 1;
        }
        /*
        StorageFrame: 0 PresentationFrame: 2 TemporalOffset: 1 Flags: 0xc0 (I)
        KeyFrame Offset: 0 StorageFrame: 1 PresentationFrame: 0 TemporalOffset: 1
        Flags: 0x33 (B) KeyFrame Offset: 1 StorageFrame: 2 PresentationFrame: 1
        TemporalOffset: -2 Flags: 0x33 (B) KeyFrame Offset: 2 StorageFrame: 3
        PresentationFrame: 5 TemporalOffset: 1 Flags: 0x22 (P) KeyFrame Offset: 3

        The main idea is that the array of entries represents the STORAGE frames-the
        Nth entry represents the Nth storage frame. But using the temporal offset
        field, the Nth entry of the same array tells you which storage frame is used
        for the Nth presentation frame.  That is, the storage frame used for the Nth
        presentation frame is N + TemporalOffset[N].

        In the example above:
        Which storage frame is used for presentation 0?  0 + temporal offset 1 = 1,
        which is a B frame. Which storage frame is used for presentation 1?  1 +
        temporal offset 1 = 2, which is a B frame. Which storage frame is used for
        presentation 2?  2 + temporal offset -2 = 0, which is an I frame. Which
        storage frame is used for presentation 3?  3 + temporal offset 1 = 4, which
        is going to be a B frame in the next entry (not shown).

        Notice that presentation for this stream starts with two B frames that have
        flag bit 5 set, meaning they contain forward predictions from a previous
        frame that is not in the stream, so these first two frames cannot be
        decoded.  Often the first two B frames will be marked 0x10; bit 5 is clear,
        and bit 4 is set meaning that the frames contain only backward predictions
        from a future frame, namely the I frame at the start of the stream, so these
        frames can be decoded.  Also note that the keyframe offsets are positive
        numbers (that's of course not a big deal if you want negative values).

        So finding a storage frame is easy: it's just entry N; and finding the
        storage frame for a presentation frame is easy: it's just N +
        TemporalOffset[N].  But answering the question "When is the Nth storage
        frame presented?" is a bit more complicated. The table goes immediately from
        presentation to storage using the temporal offset, but given a storage frame
        there's no direct way to determine which presentation frame it is.  What we
        do to find the presentation frame for the Nth storage frame is iterate from
        say N-6  to N+60 and see which entry + temporal offset is N.  So for
        example, what is the presentation frame for storage frame 0?  Check entry 0
        -> that's 0 +1 =1.  The storage frame referenced is 1, not 0 so not the one
        we're looking for.  Check entry 1 -> that's 1 +1 =2.  The storage frame
        referenced is 2, not 0 so not the one we're looking for.  Check entry 2 ->
        that's 2 - 2 =0.  The storage frame referenced is 0 which is the one we're
        looking for.  So the presentation time for storage frame 0 is 2.
        */

        m_index[idx].file_pos = au.filePos;
        m_index[idx].frame_type = au.frame_type;
        m_index[idx].coding_order = au.coding_order;
        m_index[idx].display_order = au.display_order;

        // flags field
        // Bit 7: Random Access
        // Bit 6: Sequence Header
        // Bit 5: Forward prediction flag
        // Bit 4: Backward prediction flag
        // 00: I frame (no prediction)
        // 10: P frame (forward prediction from previous frame)
        // 01: B frame (backward prediction from future frame)
        // 11: B frame (forward and backward prediction)
        // Bits 0-3: reserved for use in SMPTE Essence
        // mapping specifications.

        switch (au.frame_type) {
            case I_TYPE:
                m_index[idx].flags = 0x80; // Random Access = 1
                break;
            case P_TYPE:
                m_index[idx].flags = 0x20;
                break;
            case B_TYPE:
                m_index[idx].flags = 0x30;
                break;
            default:
                m_callbacks.err_printf(m_callbacks.context, "Invalid frame type %d in index\n", au.frame_type);
                m_index.clear();
                return 1;
        }

        m_index[idx].temporal_offset = static_cast<int8_t>(au.display_order - au.coding_order);
        m_index[idx].key_frame_offset = 0;
    }

    auto less_index_entry = [&](const index_entry_t& a, const index_entry_t& b) { return a.coding_order < b.coding_order; };
    std::sort(m_index.begin(), m_index.end(), less_index_entry);

    int64_t last_ref_idx = -1;
    // compute frame_size values
    for (int64_t idx = 0; idx < m_total_frames; idx++) {
        // compute key_frame_offset value
        if (m_index[idx].frame_type == I_TYPE) {
            m_index[idx].key_frame_offset = 0;
            last_ref_idx = idx;
        }
        else {
            if (last_ref_idx >= 0) {
                m_index[idx].key_frame_offset = static_cast<int8_t>(last_ref_idx - idx);
            }
            else {
                m_index[idx].key_frame_offset = 0;
            }
        }

        if (idx >= m_total_frames - 1) {
            m_index[idx].frame_size = static_cast<int32_t>(m_file_size - m_index[idx].file_pos);
        }
        else {
            m_index[idx].frame_size = static_cast<int32_t>(m_index[idx + 1].file_pos - m_index[idx].file_pos);
        }
    }
    return 0;
}

std::shared_ptr<ExtIOAPIFeeder> ExtIOAPIFeeder::createFeeder(
    const callbacks_t& callbacks, mpegInInfo* mfi, const std::string& filename, ExternalIoAPIMode ext_io_api_mode)
{
    std::shared_ptr<ExtIOAPIFeeder> feeder;
    if (ext_io_api_mode == NewExtIoAPI) {
        feeder = std::make_shared<NewExtIOAPIFeeder>(callbacks, const_cast<char*>(filename.c_str()));
        mfi->appData = feeder.get();
        mfi->use_external_frame_io = 1;
        mfi->extFrameOpen = &(NewExtIOAPIFeeder::externalFrameOpen);
        mfi->extFrameRead = &(NewExtIOAPIFeeder::externalFrameRead);
        mfi->extFrameClose = &(NewExtIOAPIFeeder::externalFrameClose);
        mfi->extFrameGetFrameInfo = &(NewExtIOAPIFeeder::externalFrameGetFrameInfo);
    }
    else if (ext_io_api_mode == OldExtIoAPI) {
        feeder = std::make_shared<OldExtIOAPIFeeder>(callbacks, const_cast<char*>(filename.c_str()));
        mfi->appData = feeder.get();
        mfi->use_external_io = 1;
        mfi->extOpen = &(OldExtIOAPIFeeder::externalIoOpen<char>);
        mfi->extSeek = &(OldExtIOAPIFeeder::externalIoSeek);
        mfi->extRead = &(OldExtIOAPIFeeder::externalIoRead);
        mfi->extClose = &(OldExtIOAPIFeeder::externalIoClose);
#if defined(_WIN32)
        mfi->extOpenW = &(OldExtIOAPIFeeder::externalIoOpen<wchar_t>);
#endif
        mfi->mfimport_callbacks.extGetFrameInfo = &(OldExtIOAPIFeeder::externalFrameGetFrameInfo);
        mfi->extFrameGetFrameInfo = &(OldExtIOAPIFeeder::externalFrameGetFrameInfo);

        if (feeder->makeIndex()) {
            throw std::runtime_error("Cannot not create an index for the input file.");
        }

        auto old_feeder = std::dynamic_pointer_cast<OldExtIOAPIFeeder>(feeder);
        mfi->file_length = old_feeder->m_scale;
        mfi->external_io_stream_duration = old_feeder->m_total_frames;
    }
    else if (ext_io_api_mode != NoExtIoAPI) {
        throw std::runtime_error("Wrong ext IO API mode was given.");
    }
    mfi->additional_GOP_search = 2;

    return feeder;
}

NewExtIOAPIFeeder::NewExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name)
  : ExtIOAPIFeeder(callbacks, file_name)
{
    m_file_name = file_name;
    openFile(const_cast<char*>(file_name.c_str()));
}

long NewExtIOAPIFeeder::openFile(const char* filename)
{

    m_file_name = filename;
    if (ExtIOAPIFeeder::openFile(m_file_name.c_str(), m_id)) {
        return -1;
    }
    return 0;
}

int32_t NewExtIOAPIFeeder::externalFrameOpen(void* data, externalFrameStreamInfo* stream_info)
{
    auto instance = reinterpret_cast<NewExtIOAPIFeeder*>(data);
    if (instance->makeIndex()) {
        return 1;
    }

    stream_info->frameCount = instance->m_total_frames;
    stream_info->streamType = instance->m_stream_type;

    return 0;
}

void NewExtIOAPIFeeder::externalFrameClose(void* data)
{
    auto instance = reinterpret_cast<NewExtIOAPIFeeder*>(data);
    instance->m_index.clear();
}

uint8_t* NewExtIOAPIFeeder::externalFrameRead(void* data, int64_t storage_frame, int32_t& frame_size)
{
    auto instance = reinterpret_cast<NewExtIOAPIFeeder*>(data);
    if (storage_frame < 0 || storage_frame >= instance->m_total_frames) {
        return nullptr;
    }

    index_entry_t& pIdxEntry = instance->m_index[storage_frame];
    instance->m_frame_buffer.resize(pIdxEntry.frame_size);
    instance->m_files[m_id].seekg(pIdxEntry.file_pos);
    if (instance->m_files[m_id].tellg() != pIdxEntry.file_pos) {
        instance->m_callbacks.err_printf(instance->m_callbacks.context, "Unable to seek to frame %lld in input file\n", storage_frame);
        return nullptr;
    }

    instance->m_files[m_id].read(reinterpret_cast<char*>(instance->m_frame_buffer.data()), pIdxEntry.frame_size);
    if (instance->m_files[m_id].gcount() != pIdxEntry.frame_size) {
        instance->m_callbacks.err_printf(instance->m_callbacks.context, "Unable to read frame %lld from input file\n", storage_frame);
        return nullptr;
    }

    frame_size = pIdxEntry.frame_size;
    return instance->m_frame_buffer.data();
}

OldExtIOAPIFeeder::OldExtIOAPIFeeder(const callbacks_t& callbacks, const std::string& file_name, int64_t scale)
  : ExtIOAPIFeeder(callbacks, file_name)
  , m_scale{scale}
{
}

long OldExtIOAPIFeeder::getNextId()
{
    auto found = std::find_if(m_files.begin() + 1, m_files.end(), [](std::ifstream& file) { return !file.is_open(); });
    return found == m_files.end() ? -1 : static_cast<long>(std::distance(m_files.begin(), found));
}

int32_t OldExtIOAPIFeeder::externalIoSeek(void* app_data, long id, int64_t position)
{
    auto instance = reinterpret_cast<OldExtIOAPIFeeder*>(app_data);
    int64_t frame_idx = position / (instance->m_scale / instance->m_total_frames);

    instance->m_files[id].seekg(instance->m_index[frame_idx].file_pos);
    if (instance->m_files[id].tellg() != instance->m_index[frame_idx].file_pos) {
        return 1;
    }
    return 0;
}

int32_t OldExtIOAPIFeeder::externalIoRead(void* app_data, long id, uint8_t* buffer, int32_t buffer_size)
{
    auto instance = reinterpret_cast<OldExtIOAPIFeeder*>(app_data);
    instance->m_files[id].read(reinterpret_cast<char*>(buffer), buffer_size);

    // Read less than requested
    if (instance->m_files[id].fail() && instance->m_files[id].eof()) {
        auto reset_fail_state = instance->m_files[id].rdstate() & (~std::ifstream::failbit);
        instance->m_files[id].clear(reset_fail_state);
    }

    return static_cast<int32_t>(instance->m_files[id].gcount());
}

void OldExtIOAPIFeeder::externalIoClose(void* app_data, long id)
{
    auto instance = reinterpret_cast<OldExtIOAPIFeeder*>(app_data);
    instance->closeFile(id);
}

void OldExtIOAPIFeeder::closeFile(long id)
{
    std::lock_guard<std::mutex> lock(m_mtx_files);
    ExtIOAPIFeeder::closeFile(id);
}
