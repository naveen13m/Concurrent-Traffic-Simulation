#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


// FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there should be an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T &&msg);


private:
    std::deque<T> _queue;
    std::condition_variable _cv;
    std::mutex _mtx;
    
};

enum TrafficLightPhase {
    red = 0,
    green
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    TrafficLightPhase getCurrentPhase();
    void simulate();

private:
    [[noreturn]] void cycleThroughPhases();

    std::shared_ptr<MessageQueue<TrafficLightPhase>> _queue;

    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
};

#endif
