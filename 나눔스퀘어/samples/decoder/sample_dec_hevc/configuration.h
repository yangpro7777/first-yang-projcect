#ifndef UUID_5716A889_D993_44E6_8C8D_AAAD4DB7B189
#define UUID_5716A889_D993_44E6_8C8D_AAAD4DB7B189

#include <vector>
#include <limits>

#include "dec_hevc.h"
#include "sample_common_args.h"
#include "misc.h"

/* Base configuration file support class */
class Configuration
{
public:
    Configuration()
      : smp_mode(HEVCVD_SMP_AUTO),
        hw_adapter(ITEM_NOT_INIT),
        preview_mode(HEVCVD_PREVIEW_OFF),
        nodeset((std::numeric_limits<uint64_t>::max)()),
        threads(0),
        width(0),
        height(0),
        skip_mode(SKIP_NONE),
        parse_frames(1),
        quiet(0),
        progress(0),
        md5(0),
        md5_frame(0),
        max_temporal_layer(6),
        print_sei_types(0),
        cc_pix_range(RANGE_FULL_TO_FULL),
        use_callbacks(0),
        async_intput_output(0)
    {
        hw_acc_name[0] = 0;
        fourcc[0] = 0;
        inputfile[0] = 0;
        outputfile[0] = 0;

        // Used to map command line options to our playback configuration
        // m_param_map.push_back(ParameterMap(name, place, type, default value, limit enabled, min, max));
        m_param_map.push_back(ParameterMap("FourCC", &fourcc, ItemTypeString, 0, 0, 0.0, 0.0));
        m_param_map.push_back(ParameterMap("Width", &width, ItemTypeInt, 0, 1, 0, 16384));
        m_param_map.push_back(ParameterMap("Height", &height, ItemTypeInt, 0, 1, 0, 16384));
        m_param_map.push_back(ParameterMap("Skip", &skip_mode, ItemTypeInt, SKIP_NONE, 1, SKIP_NONE, SKIP_NONREF));
        m_param_map.push_back(ParameterMap("ParseFrames", &parse_frames, ItemTypeInt, 1, 1, 0, 1));
        m_param_map.push_back(ParameterMap("SMP", &smp_mode, ItemTypeInt, HEVCVD_SMP_AUTO, 1, HEVCVD_SMP_AUTO, HEVCVD_SMP_CONCURRENT));
        m_param_map.push_back(ParameterMap("PreviewMode", &preview_mode, ItemTypeInt, HEVCVD_PREVIEW_OFF, 1, -1, 4));
        m_param_map.push_back(ParameterMap("CPUNum", &threads, ItemTypeInt, 0, 1, 0, (std::numeric_limits<uint8_t>::max)()));
        m_param_map.push_back(ParameterMap("InputFile", &inputfile, ItemTypeString, 0, 0, 0.0, 0.0));
        m_param_map.push_back(ParameterMap("OutputFile", &outputfile, ItemTypeString, 0, 0, 0.0, 0.0));
        m_param_map.push_back(ParameterMap("Quiet", &quiet, ItemTypeInt, 0, 0, 0.0, 0.0));
        m_param_map.push_back(ParameterMap("Progress", &progress, ItemTypeInt, 0, 0, 0.0, 0.0));
        m_param_map.push_back(ParameterMap("MD5", &md5, ItemTypeInt, 0, 0, 0, 2));
        m_param_map.push_back(ParameterMap("MD5Frame", &md5_frame, ItemTypeInt, 0, 0, 0, 2));
        m_param_map.push_back(ParameterMap("MaxTemporalLayer", &max_temporal_layer, ItemTypeInt, 6, 1, 0, 6));
        m_param_map.push_back(ParameterMap("Nodeset", &nodeset, ItemTypeInt64, -1, 0, 0, -1));
        m_param_map.push_back(ParameterMap("PrintSeiTypes", &print_sei_types, ItemTypeInt, 0, 0, 0, 0));
        m_param_map.push_back(ParameterMap("UseCallbacks", &use_callbacks, ItemTypeInt, 0, 0, 0, 0));
        m_param_map.push_back(ParameterMap("AsyncInputOutput", &async_intput_output, ItemTypeInt, 0, 0, 0, 0));
        m_param_map.push_back(ParameterMap("VideoFullRangeMode", &cc_pix_range, ItemTypeInt, RANGE_FULL_TO_FULL, 1, 0, 6));
        m_param_map.push_back(ParameterMap("HWAcceleration", &hw_acc_name, ItemTypeString, 0, 0, 0, 0));
        m_param_map.push_back(ParameterMap("HWAdapter", &hw_adapter, ItemTypeInt, ITEM_NOT_INIT, 1, 0, 255));
        m_param_map.push_back(ParameterMap("DeinterlacingMode", &deinterlacing_mode, ItemTypeInt, 0, 1, 0, 4));
    }

    bool initialize(char* const file_name);

    int smp_mode;
    char hw_acc_name[CONFIG_STRING_TYPE_MAX_LEN + 1];
    int hw_adapter;
    int preview_mode;
    uint64_t nodeset;
    int threads;
    int width;
    int height;
    int skip_mode;
    int parse_frames;
    int quiet;
    int progress;
    int md5;
    int md5_frame;
    int max_temporal_layer;
    int print_sei_types;
    int cc_pix_range;
    int use_callbacks;
    int async_intput_output;
    int deinterlacing_mode;
    char fourcc[CONFIG_STRING_TYPE_MAX_LEN + 1];
    char inputfile[CONFIG_STRING_TYPE_MAX_LEN + 1];
    char outputfile[CONFIG_STRING_TYPE_MAX_LEN + 1];

protected:
    struct ParameterMap : public mapparam_t
    {
        ParameterMap(const char* name, void* place, item_type_e type, double def_val, int param_limit, double min_limit, double max_limit)
        {
            this->name = name;
            this->place = place;
            this->type = type;
            this->def_val = def_val;
            this->param_limit = param_limit;
            this->min_limit = min_limit;
            this->max_limit = max_limit;
        }
    };

    std::vector<mapparam_t> m_param_map;
};

#endif
