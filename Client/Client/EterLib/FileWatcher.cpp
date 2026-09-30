#include "StdAfx.h"
#ifdef USE_HOT_RELOAD
#include "FileWatcher.h"

static FileWatcher s_watcher;

void FileWatcher::AddPath(const std::string& path, uint32_t crc)
{
	std::lock_guard<std::mutex> lock(mutex_);
	paths_[path] = SFile{ std::filesystem::last_write_time(path), path, crc };
}

void FileWatcher::SetCallback(const Callback& callback)
{
	callback_ = callback;
}

void FileWatcher::Start()
{
	running_ = true;
	threads_.emplace_back([this] { this->watch(); });
}

void FileWatcher::Stop()
{
	running_ = false;
	for (auto& thread : threads_) {
		if (thread.joinable()) {
			thread.join();
		}
	}
}

FileWatcher::FileWatcher(): paths_{}, threads_{}, callback_{}, running_{false}
{}

void FileWatcher::watch()
{
	while (running_) {
		std::this_thread::sleep_for(std::chrono::seconds(1)); // Check interval

		std::lock_guard<std::mutex> lock(mutex_);

		//std::erase_if(paths_, [](const auto& path)
		//{
		//	return !std::filesystem::exists(path.first);
		//});

		for (auto& [path, last_write_time] : paths_) {
			if (std::filesystem::exists(path)) {
				auto current_time = std::filesystem::last_write_time(path);
				if (current_time != last_write_time.time) {
					last_write_time.time = current_time;
					callback_(path, last_write_time.crc);
				}
			}
			else
			{
				last_write_time.time = {};
			}
		}
	}
}

#endif