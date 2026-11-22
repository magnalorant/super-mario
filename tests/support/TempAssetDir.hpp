#pragma once

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace mario::test {

class TempAssetDir {
public:
    TempAssetDir() {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        const std::string name = std::string("mario_") + info->test_suite_name() + "_" + info->name();
        root_ = std::filesystem::temp_directory_path() / name;
        std::filesystem::remove_all(root_);
        std::filesystem::create_directories(root_ / "levels");
    }

    ~TempAssetDir() {
        std::error_code ec;
        std::filesystem::remove_all(root_, ec);
    }

    TempAssetDir(const TempAssetDir&) = delete;
    TempAssetDir& operator=(const TempAssetDir&) = delete;
    TempAssetDir(TempAssetDir&&) = delete;
    TempAssetDir& operator=(TempAssetDir&&) = delete;

    void writeLevel(const std::string& file, const std::string& content) const {
        std::ofstream(root_ / "levels" / file) << content;
    }

    [[nodiscard]] const std::filesystem::path& path() const noexcept { return root_; }

private:
    std::filesystem::path root_;
};

}  // namespace mario::test
