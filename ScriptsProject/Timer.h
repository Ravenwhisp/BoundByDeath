#pragma once
#include <functional>

class Timer
{
public:
    Timer() = default;

    void start(float duration);
    void update(float dt);
    void reduce(float amount);
    void stop();

    bool isReady() const;
    bool isActive() const;
    float getRemaining() const;

    void onCompleted(std::function<void()> callback);

private:
    float m_remaining = 0.0f;
    std::function<void()> m_onCompleted;
};
