#pragma once

#include "../../common/singleton.h"

#include <iostream>
#include <chrono>
#include <functional>
#include <list>
#include <thread>
#include <vector>

class CEventTimer {
public:
    CEventTimer() : m_running(false), m_repeatCount(1), m_interval(std::chrono::seconds(1)), m_callback(nullptr) {}

    void SetRepeatCount(int repeatCount)
    {
        m_repeatCount = repeatCount;
    }

    void SetEndlessTimer()
    {
        SetRepeatCount(-1);
    }

    void Start(const std::chrono::steady_clock::duration& interval, const std::function<void()>& callback) {
        if (m_running)
            return;

        m_running = true;
        m_interval = interval;
        m_callback = callback;

        m_startTime = std::chrono::steady_clock::now();
    }

    void Stop() {
        m_running = false;
    }

    bool Update() {
        if (!m_running)
            return false;

        const auto currentTime = std::chrono::steady_clock::now();
        if (currentTime - m_startTime >= m_interval) {
            if (m_callback)
                m_callback();

            m_startTime = currentTime;
            if (m_repeatCount != -1)
                m_repeatCount--;

            if (m_repeatCount == 0)
                m_running = false;

            return true;
        }

        return false;
    }

    bool IsRunning() const { return m_running; }

protected:
    bool m_running;
    int m_repeatCount;
    std::chrono::steady_clock::duration m_interval;
    std::function<void()> m_callback;
    std::chrono::steady_clock::time_point m_startTime;
};

class CEventTimerManager
{
private:
    std::list<std::shared_ptr<CEventTimer>> m_timers;
public:
    template <class T>
    std::shared_ptr<CEventTimer> CreateTimer(const std::chrono::steady_clock::duration& interval, T f)
    {
        const auto timer = std::make_shared<CEventTimer>();
        timer->Start(interval, f);
        m_timers.push_back(timer);

        return timer;
    }

    template <class T>
    std::shared_ptr<CEventTimer> CreateEndlessTimer(const std::chrono::steady_clock::duration& interval, T f)
    {
        const auto timer = std::make_shared<CEventTimer>();
        timer->Start(interval, f);
        timer->SetEndlessTimer();
        m_timers.push_back(timer);

        return timer;
    }

    void Update()
    {
        std::list<std::shared_ptr<CEventTimer>> elapsedTimers{};
        for (auto& t : m_timers)
        {
            if (!t)
            {
                elapsedTimers.push_back(t);
                continue;
            }

        	if (t->IsRunning())
        		t->Update();
        	else
        		elapsedTimers.push_back(t);

        }


        for (auto& t : elapsedTimers)
            m_timers.remove(t);
    }
};