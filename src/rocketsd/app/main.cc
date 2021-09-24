#include "app.hh"

#include <iostream>
#include <filesystem>

#include <signal.h>

#include <google/protobuf/stubs/common.h>

#include <log/log.hh>
#include <log/ostream_logsink.hh>

namespace {

struct LaunchArgs {
    std::filesystem::path config_path;
    std::filesystem::path xml_path;
};

void usage()
{
    std::cout << R"~~(usage: rocketsd <options>

optional arguments:
    -c, --config    Path to configuration JSON file. Defaults to "config.json".
    -p, --protocol  Path to protocol XML file. Defaults to "protocol.xml".
    -h, --help      Displays this message.
)~~" << std::endl;
}

std::pair<LaunchArgs, bool> parseArgs(int argc, char* argv[], LaunchArgs args = {})
{
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0) {
            if (i < argc - 1) {
                args.config_path = argv[++i];
                continue;
            }
        }

        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--protocol") == 0) {
            if (i < argc - 1) {
                args.xml_path = argv[++i];
                continue;
            }
        }

        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage();
            return {{}, true};
        }

        // Unknown switch
        std::cout << "unrecognized argument: " << argv[i] << std::endl;
        usage();
        return {{}, true};
    }

    return {args, false};
}

}

int main(int argc, char* argv[])
{
    auto clean_exit = [](int rc) {
        google::protobuf::ShutdownProtobufLibrary();
        return rc;
    };

    logging::OstreamLogSink cout(std::cout);
    logging::Log::get().addSink(&cout);

    auto [args, error] = parseArgs(argc, argv, { 
        .config_path = "config.json",
        .xml_path = "protocol.xml"
    });

    if (error) {
        return clean_exit(EXIT_FAILURE);
    }

    if (!std::filesystem::is_regular_file(args.config_path)) {
        logging::err("main") << "Could not find configuration file " << args.config_path << logging::endl;
        return clean_exit(EXIT_FAILURE);
    }

    if (!std::filesystem::is_regular_file(args.xml_path)) {
        logging::err("main") << "Could not find protocol file " << args.xml_path << logging::endl;
        return clean_exit(EXIT_FAILURE);
    }

    rocketsd::app::App app(args.config_path, args. xml_path, argc, (char**)argv);
    signal(SIGINT, [](int /*a*/){ QCoreApplication::quit(); });

    return clean_exit(app.exec());
}
