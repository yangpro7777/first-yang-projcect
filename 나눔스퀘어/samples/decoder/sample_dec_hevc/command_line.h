#ifndef UUID_EDA01A99_0A93_4C08_9DDF_9DB4D295D5E3
#define UUID_EDA01A99_0A93_4C08_9DDF_9DB4D295D5E3

#include <vector>
#include <limits>
#include <string.h>
#include "misc.h"
#include "dec_hevc.h"
#include "sample_common_args.h"

/* Base command line parameters class */
class CommandLine
{
public:
    CommandLine()
      : in_file_name(NULL),
        out_file_name(NULL),
        config_file_name(NULL),
        license_file_name(NULL),
        smp_mode(HEVCVD_SMP_AUTO),
        threads(ITEM_NOT_INIT),
        fourcc(ITEM_NOT_INIT),
        width(ITEM_NOT_INIT),
        height(ITEM_NOT_INIT),
        quiet(ITEM_NOT_INIT),
        progress(ITEM_NOT_INIT),
        md5(ITEM_NOT_INIT),
        md5l(ITEM_NOT_INIT),
        md5_frame(ITEM_NOT_INIT),
        nodeset((std::numeric_limits<uint64_t>::max)()),
        preview_mode(HEVCVD_PREVIEW_OFF),
        use_callbacks(ITEM_NOT_INIT),
        async_input_output(ITEM_NOT_INIT),
        hw_enumerate(ITEM_NOT_INIT),
        hw_adapter(ITEM_NOT_INIT),
        hw_acc_name(NULL),
        cc_pix_range(RANGE_FULL_TO_FULL),
        transfer_characteristics(ITEM_NOT_INIT)
    {
        const std::map<std::string, hevc_decoding_toolset_t>& toolsets = enumerateDecodingToolsets();

        m_params.push_back(ArgItem(IDS_INPUT_FILE, 0, &in_file_name));
        m_params.push_back(ArgItem(IDS_OUTPUT_FILE, 0, &out_file_name));
        m_params.push_back(ArgItem(IDS_LIC_FILE, 0, &license_file_name));
        m_params.push_back(ArgItem(IDN_V_FOURCC, 0, &fourcc));
        m_params.push_back(ArgItem(IDI_V_WIDTH, 0, &width));
        m_params.push_back(ArgItem(IDI_V_HEIGHT, 0, &height));
        m_params.push_back(ArgItem(IDN_QUIET, 0, &quiet));
        m_params.push_back(ArgItem(IDN_V_PROGRESS, 0, &progress));
        m_params.push_back(ArgItem(IDI_NUM_THREADS, 0, &threads));
        m_params.push_back(ArgItem(IDN_V_MD5, 0, &md5));
        m_params.push_back(ArgItem(IDN_V_MD5L, 0, &md5l));
        m_params.push_back(ArgItem(IDN_V_SMP, 0, &smp_mode));
        m_params.push_back(ArgItem(IDS_CONFIG_FILE, 0, &config_file_name));
        m_params.push_back(ArgItem(IDN_V_MD5_FRAME, 0, &md5_frame));
        m_params.push_back(ArgItem(IDN_LOCAL_NODESET, 0, &nodeset));
        m_params.push_back(ArgItem(IDN_LOCAL_PREVIEW_MODE, 0, &preview_mode));
        m_params.push_back(ArgItem(IDN_LOCAL_CALLBACKS, 0, &use_callbacks));
        m_params.push_back(ArgItem(IDN_LOCAL_ASYNC_INPUT_OUTPUT, 0, &async_input_output));
        if (!toolsets.empty()) {
            m_params.push_back(ArgItem(IDI_HWACC_MODE, 0, &hw_acc_name));
            m_params.push_back(ArgItem(IDN_LOCAL_HW_ENUMERATE, 0, &hw_enumerate));
            m_params.push_back(ArgItem(IDN_LOCAL_HW_ADAPTER, 0, &hw_adapter));
        }
        m_params.push_back(ArgItem(IDN_LOCAL_PIXEL_RANGE, 0, &cc_pix_range));
        m_params.push_back(ArgItem(IDN_LOCAL_DEINTERLACING_MODE, 0, &deinterlacing_mode));
        m_params.push_back(ArgItem(IDN_LOCAL_TRANSFER_CHARACTERISTICS, 0, &transfer_characteristics));

        m_custom_params.push_back(ArgItemDescription(IDN_V_FOURCC, "<fourcc>", "cs", ItemTypeInt, 0,
            "output frames using specified colorspace, default is native colorspace of stream (for example I420 for 8-bit 4:2:0 stream (when SW decoding is "
            "used))"));
        m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_NODESET, "ns", "nodeset", ItemTypeInt64, -1,
            "bitmask of cpu nodes available for the decoder, default value all, 0 - use only one node which is used by sample"));
        m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_PREVIEW_MODE, "pm", "previewmode", ItemTypeInt, ITEM_NOT_INIT,
            "preview mode. By default is 0. The higher the value the faster is the decoding and the lower is the quality of the output"));
        m_custom_params.push_back(
            ArgItemDescription(IDN_LOCAL_CALLBACKS, "callbacks", ItemTypeNoArg, 1, "force sample to use callback api instead of polling api"));
        m_custom_params.push_back(
            ArgItemDescription(IDN_LOCAL_ASYNC_INPUT_OUTPUT, "async_input_output", "async", ItemTypeNoArg, 1, "use asynchronous input output flag"));
        m_custom_params.push_back(
            ArgItemDescription(IDN_LOCAL_TRANSFER_CHARACTERISTICS, "tc", ItemTypeInt, ITEM_NOT_INIT, "Target transfer characteristics for output stream"));

        if (!toolsets.empty()) {
            std::string hw_modes = "hardware acceleration: ";
            for (std::map<std::string, hevc_decoding_toolset_t>::const_iterator i = toolsets.begin();;) {
                hw_modes += i->first;
                ++i;
                if (i != toolsets.end())
                    hw_modes += ", ";
                else
                    break;
            }

            m_custom_params.push_back(ArgItemDescription(IDI_HWACC_MODE, "hw_acc", "", ItemTypeString, ITEM_NOT_INIT, hw_modes.c_str()));
            m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_HW_ENUMERATE, "hw_enumerate", "", ItemTypeNoArg, 1,
                "enumerate the adapters support hardware acceleration, requires to specify hw_acc"));
            m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_HW_ADAPTER, "hw_adapter", "", ItemTypeInt, ITEM_NOT_INIT, "specifies adapter number"));
        }
        m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_PIXEL_RANGE, "pix_range", "", ItemTypeInt, ITEM_NOT_INIT, "specifies dynamic range conversion"));
        m_custom_params.push_back(ArgItemDescription(IDN_LOCAL_DEINTERLACING_MODE, "deinterlacing_mode", "dm", ItemTypeInt, ITEM_NOT_INIT,
            "select deinterlacing mode for interlaced video, default value is 0, possible values: 0 - output by fields, 1 - interfield interpolation, 2 - top "
            "field stretching, 3 - bottom field stretching, 4 - weave"));
    }

    // initialize with command line args
    bool initialize(int argc, char* argv[]);

    char* in_file_name;
    char* out_file_name;
    char* config_file_name;
    char* license_file_name;

    int32_t smp_mode;
    int32_t threads;
    int32_t fourcc;
    int32_t width;
    int32_t height;
    int32_t quiet;
    int32_t progress;
    int32_t md5;
    int32_t md5l;
    int32_t md5_frame;
    uint64_t nodeset;
    int32_t preview_mode;
    int32_t use_callbacks;
    int32_t async_input_output;
    int32_t hw_enumerate;
    int32_t hw_adapter;
    char* hw_acc_name;
    int32_t cc_pix_range;
    int32_t deinterlacing_mode;
    int32_t transfer_characteristics;

protected:
    std::vector<arg_item_t> m_params;
    std::vector<arg_item_desc_t> m_custom_params;

private:
    // Print command line usage
    void printUsage(const char* appname);
};

#endif
