#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lg(_mtx); // lock guard does not work
    _cv.wait(lg, [this] { return !_queue.empty();});
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lg(_mtx);
    _queue.emplace_back(std::move(msg));
    _cv.notify_one();
//    lg.unlock();
}

/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    _queue = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

TrafficLight::~TrafficLight()
{

}

void TrafficLight::waitForGreen()
{
    while (true) {
        if (TrafficLightPhase::green == _queue->receive()) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    std::thread temp(&TrafficLight::cycleThroughPhases, this);
    threads.emplace_back(std::move(temp));
}

[[noreturn]] void TrafficLight::cycleThroughPhases()
{
    const float max_cycle_duration = 6.0;
    const float min_cycle_duration = 4.0;
    const float duration_range = max_cycle_duration - min_cycle_duration;
    auto prev_time = std::chrono::system_clock::now();
    std::chrono::duration<double> diff_time = static_cast <std::chrono::duration<double>> (0.0);
    auto curr_time = std::chrono::system_clock::now();

    while (true) {
        float curr_cycle_duration;
        curr_cycle_duration = min_cycle_duration +
                static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/duration_range));
//        curr_cycle_duration = 4.0;

        while (diff_time.count() < curr_cycle_duration) {
            curr_time = std::chrono::system_clock::now();
            diff_time = curr_time - prev_time;
            std::this_thread::sleep_for(std::chrono::milliseconds (1));
        }

        // fill the message queue
        // Start a thread with async
        _currentPhase = TrafficLightPhase::green == _currentPhase ? TrafficLightPhase::red : TrafficLightPhase::green;
        auto msg = _currentPhase;
        auto ftr = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _queue, std::move(msg));
        ftr.wait();
        prev_time = curr_time;
        diff_time = curr_time - prev_time;
    }
}
