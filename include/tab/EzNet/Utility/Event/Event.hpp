#ifndef __EVENT_HPP__
#define __EVENT_HPP__

namespace tab {

class Event {
public:
    typedef size_t event_type_id_t;

    constexpr static event_type_id_t GetEventTypeID() {
        return 0;
    }
    
public:
    Event() { }
    
protected:
    friend class EventMatcher;

}; // class Event

} // namespace tab

#endif // __EVENT_HPP__