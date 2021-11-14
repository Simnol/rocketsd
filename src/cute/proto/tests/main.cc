#include <gtest/gtest.h>

#include <google/protobuf/stubs/common.h>

int main(int argc, const char* argv[])
{
    testing::InitGoogleTest(&argc, (char**)argv);
    int rc = RUN_ALL_TESTS();

    google::protobuf::ShutdownProtobufLibrary();
    return rc;
}