#include <stdlib.h>
#include <stdio.h>
#include "configuration.h"
#include "unicode_tools.h"

// Parsing configuration file
bool Configuration::initialize(char* const file_name)
{
    FILE* f_cfg = NULL;

    if (file_name) {
        f_cfg = UnicodeTools::openFile(file_name, true);
    }
    else {
        char cfg_buffer;
        if (parse_config_params(&cfg_buffer, 0, &m_param_map[0], m_param_map.size())) {
            return true;
        }
        else {
            fprintf(stderr, "\nFailed to setup default config values\n");
            return false;
        }
    }

    if (!f_cfg) {
        UnicodeTools::printf(stderr, "\nCan`t open configuration file '%s'\n", UnicodeTools::unicodeArgument(file_name).c_str());
        return false;
    }

    fseek(f_cfg, 0, SEEK_END);
    int file_len = ftell(f_cfg);
    fseek(f_cfg, 0, SEEK_END);
    const size_t MAX_CONFIG_FILESIZE = 64 * 1024;
    if (file_len < 0 || file_len > MAX_CONFIG_FILESIZE) {
        fprintf(stderr, "\nUnreasonable size of configuration file\n");
        return false;
    }

    fseek(f_cfg, 0, SEEK_SET);
    char* cfg_buffer = (char*)malloc(file_len + 1);
    if (file_len && !fread(cfg_buffer, 1, file_len, f_cfg)) {
        fprintf(stderr, "\nError reading configuration file\n");
        free(cfg_buffer);
        return false;
    }

    cfg_buffer[file_len] = '\0';
    fclose(f_cfg);
    f_cfg = 0;

    if (!parse_config_params(cfg_buffer, file_len, &m_param_map[0], m_param_map.size())) {
        free(cfg_buffer);
        fprintf(stderr, "\nInvalid configuration file ");
        return false;
    }

    free(cfg_buffer);
    return true;
}
