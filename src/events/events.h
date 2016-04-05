#ifndef __EVENTS
#define __EVENTS

#include <cassert>
#include <deque>
#include <list>
#include <map>

#include "type_index.h"
typedef type_index event_type;

/*
 * HACK: 
 */
template <typename T>
std::type_info const & template_typeid() 
{
    T temp;
    return typeid(temp);
}

/*
 * HACK: due to HACK above, all events must define
 * a default parameterless event. This is fine for now.
 */
class Event
{
public:
    virtual ~Event() {};
};

/*
 * see if we really need exec AND call
 */
class EventHookBase
{
public:
    virtual ~EventHookBase() {};
    void exec(Event const * event)
    {
        call(event);
    }

private:
    virtual void call(Event const *) = 0;
};

template<class T, class E>
class EventHook : public EventHookBase
{
public:
    typedef void (T::*Hook)(E const *);

    EventHook(T * instance, Hook hook)
    : _instance(instance)
    , _hook(hook)
    {}

    void call(Event const * event)
    {
        (_instance->*_hook)(static_cast<E const *>(event));
    }

private:
    T * _instance;
    Hook _hook;

    friend class EventManager;
};


class EventManager
{
public:
    virtual ~EventManager()
    {
        // TODO: delete all EventHooks
    }; 

    template <class T, class E>
    void addEventHook(T * instance, void (T::*member_function)(E *))
    {
        // can add order order or something,
        // but maybe that will require ...
        event_hook_list_map[template_typeid<E>()].push_back(new EventHook<T, E>(instance, member_function));
    }

    template <class T, class E>
    void removeEventHook(T * instance, void (T::*member_function)(E *))
    {
        HookListMap::iterator mit = event_hook_list_map.find(template_typeid<E>());
        if (mit == event_hook_list_map.end())
        {
            return;
        }

        HookList & hooks = mit->second;
        HookList::iterator lit = hooks.begin();
        while (lit != hooks.end())
        {
            EventHook<T, E> * hook = static_cast< EventHook<T, E> *> (*lit);
            if (hook->_instance == instance && hook->_hook == member_function)
            {
                hooks.erase(lit++);
            }
            else
            {
                ++lit;
            }
        }
    }

    template <class E>
    void removeEventHooks()
    {
        HookListMap::iterator mit = event_hook_list_map.find(template_typeid<E>());
        if (mit != event_hook_list_map.end())
        {
            event_hook_list_map.erase(mit);
        }
    }

    // template <class E>
    // void enqueueEvent(E const * event)
    // {
    //     assert(event);
    //     event_queue.push_back(event);
    // }

    // void pollEvents()
    // {
    //     while(event_queue.size())
    //     {
    //         Event const * event = event_queue.front();
    //         event_queue.pop_front();
    //         triggerEvent(event);
    //     }
    // }

    template <class E>
    void triggerEvent(E const * event)
    {
        HookListMap::iterator it = event_hook_list_map.find(typeid(*event));
        if (it == event_hook_list_map.end())
        {
            return;
        }

        HookList const & hooks = it->second;
        for (HookList::const_iterator it = hooks.begin(); it != hooks.end(); ++it)
        {
            EventHookBase * hook = *it;
            hook->exec(event);
        }
    }

private:
    

    /*
     * basic idea:
     * trigger event -> add event to event queue
     * poll events -> pop off and handle each item on the queue,
     *                by calling each function in the handlers map
     */
    typedef std::deque<Event const *> EventQueue;
    typedef std::list<EventHookBase *> HookList;
    typedef std::map<event_type, HookList > HookListMap;

    EventQueue event_queue;
    HookListMap event_hook_list_map;
};

#endif  // __EVENTS
