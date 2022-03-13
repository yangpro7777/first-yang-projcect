#ifndef UUID_CD3A2835_5384_4362_8CCF_CC4206CE12F3
#define UUID_CD3A2835_5384_4362_8CCF_CC4206CE12F3

#include <mutex>
#include "instance.h"
#include "stream.h"
#include "sample_common_args.h"

#define ARG_THREADS (IDC_CUSTOM_START_ID + 1)
#define ARG_MULTIPROCESSING (IDC_CUSTOM_START_ID + 2)
#define ARG_ASYNC_OUTPUT (IDC_CUSTOM_START_ID + 3)
#define ARG_ASYNC_INPUT (IDC_CUSTOM_START_ID + 4)
#define ARG_TOOLSET (IDC_CUSTOM_START_ID + 5)
#define ARG_ADAPTER (IDC_CUSTOM_START_ID + 6)
#define ARG_INSTANCES (IDC_CUSTOM_START_ID + 7)
#define ARG_PICTURES (IDC_CUSTOM_START_ID + 8)
#define ARG_REORDER (IDC_CUSTOM_START_ID + 9)
#define ARG_COPYBACK (IDC_CUSTOM_START_ID + 10)
#define ARG_CHUNKS (IDC_CUSTOM_START_ID + 11)
#define ARG_PROGRESS (IDC_CUSTOM_START_ID + 12)
#define ARG_LEGEND (IDC_CUSTOM_START_ID + 13)
#define ARG_HEADERS (IDC_CUSTOM_START_ID + 14)
#define ARG_PEDANTIC (IDC_CUSTOM_START_ID + 15)
#define ARG_POC (IDC_CUSTOM_START_ID + 16)
#define ARG_PDC (IDC_CUSTOM_START_ID + 17)
#define ARG_OFFSET (IDC_CUSTOM_START_ID + 18)
#define ARG_GOPS (IDC_CUSTOM_START_ID + 19)
#define ARG_FRAMES (IDC_CUSTOM_START_ID + 20)
#define ARG_MILLISECONDS (IDC_CUSTOM_START_ID + 21)
#define ARG_IFR (IDC_CUSTOM_START_ID + 22)
#define ARG_DFR (IDC_CUSTOM_START_ID + 23)
#define ARG_OFR (IDC_CUSTOM_START_ID + 24)
#define ARG_LOOPS (IDC_CUSTOM_START_ID + 25)
#define ARG_THREADPOOL_TYPE (IDC_CUSTOM_START_ID + 26)
#define ARG_THREADPOOL_SHARED (IDC_CUSTOM_START_ID + 27)
#define ARG_THREADPOOL_ORDERING (IDC_CUSTOM_START_ID + 28)
#define ARG_COUNT 28

// Rate restrictions for pipeline stages
struct Framerate
{
    int32_t input = 0;
    int32_t decode = 0;
    int32_t output = 0;
};

// Latency measurement application business logic
class Application
{
public:
    typedef std::function<void(Application&)> Hook;
    typedef std::function<void(Application&, Stream&)> Fork;

    Application(int argc, char* argv[], const callbacks_decoder_hevc_t& callbacks)
    {
        executable = argv[0];

        int offset = 1;
        while (offset < argc && strcmp(argv[offset++], "--"))
            ;

        parse_args(offset - !!(argc - offset) - 1, argv + 1, ARG_COUNT, ARGUMENTS, DESCRIPTIONS, ARG_COUNT);

        if (threads == ITEM_NOT_INIT)
            threads = -1;
        if (smp == ITEM_NOT_INIT)
            smp = HEVCVD_SMP_OVERLAPPED;
        if (async_output == ITEM_NOT_INIT)
            async_output = 0;
        if (async_input == ITEM_NOT_INIT)
            async_input = 0;
        if (framerate.input == ITEM_NOT_INIT)
            framerate.input = 0;
        if (framerate.decode == ITEM_NOT_INIT)
            framerate.decode = 0;
        if (framerate.output == ITEM_NOT_INIT)
            framerate.output = 0;
        if (icount == ITEM_NOT_INIT)
            icount = 1;
        if (icount > MAX_COUNT_INSTANCES)
            icount = MAX_COUNT_INSTANCES;
        if (pictures == ITEM_NOT_INIT)
            pictures = 0;
        if (pictures > MAX_COUNT_PICTURES)
            pictures = MAX_COUNT_PICTURES;
        if (reorder == ITEM_NOT_INIT)
            reorder = 0;
        if (copyback == ITEM_NOT_INIT)
            copyback = 0;
        if (toolset == ITEM_NOT_INIT)
            toolset = HEVCVD_DECODING_TOOLSET_CPU;
        if (adapter == ITEM_NOT_INIT)
            adapter = 0;
        if (chunks == ITEM_NOT_INIT)
            chunks = HEVCVD_CP_AU;
        if (progress == ITEM_NOT_INIT)
            progress = 0;
        if (legend == ITEM_NOT_INIT)
            legend = 0;
        if (headers == ITEM_NOT_INIT)
            headers = 0;
        if (pedantic == ITEM_NOT_INIT)
            pedantic = 0;
        if (poc == ITEM_NOT_INIT)
            poc = 0;
        if (pdc == ITEM_NOT_INIT)
            pdc = 0;
        if (offset == ITEM_NOT_INIT)
            offset = 0;
        if (loops == ITEM_NOT_INIT)
            loops = 1;
        if (loops > (std::numeric_limits<uint8_t>::max)())
            loops = (std::numeric_limits<uint8_t>::max)();
        if (gops == ITEM_NOT_INIT)
            gops = (std::numeric_limits<uint32_t>::max)();
        if (milliseconds == ITEM_NOT_INIT)
            milliseconds = 0;
        if (threadpool.type == ITEM_NOT_INIT || threadpool.type < 0 || threadpool.type > TP_TYPE_GCD)
            threadpool.type = -1;
        if (threadpool.shared == ITEM_NOT_INIT || threadpool.type == -1)
            threadpool.shared = 0;
        if (threadpool.ordering == ITEM_NOT_INIT)
            threadpool.ordering = 0;

        // Create streams
        for (uint8_t i = 0; i < std::min<int>(argc - offset, icount); ++i)
            streams.push_back(new Stream(argv[offset + i]));

        // Deduce the optimal number of producers to be used by the threadpool scheduler
        threadpool.producer_count = threadpool.shared ? static_cast<uint16_t>(async_input ? icount : streams.size()) : 1;

        // Create decoder instances
        if (!streams.empty()) {
            const char* frames_cursor = frames;
            for (uint8_t i = 0; i < icount; ++i) {
                instances.push_back(new Instance(i, callbacks, this, streams[i % streams.size()]));
                if (frames_cursor) {
                    instances[i]->frames = atoi(frames_cursor);
                    frames_cursor = (frames_cursor = strchr(frames_cursor, '+')) ? frames_cursor + 1 : nullptr;
                }
            }
        }

        // Create external threadpool if enabled
        if (threadpool.shared && !threadpool.open())
            printf("Failed to create external threadpool type %d\n", threadpool.type);
    }

    ~Application()
    {
        for (auto& stream : streams)
            delete stream;

        for (auto& instance : instances)
            delete instance;

        threadpool.close();
    }

    bool initialized() const noexcept { return !streams.empty() && streams[0]->handle != nullptr; }

    int main(const Hook& loop, const Hook& finalize, const Instance::Hook& progress) noexcept
    {
        if (!initialized()) {
            fprintf(stderr, "\nUSAGE:\n%s [options] -- <stream1> [<stream2> ... <stream%d>]\n", executable, MAX_COUNT_STREAMS);
            fprintf(stderr, "\nOPTIONS:\n");
            print_help(ARGUMENTS, ARG_COUNT, DESCRIPTIONS + 1, ARG_COUNT - 1);
            return -1;
        }

        // Collect auxiliary info about input elementary streams (NALUs, AUs, etc.)
        for (auto& stream : streams)
            if (!stream->parse(this))
                printf("Failed to collect auxiliary info about the stream\n");

        // Decode the stream several times in the loop
        if (setup(progress)) {
            loop(*this);

            // Finalize decoders' output
            for (auto& instance : instances)
                instance->flush();

            // Close decoders
            for (auto& instance : instances)
                instance->close();
        }

        // Output statistics
        bool error = false;
        for (auto& instance : instances)
            error |= instance->error != NO_RUNTIME_ERROR;

        if (!error)
            finalize(*this);

        return 0;
    }

    bool setup(const Instance::Hook& logger) noexcept
    {
        for (auto& instance : instances) {
            instance->logger = logger;

            if (!instance->open()) {
                printf("Failed to create decoder instance\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(reorder ? INTERN_REORDERING_FLAG : 0, PARSE_OPTIONS) != BS_OK) {
                printf("Failed to set reordering mode\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(async_output, SET_ASYNC_INPUT_OUTPUT_MODE) != BS_OK) {
                printf("Failed to set asynchronous I/O mode\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(chunks, SET_CHUNK_PARSE_MODE) != BS_OK) {
                printf("Failed to set chunk parse mode\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(threads, SET_CPU_NUM) != BS_OK) {
                printf("Failed to set the number of worker threads\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(smp, SET_SMP_MODE) != BS_OK) {
                printf("Failed to set SMP mode\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (instance->auxinfo(pictures, SET_MAX_OVERLAPPED_PICTURES) != BS_OK) {
                printf("Failed to set the number of extra pictures in DPB\n");
                instance->error = UNSUPPORTED_FEATURE;
                return false;
            }

            if (toolset > HEVCVD_DECODING_TOOLSET_CPU) {
                if (instance->auxinfo(toolset, HWACC_SET_ACC_MODE) != BS_OK) {
                    printf("Failed to set the decoding toolset\n");
                    instance->error = UNSUPPORTED_FEATURE;
                    return false;
                }

                if (adapter == ITEM_NOT_INIT || instance->auxinfo(adapter, SET_HW_ADAPTER) != BS_OK) {
                    printf("Failed to set the adapter for HW accelerated decoding\n");
                    instance->error = UNSUPPORTED_FEATURE;
                    return false;
                }
            }
        }

        return true;
    }

    // Read and decode the input stream by multiple instances in the loop. Default implementation
    static void default_fork_point(Application& application, Stream& stream) noexcept
    {
        size_t offset{};
        const std::function<bool(const Stream&, uint8_t*, size_t)> consume = [&](const Stream& stream, uint8_t* buffer, size_t size) noexcept->bool
        {
            for (size_t count = stream.instances.size(), limit = count + offset, i = offset++; i < limit; ++i) {
                Instance* instance = stream.instances[i % count];
                if (instance->stop)
                    return false;
                instance->consume(buffer, size);
            }
            return true;
        };

        // Start reading the input stream in the loop
        for (stream.loop = 0; stream.loop < application.loops && fseek(stream.handle, 0, SEEK_SET) == 0; ++stream.loop) {
            if (application.chunks == HEVCVD_CP_AU) {
                for (const AccessUnit& chunk : stream.aus)
                    if (!consume(stream, stream.read(chunk.size), chunk.size))
                        break;
            }
            else if (application.chunks == HEVCVD_CP_NALU) {
                for (const NalUnit& chunk : stream.nalus)
                    if (!consume(stream, stream.read(chunk.size), chunk.size))
                        break;
            }
            else {
                size_t size{};
                for (uint8_t* buffer = stream.read(&size); size > 0; stream.read(&size))
                    if (!consume(stream, buffer, size))
                        break;
            };
        }
    };

    void record(const hevc_seq_par_set_t* sps) const noexcept
    {
        static const char* SUBSAMPLING[] = { "4:0:0", "4:2:0", "4:2:2", "4:4:4", "2:1:1", "4:1:0", "4:1:1", "8:1:0" };
        printf("%4dx%-4d |*| BITDEPTH=%02dL/%02dC  SUBSAMPLING=%s  CTU=%2d  BUFFERING=%-2d REORDERING=%-2d LATENCY=%-2d  AMP=%d  TMVP=%d  PCM=%d  SAO=%d\n",
            sps->pic_width_in_luma_samples, sps->pic_height_in_luma_samples, sps->bit_depth_luma, sps->bit_depth_chroma, SUBSAMPLING[sps->chroma_format_idc],
            1 << (sps->log2_diff_max_min_coding_block_size + sps->log2_min_coding_block_size), sps->sps_max_dec_pic_buffering[0] - 1,
            sps->sps_max_num_reorder_pics[0], sps->sps_max_latency_increase[0] ? sps->sps_max_latency_increase[0] + 1 : 0, sps->amp_enabled_flag > 0,
            sps->sps_temporal_mvp_enabled_flag > 0, sps->pcm_enabled_flag > 0, sps->sample_adaptive_offset_enabled_flag > 0);
    }

    void record(const hevc_pic_par_set_t* pps) const noexcept
    {
        printf(" |*| TILES=%02dx%02d  DSEGMENTS=%d  SLISTS=%d  LF_ACROSS_SL/TL=%d/%d  TRSKIP=%d  TBP=%d  CINTRA=%d  TILE=%d  WPP=%d  DBF=%d  DQP=%d\n",
            pps->num_tile_columns, pps->num_tile_rows, pps->dependent_slice_segments_enabled_flag > 0, pps->constrained_intra_pred_flag > 0,
            pps->transform_skip_enabled_flag > 0, pps->scaling_list_data_present_flag > 0, pps->loop_filter_across_slices_enabled_flag > 0,
            pps->loop_filter_across_tiles_enabled_flag > 0, pps->transquant_bypass_enabled_flag > 0, pps->tiles_enabled_flag > 0,
            pps->entropy_coding_sync_enabled_flag > 0, pps->pps_deblocking_filter_disabled_flag == 0 || pps->deblocking_filter_override_enabled_flag != 0,
            pps->cu_qp_delta_enabled_flag > 0);
    }

    std::vector<Instance*> instances{};
    std::vector<Stream*> streams{};

    mutable std::mutex mutex{};

    char* executable = nullptr;

    int32_t threads = -1;
    int32_t smp = HEVCVD_SMP_OVERLAPPED;
    int32_t async_output = 0;
    int32_t async_input = 0;
    int32_t pictures = 0;
    int32_t reorder = 0;
    int32_t copyback = 0;
    int32_t toolset = HEVCVD_DECODING_TOOLSET_CPU;
    int32_t adapter = 0;
    int32_t chunks = HEVCVD_CP_AU;
    int32_t progress = 0;
    int32_t legend = 0;
    int32_t headers = 0;
    int32_t pedantic = 0;
    int32_t poc = 0;
    int32_t pdc = 0;
    int32_t offset = 0;

    uint32_t loops = 1;
    char* frames = nullptr;
    uint32_t milliseconds = 0;
    uint32_t gops = (std::numeric_limits<uint32_t>::max)();

    Framerate framerate{};
    Threadpool threadpool{};

private:
    int32_t icount = 0;

    arg_item_t ARGUMENTS[ARG_COUNT] = { { ARG_THREADS, 0, &threads }, { ARG_MULTIPROCESSING, 0, &smp }, { ARG_INSTANCES, 0, &icount },
        { ARG_PICTURES, 0, &pictures }, { ARG_TOOLSET, 0, &toolset }, { ARG_ADAPTER, 0, &adapter }, { ARG_CHUNKS, 0, &chunks }, { ARG_OFFSET, 0, &offset },
        { ARG_GOPS, 0, &gops }, { ARG_FRAMES, 0, &frames }, { ARG_MILLISECONDS, 0, &milliseconds }, { ARG_POC, 0, &poc }, { ARG_PDC, 0, &pdc },
        { ARG_ASYNC_OUTPUT, 0, &async_output }, { ARG_ASYNC_INPUT, 0, &async_input }, { ARG_REORDER, 0, &reorder }, { ARG_COPYBACK, 0, &copyback },
        { ARG_PROGRESS, 0, &progress }, { ARG_LEGEND, 0, &legend }, { ARG_HEADERS, 0, &headers }, { ARG_PEDANTIC, 0, &pedantic },
        { ARG_IFR, 0, &framerate.input }, { ARG_DFR, 0, &framerate.decode }, { ARG_OFR, 0, &framerate.output }, { ARG_LOOPS, 0, &loops },
        { ARG_THREADPOOL_TYPE, 0, &threadpool.type }, { ARG_THREADPOOL_SHARED, 0, &threadpool.shared }, { ARG_THREADPOOL_ORDERING, 0, &threadpool.ordering } };

    // clang-format off
    arg_item_desc_t DESCRIPTIONS[ARG_COUNT] = {
        { ARG_THREADS, { "threads", "" }, ItemTypeInt, -1,                              "The number of worker threads.                                      |  By default the number of threads is set to the number of CPU cores" },
        { ARG_THREADPOOL_TYPE, { "threadpool", "" }, ItemTypeInt, TP_TYPE_SCALABLE,     "The type of an external threadpool to be used by decoders.         |  By default the internal threadpool implementation {-1} is used" },
        { ARG_MULTIPROCESSING, { "smp", "" }, ItemTypeInt, HEVCVD_SMP_OVERLAPPED,       "Choose ST {0}, overlapped MT {1}, or concurrent MT {2} decoding.   |  By default the overlapped MT {1} is choosen" },
        { ARG_INSTANCES, { "instances", "" }, ItemTypeInt, 0,                           "The number of decoder instances running in parallel (max. 64).     |  By default the number of instances is set to 1" },
        { ARG_PICTURES, { "pictures", "" }, ItemTypeInt, 0,                             "The number of extra pictures to use in DPB (max. 255).             |  By default the number of extra pictures is set to 0. Set -1 to enable auto-configuration" },
        { ARG_TOOLSET, { "toolset", "" }, ItemTypeInt, HEVCVD_DECODING_TOOLSET_CPU,     "Choose decoding on CPU {0} or GPU. Choose D3D9 {1} or D3D11 {2}.   |  By default the CPU toolset {0} is choosen" },
        { ARG_ADAPTER, { "adapter", "" }, ItemTypeInt, ITEM_NOT_INIT,                   "Choose GPU adapter by serial. Ignored when CPU toolset is chosen.  |  By default the GPU is selected by certain internal criteria" },
        { ARG_CHUNKS, { "chunks", "" }, ItemTypeInt, HEVCVD_CP_AU,                      "Feed decoder with random data chunks {0}, NALUs {1}, or AUs {2}.   |  By default the decoder is fed by AUs" },
        { ARG_OFFSET, { "offset", "" }, ItemTypeInt, 0,                                 "The number of GOPs to skip from the beginning of a stream.         |  By default no, i.e. zero, GOPs are skipped" },
        { ARG_GOPS, { "gops", "" }, ItemTypeInt, 0,                                     "The number of GOPs to decode.                                      |  By default all GOPs are decoded starting from the offset" },
        { ARG_FRAMES, { "frames", "" }, ItemTypeString, 0,                              "The number of frames to decode per instance.                       |  By default all frames are decoded (0), instances separated by commas" },
        { ARG_MILLISECONDS, { "milliseconds", "" }, ItemTypeInt, 0,                     "The number of milliseconds to decode.                              |  By default there is no limit (0)" },
        { ARG_POC, { "poc", "" }, ItemTypeNoArg, 1,                                     "Display the progress in a picture output order (sort by POC).      |  By default the progress is displayed in a picture release order (sorted by PRC)" },
        { ARG_PDC, { "pdc", "" }, ItemTypeNoArg, 1,                                     "Display the progress in a picture decode order (sort by PDC).      |  By default the progress is displayed in a picture release order (sorted by PRC)" },
        { ARG_LOOPS, { "loops", "" }, ItemTypeInt, 1,                                   "The number of loops to run the decoder in (max. 255).              |  By default the single loop is used" },
        { ARG_IFR, { "irate", "" }, ItemTypeInt, 0,                                     "Input framerate. Affects the input thread.                         |  Not applied by default" },
        { ARG_DFR, { "drate", "" }, ItemTypeInt, 0,                                     "Decoding framerate. Affects worker threads.                        |  Not applied by default" },
        { ARG_OFR, { "orate", "" }, ItemTypeInt, 0,                                     "Output framerate. Affects the output thread.                       |  Not applied by default" },
        { ARG_THREADPOOL_SHARED, { "shared", "" }, ItemTypeNoArg, 1,                    "Share a single external threadpool by multiple decoder instances.  |  Disabled by default" },
        { ARG_THREADPOOL_ORDERING, { "ordering", "" }, ItemTypeInt, ITEM_NOT_INIT,      "Ordering used by external LockfreeThreadPool if selected.          |  Strict FIFO by default" },
        { ARG_ASYNC_OUTPUT, { "async_output", "" }, ItemTypeNoArg, 1,                   "Enable asynchronous output.                                        |  Disabled by default" },
        { ARG_ASYNC_INPUT, { "async_input", "" }, ItemTypeNoArg, 1,                     "Enable asynchronous input.                                         |  Disabled by default" },
        { ARG_REORDER, { "reorder", "" }, ItemTypeNoArg, 1,                             "Send pictures to output in output (not stream) order.              |  Disabled by default" },
        { ARG_COPYBACK, { "copyback", "" }, ItemTypeNoArg, 1,                           "Copy surface from GPU to CPU. Ignored when CPU toolset is chosen.  |  Disabled by default" },
        { ARG_PROGRESS, { "progress", "" }, ItemTypeNoArg, 1,                           "Display detailed statistics for each picture.                      |  Disabled by default" },
        { ARG_LEGEND, { "legend", "" }, ItemTypeNoArg, 1,                               "Display the legend.                                                |  Disabled by default" },
        { ARG_HEADERS, { "headers", "" }, ItemTypeNoArg, 1,                             "Display the most significant fields from SPS and PPS.              |  Disabled by default" },
        { ARG_PEDANTIC, { "pedantic", "" }, ItemTypeNoArg, 1,                           "Display error messages and stop decoding on errors.                |  Disabled by default" } };
    // clang-format on
};
#endif
