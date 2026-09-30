#pragma once

#include "../UserInterface/Locale_inc.h"
#ifdef USE_HOT_RELOAD
#include "../eterBase/Singleton.h"
#include <filesystem>
#include <algorithm>

#include <future>

class FileWatcher : public CSingleton<FileWatcher>
{
public:
    using Callback = std::function<void(const std::string&, uint32_t)>;
public:
    struct SFile
    {
        std::filesystem::file_time_type time;
        std::string path;
        uint32_t crc;
    };
private:
    std::unordered_map<std::string, SFile> paths_;
    std::vector<std::thread> threads_;
    std::mutex mutex_;
    Callback callback_;
    bool running_ = false;

public:

    FileWatcher();

    void AddPath(const std::string& path, uint32_t crc);

    void SetCallback(const Callback& callback);

    void Start();

    void Stop();

private:
    void watch();
};
#endif