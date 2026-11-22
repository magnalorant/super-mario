#include <gtest/gtest.h>

#include "mario/core/CommandLine.hpp"

namespace mario {
namespace {

TEST(CommandLineTest, NoArgumentsGivesDefaults) {
    const CommandLineOptions options = parseCommandLine({});
    EXPECT_FALSE(options.assetDir.has_value());
    EXPECT_FALSE(options.frameLimit.has_value());
    EXPECT_FALSE(options.verbose);
    EXPECT_FALSE(options.showHelp);
}

TEST(CommandLineTest, ParsesAllOptions) {
    const CommandLineOptions options = parseCommandLine({"--assets", "data", "-v", "--frames", "120", "-h"});
    ASSERT_TRUE(options.assetDir.has_value());
    EXPECT_EQ(*options.assetDir, "data");
    EXPECT_EQ(options.frameLimit, 120);
    EXPECT_TRUE(options.verbose);
    EXPECT_TRUE(options.showHelp);
}

TEST(CommandLineTest, AcceptsAssetsWithEqualsSign) {
    const CommandLineOptions options = parseCommandLine({"--assets=/opt/mario"});
    EXPECT_EQ(*options.assetDir, "/opt/mario");
}

TEST(CommandLineTest, RejectsUnknownOptions) {
    EXPECT_THROW((void)parseCommandLine({"--fly"}), CommandLineError);
}

TEST(CommandLineTest, RejectsMissingValues) {
    EXPECT_THROW((void)parseCommandLine({"--assets"}), CommandLineError);
    EXPECT_THROW((void)parseCommandLine({"--frames"}), CommandLineError);
    EXPECT_THROW((void)parseCommandLine({"--assets="}), CommandLineError);
}

TEST(CommandLineTest, RejectsInvalidFrameCounts) {
    EXPECT_THROW((void)parseCommandLine({"--frames", "abc"}), CommandLineError);
    EXPECT_THROW((void)parseCommandLine({"--frames", "0"}), CommandLineError);
    EXPECT_THROW((void)parseCommandLine({"--frames", "12x"}), CommandLineError);
}

TEST(CommandLineTest, UsageMentionsProgramAndOptions) {
    const std::string text = usage("super_mario");
    EXPECT_NE(text.find("super_mario"), std::string::npos);
    EXPECT_NE(text.find("--assets"), std::string::npos);
    EXPECT_NE(text.find("--help"), std::string::npos);
}

}  // namespace
}  // namespace mario
