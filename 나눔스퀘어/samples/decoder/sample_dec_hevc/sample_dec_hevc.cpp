/********************************************************************
 File name: sample_dec_hevc.cpp
 Purpose: standard command-line sample for HEVC decoder

 Copyright (c) 2016 MainConcept GmbH or its affiliates.  All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties.  Unauthorized
 reproduction or distribution of any portion is prohibited by law.
*********************************************************************/

#include "unicode_tools.h"
#include "decoder.h"

int main(int argc, char* argv[])
{
#if defined(_WIN32)
    setvbuf(stderr, NULL, _IONBF, 0);
#endif

    std::vector<const char*> ansi_arguments = UnicodeTools::initialize(argc, argv);

    argc = static_cast<int>(ansi_arguments.size());
    argv = const_cast<char**>(&ansi_arguments[0]);

    Helper helper;
    helper.printHeader();

    CommandLine command_line;
    if (!command_line.initialize(argc, argv))
        return 1;

    Configuration config;
    if (!config.initialize(command_line.config_file_name))
        return 1;

    if (!helper.initialize(command_line, config))
        return 1;

    Decoder decoder(helper);
    if (!decoder.initialize(createDecoderHEVC))
        return 1;

    if (helper.hw_enumerate) {
        decoder.showAdapters();
        return 0;
    }

    helper.start();

    /////////////////////////////////////////////////////
    // Call the main decode loop to process the HEVC file
    /////////////////////////////////////////////////////
    int result = decoder.loop();
    if (!result)
        result = decoder.flush();

    helper.finish();
    helper.printStreamSummary();
    helper.printSummary();

    fprintf(helper.log, "\n");

    return result;
}
