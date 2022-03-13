/*****************************************************************************
 Created: 2014/12/05
 File name: dec_hevc_decoder.h
 Purpose: example code for HEVC decoder

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized reproduction or distribution of any portion is prohibited by law.
******************************************************************************/

#ifndef UUID_DEE915B1_9ACB_4356_BFDF_94F40637BAD0
#define UUID_DEE915B1_9ACB_4356_BFDF_94F40637BAD0

#include <map>
#include "helper.h"

/* How many HEVC file bytes to read at a time */
#define READ_BUFFER_SIZE (64 * 1024)

class Decoder
{
public:
    typedef decltype(&createDecoderHEVC) BufstreamCreator;

    Decoder(Helper& helper) : helper(helper), m_buffer(NULL), m_buffer_size(0) {}

    ~Decoder()
    {
        if (m_decoder)
            close_bufstream(m_decoder, 0);

        if (m_buffer)
            free(m_buffer);
    }

    bool initialize(BufstreamCreator bufstream_creator);
    int loop();
    int flush();
    void showAdapters();

    Helper& helper;

protected:
    bool setDecoderParam(int value, int command);

    std::map<int, const char* const> m_dec_param_to_string;

    virtual void initCallbacks();

    virtual bool handleErrors(const uint32_t state);
    virtual bool processFrame(const uint32_t state);

    bufstream_tt* m_decoder;
    callbacks_t m_callbacks;
    callbacks_decoder_hevc_t m_decoder_callback;

private:
    // Called as each NALU is parsed from the stream
    static void naluCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_nalu_t* nalu);
    // Called as each sps is parsed from the stream
    static void spsCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_seq_par_set_t* sps);
    // Called as each pps is parsed from the stream
    static void ppsCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_pic_par_set_t* pps);
    // Called as each sei is parsed from the stream
    static void seiCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_sei_payload_t* sei, const uint32_t count);
    // Called as each slice header is parsed from the stream
    static void sliceHeaderCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_slice_hdr_t* slice_hdr);
    // Called after a picture is parsed from the stream but before its to be scheduled for decode
    static void pictureParsedCallback(context_t context, const hevc_picture_t* hevc_pic);
    // Called when a picture is decoded. Pictures are received in decode order.
    static void pictureDecodedCallback(context_t context, const hevc_picture_t* hevc_pic);
    // Called when a picture is selected from the DPB for display and before its internally queued for display
    // Pictures are received in display order.
    static void pictureOutputCallback(context_t context, const hevc_picture_t* hevc_pic);
    // Called when a picture is skipped. Pictures can be skipped because a flag like SKIP_B is active.
    static void pictureSkippedCallback(context_t context, const hevc_picture_t* hevc_pic);
    // Called when a stream or decode error occurs
    static void errorCallback(context_t context, const hevc_runtime_error_t code);
    // Requests license data. See callbacks_t::get_license_data description for more infirmation
    static int32_t getLicenseData(context_t context, void* data);

    bool getFrame();
    bool getHwFrame();
    void initializeFrame();
    void getFrameSize(uint32_t& width, uint32_t& height);

    bool getFrameMd5(hevc_sei_messages_t* sei_messages, uint8_t plane_count, uint8_t* plane_md5[3]);

    void initializeDictionary();

    uint8_t* m_buffer;
    frame_tt m_frame;
    frame_colorspace_info_tt m_cs_info;
    uint32_t m_buffer_size;
};

#endif
