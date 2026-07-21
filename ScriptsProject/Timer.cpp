#include "pch.h"
#include "Timer.h"

void Timer::start(float duration)
{
    m_remaining = (duration > 0.0f) ? duration : 0.0f;
}

void Timer::update(float dt)
{
    if (m_remaining <= 0.0f)
    {
        return;
    }

    m_remaining -= dt;

    if (m_remaining <= 0.0f)
    {
        m_remaining = 0.0f;
        if (m_onCompleted)
        {
            m_onCompleted();
        }
    }
}

void Timer::reduce(float amount)
{
    if (m_remaining <= 0.0f || amount <= 0.0f)
    {
        return;
    }

    m_remaining -= amount;

    if (m_remaining <= 0.0f)
    {
        m_remaining = 0.0f;
        if (m_onCompleted)
        {
            m_onCompleted();
        }
    }
}

void Timer::stop()
{
    m_remaining = 0.0f;
}

bool Timer::isReady() const
{
    return m_remaining <= 0.0f;
}

bool Timer::isActive() const
{
    return m_remaining > 0.0f;
}

float Timer::getRemaining() const
{
    return m_remaining;
}

void Timer::onCompleted(std::function<void()> callback)
{
    m_onCompleted = std::move(callback);
}
