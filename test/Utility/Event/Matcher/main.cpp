#include <iostream>
#include <string>

#include "EzNet/Utility/Event/EventMatcher.hpp"

using namespace std;
using namespace tab;

class EventA : public Event {
public:
    EventA(int v) : value_(v) {}

    constexpr static event_type_id_t GetEventTypeID() {
        return 1;
    }

    auto get() const {
        return value_;
    }

private:
    int value_;

};

class EventB : public Event {
public:
    EventB(string v) : value_(v) {}

    constexpr static event_type_id_t GetEventTypeID() {
        return 2;
    }

    auto get() const {
        return value_;
    }

private:
    string value_;

};

int main() {
    EventMatcher em;

    em.set<EventA>([](const EventA& e){
        cout << e.get() << endl;
    });
    em.set<EventB>([](EventB& e){
        cout << e.get() << endl;
    });

    EventA a(123);
    EventB b("abc");

    em.call(a);
    em.call(b);

    cout << "Current size: " << em.size() << endl;

    em.remove<EventB>();

    cout << "Current size: " << em.size() << endl;

    try {
        em.call(b);
    }
    catch (...) {
        cout << "OK" << endl;
    }
    return 0;
}