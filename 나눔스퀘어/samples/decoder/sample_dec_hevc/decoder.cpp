/******************************************************************************
 Created: 2013/02/22
 File name: sample_dec_hevc.cpp
 Purpose: command-line sample for HEVC decoder

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.
 Unauthorized reproduction or distribution of any portion is prohibited by law.
 *******************************************************************************/

#include <utility>
#include "decoder.h"

bool Decoder::initialize(BufstreamCreator bufstream_creator)
{
    memset(&m_frame, 0, sizeof(m_frame));
    // initialize stream parameters
    stream_params_t stream_params = { 0 };
    stream_params.nodeset = helper.nodeset;

    initCallbacks();

    // Create the decoder
    m_decoder = bufstream_creator(&m_callbacks, &m_decoder_callback, &stream_params);
    if (!m_decoder) {
        fprintf(helper.log, "\nError: Can`t create HEVC Video Decoder");
        return false;
    }

    initializeDictionary();

    if (!setDecoderParam(0, PARSE_INIT))
        return false;
    // Make frames display in display order and not decode order
    if (!setDecoderParam(helper.decoder_options, PARSE_OPTIONS))
        return false;
    if (!setDecoderParam(0, PARSE_SEQ_HDR))
        return false;
    if (!setDecoderParam(helper.max_temporal_layer, SET_MAX_TEMPORAL_LAYER))
        return false;
    if (!setDecoderParam(helper.preview_mode, SET_PREVIEW_MODE))
        return false;

    if (helper.parse_frames) {
        if (!setDecoderParam(helper.skip_mode, PARSE_FRAMES))
            return false;
    }
    else {
        if (!setDecoderParam(SKIP_NONE, PARSE_FRAMES))
            return false;
        if (!setDecoderParam(0, PARSE_SEQ_HDR))
            return false;
    }

    if (helper.use_callbacks)
        if (!setDecoderParam(helper.async_intput_output, SET_ASYNC_INPUT_OUTPUT_MODE))
            return false;

    if (helper.decoding_toolset > HEVCVD_DECODING_TOOLSET_CPU) {
        if (!setDecoderParam(helper.decoding_toolset, HWACC_SET_ACC_MODE))
            return false;

        //! [Choose video adapter]
        // Enumerate installed video adapters
        hevc_hardware_adapters_t adapters = { 0 };
        if (BS_OK == m_decoder->auxinfo(m_decoder, 0, HWACC_GET_HW_CAPS, &adapters, sizeof(hevc_hardware_adapters_t)) && adapters.count) {
            // Select adapter if required
            if (helper.hw_adapter != ITEM_NOT_INIT && !setDecoderParam(helper.hw_adapter, SET_HW_ADAPTER))
                return false;
        }
        //! [Choose video adapter]
    }

    if (!setDecoderParam(helper.threads, SET_CPU_NUM))
        return false;
    if (!setDecoderParam(helper.smp_mode, SET_SMP_MODE))
        return false;
    if (!setDecoderParam(helper.deinterlacing_mode, HEVCVD_SET_DEINTERLACING_MODE))
        return false;

    if (helper.transfer_characteristics != TRANSFER_CHARACTERISTICS_AUTO || helper.cc_pix_range != RANGE_PRESERVE) {
        colorimetry_t colorimetry_input;
        colorimetry_t colorimetry_output;
        set_colorimetry_from_color_description(&colorimetry_input, COLOR_DESCRIPTION_AUTO);
        set_colorimetry_from_color_description(&colorimetry_output, COLOR_DESCRIPTION_AUTO);

        switch (helper.cc_pix_range) {
            case RANGE_PRESERVE:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_AUTO;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_AUTO;
                break;
            case RANGE_SRC_TO_SHORT:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_AUTO;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_SHORT;
                break;
            case RANGE_SRC_TO_FULL:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_AUTO;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_FULL;
                break;
            case RANGE_FULL_TO_FULL:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_FULL;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_FULL;
                break;
            case RANGE_FULL_TO_SHORT:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_FULL;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_SHORT;
                break;
            case RANGE_SHORT_TO_SHORT:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_SHORT;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_SHORT;
                break;
            case RANGE_SHORT_TO_FULL:
                colorimetry_input.signal_range = PIC_SIGNAL_RANGE_SHORT;
                colorimetry_output.signal_range = PIC_SIGNAL_RANGE_FULL;
                break;
        }
        if (helper.transfer_characteristics != TRANSFER_CHARACTERISTICS_AUTO) {
            colorimetry_output.transfer_characteristics = helper.transfer_characteristics;
        }

        if (BS_OK != m_decoder->auxinfo(m_decoder, 0, OVERRIDE_NATIVE_COLORIMETRY, &colorimetry_input, sizeof(colorimetry_input)))
            return false;
        if (BS_OK != m_decoder->auxinfo(m_decoder, 0, SET_OUTPUT_COLORIMETRY, &colorimetry_output, sizeof(colorimetry_input)))
            return false;
    }

    return true;
}

int Decoder::loop()
{
    uint8_t buffer[READ_BUFFER_SIZE];
    // While we have file data, feed it to the decoder
    while (uint32_t bytes_available = (uint32_t)fread(buffer, 1, READ_BUFFER_SIZE, helper.input_file)) {
        uint32_t consumed = 0;
        do {
            // Feed data to the decoder
            consumed += m_decoder->copybytes(m_decoder, buffer + consumed, bytes_available - consumed);

            // Check the results of the copybytes() call
            uint32_t state = m_decoder->auxinfo(m_decoder, 0, CLEAN_PARSE_STATE, NULL, 0);

            if (!handleErrors(state))
                return 1;

            if (!helper.use_callbacks && (state & (PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG))) {
                if (!processFrame(state)) {
                    fprintf(helper.log, "Error: Failed to get picture\n");
                    return 1;
                }
            }

        } while (bytes_available - consumed);
    }

    return 0;
}

int Decoder::flush()
{
    // Once all data is read from the file, decode any remaining data left in the decoder
    int32_t bytes_left = 0;
    do {
        bytes_left = m_decoder->copybytes(m_decoder, NULL, 0); // Decode

        // Check the results of the copybytes() call
        uint32_t state = m_decoder->auxinfo(m_decoder, 0, CLEAN_PARSE_STATE, NULL, 0);

        if (!handleErrors(state))
            return 1;

        if (!helper.use_callbacks && (state & (PIC_DECODED_FLAG | PIC_VALID_FLAG | PIC_FULL_FLAG))) {
            if (!processFrame(state)) {
                fprintf(helper.log, "Error: Failed to get picture\n");
                return 1;
            }
        }
    } while (bytes_left);

    return 0;
}

void Decoder::showAdapters()
{
    hevc_hardware_adapters_t adapters;
    const std::map<std::string, hevc_decoding_toolset_t>& toolsets = enumerateDecodingToolsets();
    for (std::map<std::string, hevc_decoding_toolset_t>::const_iterator i = toolsets.begin(); i != toolsets.end(); ++i) {

        if (!setDecoderParam(i->second, HWACC_SET_ACC_MODE) ||
            BS_OK != m_decoder->auxinfo(m_decoder, 0, HWACC_GET_HW_CAPS, &adapters, sizeof(hevc_hardware_adapters_t)))
            continue;

        printf("Devices %s (option: -hw_acc %s): \n", i->first.c_str(), i->first.c_str());

        for (uint8_t i = 0; i < adapters.count; i++)
            printf("\t%2d# Video adapter: %S\n", i, adapters.set[i].description);
    }
}

void Decoder::initializeFrame()
{
    if (helper.convert_frame) {
        uint32_t width, height;

        if (!helper.fourcc) {
            //! [Get native colorspace]
            uint32_t fourcc;
            // getting current frame native fourcc
            if (BS_OK == m_decoder->auxinfo(m_decoder, 0, GET_NATIVE_FOURCC, &fourcc, sizeof(uint32_t)))
                m_frame.four_cc = fourcc;
            //! [Get native colorspace]
            else
                m_frame.four_cc = 0;
        }
        else
            m_frame.four_cc = helper.fourcc;

        if (helper.config_width > 0 && helper.config_height > 0) {
            width = helper.config_width;
            height = helper.config_height;
        }
        else {
            getFrameSize(width, height);
        }

        get_frame_colorspace_info(&m_cs_info, width, height, m_frame.four_cc, 0);
        // Check if the size has changed and reallocate buffer
        if (m_cs_info.frame_size != m_buffer_size) {
            m_buffer_size = m_cs_info.frame_size;
            if (m_buffer)
                free(m_buffer);
            m_buffer = reinterpret_cast<uint8_t*>(malloc(m_buffer_size + 15));
        }

        uint8_t* const base_ptr = reinterpret_cast<uint8_t*>(uintptr_t(m_buffer + 15) & ~15);
        fill_frame_from_colorspace_info(&m_cs_info, base_ptr, &m_frame);
    }
    else {
        m_frame.four_cc = 0;
        getFrameSize(m_frame.width, m_frame.height);
    }
}

bool Decoder::getFrame()
{
    initializeFrame();
    if (helper.frame_required) {
        if (BS_OK != m_decoder->auxinfo(m_decoder, 0, GET_PIC, &m_frame, sizeof(frame_tt)))
            return false;

        if (!helper.convert_frame)
            get_frame_colorspace_info(&m_cs_info, m_frame.width, m_frame.height, m_frame.four_cc, 0);
    }

    if (!helper.width && !helper.height) {
        helper.width = m_frame.width;
        helper.height = m_frame.height;
    }

    return true;
}

bool Decoder::getHwFrame()
{
    hevc_picture_s* pic;
    return BS_OK == m_decoder->auxinfo(m_decoder, 0, HWACC_GET_PIC, &pic, sizeof(*pic));
}

void Decoder::getFrameSize(uint32_t& width, uint32_t& height)
{
    //! [Get pointer to Get GET_SEQ_PARAMSPEX]
    // Get the sequence header for width and height
    SEQ_ParamsEx* sps;
    if (BS_OK == m_decoder->auxinfo(m_decoder, 0, GET_SEQ_PARAMSPEX, &sps, sizeof(SEQ_ParamsEx))) {
        width = sps->horizontal_size;
        height = sps->vertical_size;
    }
    //! [Get pointer to Get GET_SEQ_PARAMSPEX]
}

bool Decoder::handleErrors(const uint32_t state)
{
    if (state & INTERNAL_ERROR) {
        fprintf(helper.log, "Error: Out of memory\n");
        return false;
    }

    return true;
}

bool Decoder::processFrame(const uint32_t state)
{
    bool surface_available = false;
    // If there is an output video frame in the decoder, put it into the allocated frame_tt
    if (state & PIC_DECODED_FLAG) {

        if (helper.raw_hardware_output) {
            // for example how get hw buffer
            surface_available = getHwFrame();
        }

        if (!getFrame())
            return false;

        helper.pictures_decoded++;

        // Update the MD5
        if (helper.md5)
            helper.updateMD5(m_frame.plane, m_cs_info.stride, m_cs_info.plane_height, m_frame.stride, m_cs_info.planes);

        // Write out the yuv frame
        if (helper.output_file)
            helper.outputFrame(m_frame.plane, m_cs_info.stride, m_cs_info.plane_height, m_frame.stride, m_cs_info.planes);

        // Check md5 frame
        if (helper.md5_frame) {
            uint8_t* md5_original[3];
            hevc_sei_messages_t* sei_messages;
            if (BS_OK != m_decoder->auxinfo(m_decoder, 0, GET_SEI, &sei_messages, sizeof(hevc_sei_messages_t)) ||
                !getFrameMd5(sei_messages, m_cs_info.planes, md5_original)) {
                memset(md5_original, 0, sizeof(md5_original));
            }

            helper.checkFrameMd5(m_frame.plane, m_cs_info.stride, m_cs_info.plane_height, m_frame.stride, m_cs_info.planes, md5_original);
        }
    }
    else if ((state & PIC_FULL_FLAG) != 0) {
        helper.pictures_decoded++;
        getFrameSize(m_frame.width, m_frame.height);

        if (!helper.width && !helper.height) {
            helper.width = m_frame.width;
            helper.height = m_frame.height;
        }
    }

    // Update progress
    if (helper.progress == 1) {
        helper.updateProgress();
    }
    else if (helper.verbose) {
        PIC_Params* picPara = 0;
        if (BS_OK == m_decoder->auxinfo(m_decoder, 0, GET_PIC_PARAMSP, &picPara, sizeof(*picPara))) {
            const bool pic_error = PIC_ERROR_FLAG == (state & PIC_ERROR_FLAG);
            const bool pic_skipped = (state & PIC_DECODED_FLAG) == 0;
            helper.updateVerbose(picPara->temporal_reference, picPara->picture_type, m_frame.width, m_frame.height, pic_error, pic_skipped, surface_available);

            if (helper.print_sei_types) {
                hevc_sei_messages_t* sei;
                if (BS_OK == m_decoder->auxinfo(m_decoder, 0, GET_SEI, &sei, sizeof(hevc_sei_messages_t)))
                    helper.printSeiInfo(sei);
            }
        }
    }

    return true;
}

bool Decoder::getFrameMd5(hevc_sei_messages_t* sei_messages, uint8_t plane_count, uint8_t* md5[3])
{
    for (uint32_t i = 0; i < sei_messages->num_messages; i++) {
        hevc_sei_payload_s* sei_payload = sei_messages->sei_payload + i;

        if (SEI_TYPE_DECODED_PICTURE_HASH == sei_payload->payload_type) {
            uint8_t* ptr = sei_payload->payload;
            const uint8_t hash_type = *ptr;
            if (0 == hash_type) { // md5
                ptr++;
                for (uint8_t i = 0; i < plane_count; i++) {
                    md5[i] = ptr;
                    ptr += 16;
                }
                return true;
            }
        }
    }

    return false;
}

bool Decoder::setDecoderParam(int value, int command)
{
    const int result(m_decoder->auxinfo(m_decoder, value, command, NULL, 0));
    if (result != BS_OK) {
        if (m_dec_param_to_string.find(command) != m_dec_param_to_string.end())
            fprintf(helper.log, "\nError: decoder failed to initialize parameter: %s, code returned: %d\n\n", m_dec_param_to_string[command], result);
        else
            fprintf(helper.log, "\nError: decoder failed to initialize parameter: %d, code returned: %d\n\n", command, result);

        return false;
    }
    return true;
}

void Decoder::initializeDictionary()
{
    m_dec_param_to_string.insert(std::make_pair(PARSE_INIT, "PARSE_INIT"));
    m_dec_param_to_string.insert(std::make_pair(PARSE_OPTIONS, "PARSE_OPTIONS"));
    m_dec_param_to_string.insert(std::make_pair(PARSE_SEQ_HDR, "PARSE_SEQ_HDR"));
    m_dec_param_to_string.insert(std::make_pair(SET_SMP_MODE, "SET_SMP_MODE"));
    m_dec_param_to_string.insert(std::make_pair(SET_CPU_NUM, "SET_CPU_NUM"));
    m_dec_param_to_string.insert(std::make_pair(SET_MAX_TEMPORAL_LAYER, "SET_MAX_TEMPORAL_LAYER"));
    m_dec_param_to_string.insert(std::make_pair(SET_PREVIEW_MODE, "SET_PREVIEW_MODE"));
    m_dec_param_to_string.insert(std::make_pair(PARSE_FRAMES, "PARSE_FRAMES"));
    m_dec_param_to_string.insert(std::make_pair(SET_HW_ADAPTER, "SET_HW_ADAPTER"));
    m_dec_param_to_string.insert(std::make_pair(HWACC_GET_HW_CAPS, "HWACC_GET_HW_CAPS"));
    m_dec_param_to_string.insert(std::make_pair(HWACC_SET_ACC_MODE, "HWACC_SET_ACC_MODE"));
    m_dec_param_to_string.insert(std::make_pair(SET_ASYNC_INPUT_OUTPUT_MODE, "SET_ASYNC_INPUT_OUTPUT_MODE"));
}

////////////
// Callbacks
////////////

void Decoder::naluCallback(context_t context, const hevc_picture_t* hevc_pic, const hevc_nalu_t* nalu)
{
    Decoder* dec = (Decoder*)context.p;

    if (!dec)
        return;

    dec->helper.latest_nalu_type = (nalu_types_e)nalu->nal_unit_type;
}

// Called when a picture is selected from the DPB for display and before its internally queued for display
// Pictures are received in display order.
void Decoder::pictureOutputCallback(context_t context, const hevc_picture_t* hevc_pic)
{
    Decoder* dec = (Decoder*)context.p;

    if (!dec)
        return;

    if (dec->helper.use_callbacks) {
        if (hevc_pic && hevc_pic->sps && !hevc_pic->skipped &&
            ((dec->helper.deinterlacing_mode == HEVCVD_DEINTERLACING_NONE) || hevc_pic->full_frame_available)) {
            if (dec->helper.convert_frame) {
                dec->processFrame(PIC_DECODED_FLAG | PIC_FULL_FLAG | (hevc_pic->error ? PIC_ERROR_FLAG : PIC_VALID_FLAG));
            }
            else {
                // Update the number of pics decoded
                dec->helper.pictures_decoded++;

                // Store the frame width and height
                if (!dec->helper.width && !dec->helper.height) {
                    dec->helper.width = hevc_pic->width;
                    dec->helper.height = hevc_pic->height;
                }

                if (dec->helper.output_file || dec->helper.md5 || dec->helper.md5_frame) {

                    const bool pixel_valid = BS_OK == dec->m_decoder->auxinfo(dec->m_decoder, 0, GET_PIC, NULL, sizeof(frame_tt));
                    if (pixel_valid) {
                        get_frame_colorspace_info(&dec->m_cs_info, hevc_pic->width, hevc_pic->height, hevc_pic->fourcc, 0);
                        if (dec->helper.output_file)
                            dec->helper.outputFrame(hevc_pic->pixel, dec->m_cs_info.stride, dec->m_cs_info.plane_height,
                                reinterpret_cast<const int32_t*>(hevc_pic->stride), dec->m_cs_info.planes);

                        if (dec->helper.md5)
                            dec->helper.updateMD5(hevc_pic->pixel, dec->m_cs_info.stride, dec->m_cs_info.plane_height,
                                reinterpret_cast<const int32_t*>(hevc_pic->stride), dec->m_cs_info.planes);

                        if (dec->helper.md5_frame) {
                            uint8_t* md5_original[3];
                            hevc_sei_messages_t* sei_messages;
                            if (BS_OK != dec->m_decoder->auxinfo(dec->m_decoder, 0, GET_SEI, &sei_messages, sizeof(hevc_sei_messages_t)) ||
                                !dec->getFrameMd5(sei_messages, dec->m_cs_info.planes, md5_original))
                                memset(md5_original, 0, sizeof(md5_original));

                            dec->helper.checkFrameMd5(hevc_pic->pixel, dec->m_cs_info.stride, dec->m_cs_info.plane_height,
                                reinterpret_cast<const int32_t*>(hevc_pic->stride), dec->m_cs_info.planes, md5_original);
                        }
                    }
                    else
                        dec->helper.printError("Error of obtaining pixels");
                }

                // Output some frame info
                if (dec->helper.progress) {
                    dec->helper.updateProgress();
                }
                else if (dec->helper.verbose) {
                    dec->helper.updateVerbose(hevc_pic->poc, hevc_pic->slice_hdr[0]->slice_type, hevc_pic->width, hevc_pic->height, hevc_pic->error != 0,
                        hevc_pic->skipped != 0, hevc_pic->surface != 0);

                    if (dec->helper.print_sei_types) {
                        hevc_sei_messages_t* sei;
                        if (BS_OK == dec->m_decoder->auxinfo(dec->m_decoder, 0, GET_SEI, &sei, sizeof(hevc_sei_messages_t)))
                            dec->helper.printSeiInfo(sei);
                    }
                }
            }
        }
    }
}

int32_t Decoder::getLicenseData(context_t context, void* pData)
{
    Decoder* dec = (Decoder*)context.p;
    FILE* license_file = nullptr;
    if (!dec || !dec->helper.license_file_name || !(license_file = fopen(dec->helper.license_file_name, "rb")))
        return 0;

    int32_t ret = -1;
    if (pData) {
        if (fread(pData, sizeof(uint8_t), dec->helper.license_file_size, license_file))
            ret = 0;
    }
    else {
        size_t file_size = 0;
        if (!fseek(license_file, 0, SEEK_END) && (file_size = ftell(license_file)) && file_size < INT32_MAX) {
            ret = static_cast<int32_t>(file_size);
            dec->helper.license_file_size = ret;
        }
    }
    fclose(license_file);
    return ret;
}

//! [Callback functions dispatcher]
void Decoder::initCallbacks()
{
    ::init_callbacks(m_callbacks);
    // Fill m_decoder_callback structure with zeroes
    memset(&m_decoder_callback, 0, sizeof(callbacks_decoder_hevc_t));
    m_callbacks.context = context_t{ this };
    m_callbacks.get_license_data = Decoder::getLicenseData;
    m_decoder_callback.pic_output_callback = Decoder::pictureOutputCallback;
    m_decoder_callback.nalu_callback = Decoder::naluCallback;
}
//! [Callback functions dispatcher]
