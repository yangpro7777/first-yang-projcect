#ifndef UUID_ED53DFC0_673A_44C1_9A5C_4BD401092C0A
#define UUID_ED53DFC0_673A_44C1_9A5C_4BD401092C0A

#include <time.h>
#include <stdio.h>
#include <stdio.h>
#include <atomic>
#include <mutex>
#include <assert.h>

#include <windowsx.h>

#include "dec_hevc.h"

static constexpr uint8_t PICTURE_BUFFER_SIZE = 3;

struct PictureInfo
{
    const hevc_picture_t* pic = nullptr;
    int32_t frame_duration = 0;
};

class AdaptiveDelay
{
public:
    void setFps(double fps) { target_frame_duration = static_cast<int32_t>(1000.0 / fps); }

    void delayFrame(PictureInfo& pic_info)
    {
        if (display_start == 0) {
            display_start = clock();
            display_done = clock();
        }

        if (target_frame_duration <= 0)
            target_frame_duration = pic_info.frame_duration;

        if (target_frame_duration > 0) {
            int32_t current_frame_duration = target_frame_duration + jitter;
            if (current_frame_duration > 0)
                Sleep(current_frame_duration);

            clock_t new_time = clock();
            clock_t frame_duration = (new_time - display_done) * 1000 / CLOCKS_PER_SEC;
            jitter += target_frame_duration - frame_duration;

            display_done = new_time;
        }
        decoded_pic_count++;
        display_done = clock();
    }

    float getAchievedFPS() { return static_cast<float>(decoded_pic_count * CLOCKS_PER_SEC) / static_cast<float>(display_done - display_start); }

    void printStatistics()
    {
        float fps = getAchievedFPS();
        printf("Finished. Frames decoded: %d, average framerate: %3.2f fps\n", decoded_pic_count, fps);
    }

private:
    clock_t display_start = 0;
    clock_t display_done = 0;
    int32_t target_frame_duration = -1;

    int32_t jitter = 0;
    int32_t decoded_pic_count = 0;
};

static void onPaint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(hwnd, &ps);
    EndPaint(hwnd, &ps);
}

static void onDestroy(HWND hwnd) { PostQuitMessage(0); }

static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        HANDLE_MSG(hwnd, WM_DESTROY, onDestroy);
        HANDLE_MSG(hwnd, WM_PAINT, onPaint);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static int messageLoop()
{
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
    }
    return INT(msg.wParam);
}

class Window
{
public:
    bool createWindow();
    void closeWindow() { SendMessage(handle, WM_CLOSE, 0, 0); }
    ~Window() { DestroyWindow(handle); }
    void updateWindowSize(uint32_t width, uint32_t height);

    HWND handle = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

class DecoderContext
{
public:
    void releaseDisplayPicture();

    bufstream_tt* decoder;
    PictureInfo pic_buffer[PICTURE_BUFFER_SIZE];
    uint8_t buffering_pic_count = 0;
    std::atomic_bool stop;

    bool error;

    std::mutex pic_buffer_mutex;
    std::condition_variable picture_released;

    ~DecoderContext()
    {
        if (input_stream)
            fclose(input_stream);
    }

    void decode(class MainContext* context);
    bool setInput(const char* input_filepath);

private:
    FILE* input_stream = nullptr;
};

class MainContext
{
public:
    int process(int argc, char** argv);

    virtual bool setUpDecoder(bufstream_tt* decoder) = 0;

private:
    void display();

    virtual bool createDevice(HWND g_window) = 0;
    virtual bool render(const hevc_picture_t* picture) = 0;

    AdaptiveDelay timing;
    Window window;
    DecoderContext decoder_context;
};

bool Window::createWindow()
{
    static const TCHAR CLASS_NAME[] = TEXT("MainConcept DXVA 2.0 Decoder Sample Class");
    static const TCHAR WINDOW_NAME[] = TEXT("MainConcept DXVA 2.0 Decoder Sample Application");

    HINSTANCE hInst = GetModuleHandle(NULL);

    WNDCLASS wc = {};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc))
        return false;

    handle = CreateWindowEx(WS_EX_TOPMOST, CLASS_NAME, WINDOW_NAME, WS_POPUP, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);

    if (!handle)
        return false;

    ShowWindow(handle, SW_MAXIMIZE);
    UpdateWindow(handle);

    return true;
}

void Window::updateWindowSize(uint32_t width, uint32_t height)
{
    int max_screen_width = GetSystemMetrics(SM_CXSCREEN);
    int max_screen_height = GetSystemMetrics(SM_CYSCREEN);
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;

        int window_width = m_width;
        int window_height = m_height;

        if (max_screen_width < window_width) {
            double scale = (double)max_screen_width / (double)window_width;
            window_width = max_screen_width;
            window_height = static_cast<int>(window_height * scale);
        }
        else if (max_screen_height < window_height) {
            double scale = (double)max_screen_height / (double)window_height;
            window_width = static_cast<int>(window_width * scale);
            window_height = max_screen_height;
        }

        MoveWindow(handle, 0, 0, window_width, window_height, TRUE);
    }
}

void errorCallback(context_t context, const hevc_picture_t* picture, const hevc_runtime_error_t code)
{
    if (code != HARDWARE_ACCELERATION_ERROR)
        return;

    DecoderContext* decoder_context = reinterpret_cast<DecoderContext*>(context.p);
    printf("Error occurred while trying to use hardware acceleration.\n");
    decoder_context->stop = true;
    decoder_context->error = true;
}

//! [Picture addref and release]
void pictureOutputCallback(context_t context, const hevc_picture_t* field_hevc_pic)
{
    DecoderContext* decoder_context = reinterpret_cast<DecoderContext*>(context.p);

    if (field_hevc_pic->d3d11_info == nullptr || !field_hevc_pic->full_frame_available)
        return;

    std::unique_lock<std::mutex> lock(decoder_context->pic_buffer_mutex);
    if (decoder_context->buffering_pic_count >= PICTURE_BUFFER_SIZE)
        decoder_context->picture_released.wait(lock);

    if (decoder_context->stop)
        return;

    PictureInfo& info = decoder_context->pic_buffer[decoder_context->buffering_pic_count];

    if (BS_OK != decoder_context->decoder->auxinfo(decoder_context->decoder, 0, HWACC_GET_PIC, &info.pic, sizeof(hevc_picture_t)))
        return;

    uint32_t result = decoder_context->decoder->auxinfo(decoder_context->decoder, 0, ADDREF_PIC, const_cast<hevc_picture_t*>(info.pic), sizeof(hevc_picture_t));
    assert(result == BS_OK);

    SEQ_ParamsEx* mc_sps = NULL;

    static uint32_t NOT_AVAILABLE = -1;
    int scale = 25, units = 1; // defualt 25 fps
    if (BS_OK == decoder_context->decoder->auxinfo(decoder_context->decoder, 0, GET_SEQ_PARAMSPEX, &mc_sps, sizeof(SEQ_ParamsEx)) &&
        mc_sps->scale != NOT_AVAILABLE) {
        scale = mc_sps->scale;
        units = mc_sps->units;
    }

    info.frame_duration = (uint32_t)(1000.0 * units / scale);
    decoder_context->buffering_pic_count++;
}

bool DecoderContext::setInput(const char* input_filepath)
{
    bool is_ok;
#if _MSC_VER >= 1400
    is_ok = !fopen_s(&input_stream, input_filepath, "rb");
#else
    input_stream = fopen(input_filepath, "rb");
    is_ok = input_stream;
#endif
    if (!is_ok) {
        char* str_error = strerror(errno);
        printf("Failed to open file %s, error: %s\n", input_filepath, str_error);
        input_stream = nullptr;
    }
    return is_ok;
}

void DecoderContext::releaseDisplayPicture()
{
    {
        std::lock_guard<std::mutex> lock(pic_buffer_mutex);
        if (pic_buffer[0].pic) {
            decoder->auxinfo(decoder, 0, RELEASE_PIC, const_cast<hevc_picture_t*>(pic_buffer[0].pic), sizeof(hevc_picture_t));
            pic_buffer[0].pic = 0;
        }

        buffering_pic_count--;
        for (uint32_t i = 0; i < buffering_pic_count; i++)
            pic_buffer[i] = pic_buffer[i + 1];
    }

    picture_released.notify_all();
}

//! [Picture addref and release]
void DecoderContext::decode(MainContext* context)
{
#ifdef DEMO_LOGO
    static const char library_name[] = "demo_dec_hevc";
#else
    static const char library_name[] = "mc_dec_hevc";
#endif

    static const char name_func_create[] = "createDecoderHEVC";

    HMODULE library = LoadLibraryA(library_name);
    if (!library) {
        printf("Error: failed to load \"%s\" library.\n", library_name);
        error = true;
    }

    callbacks_t callbacks{};
    callbacks.context.p = this;
    callbacks_decoder_hevc_t decoder_callbacks{};
    decoder_callbacks.error_callback = errorCallback;
    decoder_callbacks.pic_output_callback = pictureOutputCallback;

    if (library) {
        typedef decltype(&::createDecoderHEVC) createDecoderHEVC_t;
        createDecoderHEVC_t createDecoder = reinterpret_cast<createDecoderHEVC_t>(GetProcAddress(library, name_func_create));
        decoder = createDecoder(&callbacks, &decoder_callbacks, 0);

        if (!decoder) {
            printf("Error: failed to create Decoder instance.\n");
            error = true;
        }
    }

    // Try configuring the decoder to use d3d hardware acceleration with the provided device manager
    if (decoder && context->setUpDecoder(decoder)) {

        decoder->auxinfo(decoder, HEVCVD_DEINTERLACING_INTERFIELD_INTERPOLATION, HEVCVD_SET_DEINTERLACING_MODE, 0, 0);

        // Tell the decoder that one extra picture should be allocated (some pictures will be held until shown)
        decoder->auxinfo(decoder, PICTURE_BUFFER_SIZE + 1, SET_MAX_HELD_PICTURES, 0, 0);
        // Enable asynchronous output mode
        decoder->auxinfo(decoder, 1, SET_ASYNC_INPUT_OUTPUT_MODE, 0, 0);

        static constexpr size_t BUFFER_BITSTREAM_SIZE = 1024 * 1024;
        uint8_t* bitstream_buffer = reinterpret_cast<uint8_t*>(malloc(BUFFER_BITSTREAM_SIZE));
        // Decode the stream
        size_t byte_count = 0;
        uint32_t consumed;
        while (!stop && (byte_count = fread(bitstream_buffer, 1, BUFFER_BITSTREAM_SIZE, input_stream))) {
            consumed = 0;
            do {
                consumed += decoder->copybytes(decoder, bitstream_buffer + consumed, (uint32_t)byte_count - consumed);
            } while (!stop && byte_count - consumed);
        }
        free(bitstream_buffer);

        // Flush remaining frames from the decoder
        consumed = 1;
        while (!stop && consumed)
            consumed = decoder->copybytes(decoder, NULL, 0);
    }
    else {
        printf("Error: can't use hardware acceleration.\n");
        error = true;
    }

    // Wait until all buffered pictures are shown
    while (!stop && buffering_pic_count)
        Sleep(0);

    stop = true;

    if (decoder)
        close_bufstream(decoder, false);
    if (library)
        FreeLibrary(library);
}

void MainContext::display()
{
    while (true) {
        if (decoder_context.buffering_pic_count) {
            const hevc_picture_t* picture = decoder_context.pic_buffer[0].pic;

            window.updateWindowSize(picture->width, picture->height);
            if (!render(picture)) {
                decoder_context.releaseDisplayPicture();
                decoder_context.stop = true;
                decoder_context.error = true;
                break;
            }

            // Release first picture in buffer. It was already shown.
            decoder_context.releaseDisplayPicture();
            timing.delayFrame(decoder_context.pic_buffer[0]);
        }

        if (decoder_context.stop)
            break;
    }

    // Release all held pictures before exiting display thread in case of stopping application.
    while (decoder_context.buffering_pic_count)
        decoder_context.releaseDisplayPicture();

    window.closeWindow();
}

int MainContext::process(int argc, char** argv)
{
    char* input_filepath = nullptr;
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "-fps", 6)) {
            double fps = atof(argv[++i]);
            if (fps > 0.1) {
                timing.setFps(fps);
            }
        }
        else if (!strncmp(argv[i], "-hevc", 4)) {
            input_filepath = argv[++i];
        }
    }

    if (!input_filepath) {
        printf("\n-<hevc> <stream file name>");
        printf("\n-fps <target framerate. 0 - not restricted. Default - use stream "
               "parameters.>");
        return 0;
    }

    if (!decoder_context.setInput(input_filepath))
        return -1;

    if (!window.createWindow() || !createDevice(window.handle))
        return -1;

    using namespace std;
    unique_ptr<thread> decode_thread;
    unique_ptr<thread> display_thread;

    try {
        display_thread = make_unique<thread>(&MainContext::display, this);
        try {
            decode_thread = make_unique<thread>(&DecoderContext::decode, &decoder_context, this);
        }
        catch (const std::bad_alloc&) {
            decoder_context.error = true;
            printf("Error: can't create decode thread\n");
        }
    }
    catch (const std::bad_alloc&) {
        decoder_context.error = true;
        printf("Error: can't create display thread\n");
    }

    if (decoder_context.error)
        window.closeWindow();

    messageLoop();
    decoder_context.stop = true;

    if (decode_thread)
        decode_thread->join();
    if (display_thread)
        display_thread->join();

    if (!decoder_context.error) {
        timing.printStatistics();
        return 0;
    }
    else
        return -1;
}

#endif
