#ifndef __EVENT_CENTER_HPP__
#define __EVENT_CENTER_HPP__

#include <any>
#include <exception>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "Event.hpp"

namespace tab {

class EventMatcher {
public:
    EventMatcher() { }

    EventMatcher(EventMatcher&& em) : event_map_(std::move(em.event_map_)) { }

    EventMatcher(const EventMatcher& em) : event_map_(em.event_map_) { }

    ~EventMatcher() { }

    template <typename E>
    auto call(E& e) 
    -> std::enable_if_t<
        std::is_base_of<Event, std::decay_t<E>>::value, 
        EventMatcher&> {
        auto ite = event_map_.find(std::decay_t<E>::GetEventTypeID());
        if (ite == event_map_.end())
            throw std::runtime_error(
                "tab::EventMatcher::call(): "
                "The given event is unknown in this container.");
        std::any_cast<std::function<void(std::decay_t<E>&)>>(ite->second)(e);
        return *this;
    }

    template <typename Arg>
    EventMatcher& set(std::function<void(Arg)> f) {
        static_assert(std::is_base_of<Event, std::decay_t<Arg>>::value,
            "The event handler function must accept an lvalue ref of "
            "an object of a class derived from 'tab::Event'. "
            "(The 'Arg' here is not derived from 'tab::Event'.)");
        static_assert(std::is_same<Arg, std::decay_t<Arg>&>::value, 
            "The event handler function must accept an lvalue ref of "
            "an object of a class derived from 'tab::Event'. "
            "(The 'Arg' here is not lvalue ref.)");
        auto ite = event_map_.find(std::decay_t<Arg>::GetEventTypeID());
        if (ite == event_map_.end())
            event_map_.insert({std::decay_t<Arg>::GetEventTypeID(), f});
        else
            ite->second = f;
        return *this;
    }

    template <typename Arg, typename Func>
    EventMatcher& set(Func callable) {
        static_assert(std::is_base_of<Event, std::decay_t<Arg>>::value,
            "The event handler function must accept an lvalue ref of "
            "an object of a class derived from 'tab::Event'. "
            "(The 'Arg' here is not derived from 'tab::Event'.)");
        std::function<void(std::decay_t<Arg>&)> f(callable);
        return set(f);
    }

    template <typename E>
    EventMatcher& remove() {
        static_assert(std::is_base_of<Event, std::decay_t<E>>::value, 
            "The given type is not derived from 'tab::Event'.");
        auto ite = event_map_.find(std::decay_t<E>::GetEventTypeID());
        if (ite == event_map_.end())
            throw std::runtime_error(
                "tab::EventMatcher::remove(): "
                "The given event is unknown in this container.");
        else
            event_map_.erase(ite);
        return *this;
    }

    template <typename E>
    bool has() {
        static_assert(std::is_base_of<Event, std::decay_t<E>>::value, 
            "The given type is not derived from 'tab::Event'.");
        auto ite = event_map_.find(std::decay_t<E>::GetEventTypeID());
        if (ite == event_map_.end())
            return false;
        else
            return true;
    }

    size_t size() const {
        return event_map_.size();
    }

protected:
    std::unordered_map<Event::event_type_id_t, std::any> event_map_;

}; // class EventMatcher

} // namespace tab

#endif // __EVENT_CENTER_HPP__