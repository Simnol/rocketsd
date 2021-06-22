#include "app.hh"

#include <iostream>

#include <signal.h>

#include <google/protobuf/stubs/common.h>

#include <log/log.hh>
#include <log/ostream_logsink.hh>

int main(int argc, char* argv[])
{
    logging::OstreamLogSink cout(std::cout);
    logging::Log::get().addSink(&cout);

    rocketsd::app::App app(argc, (char**)argv);
    signal(SIGINT, [](int /*a*/){ QCoreApplication::quit(); });
 
    int rc = app.exec();
    google::protobuf::ShutdownProtobufLibrary();
    return rc;
}
