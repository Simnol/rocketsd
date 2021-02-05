#include "app.hh"

#include <memory>

#include <signal.h>

#include <proto/packet.hh>

int main(int argc, char* argv[])
{
    App app(argc, (char**)argv);
    signal(SIGINT, [](int a){ QCoreApplication::quit(); });
    return app.exec();
}
