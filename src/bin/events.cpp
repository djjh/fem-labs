#include <iostream>

#include "events.h"

class ExampleEvent : public Event
{
public:
	ExampleEvent() : message("I'm an example event!") {};
	virtual ~ExampleEvent() {};
	std::string message;
};

class ExampleElement
{
public:
	void hook1(ExampleEvent const * event)
	{
		std::cout << "hook1: " << event->message << std::endl;
	}
	void hook2(ExampleEvent const * event)
	{
		std::cout << "hook2: " << event->message << std::endl;
	}
};

int main(void)
{
	ExampleElement example_element;

	EventManager em;
	em.addEventHook(&example_element, &ExampleElement::hook2);
	em.addEventHook(&example_element, &ExampleElement::hook1);

	ExampleEvent example_event;
	em.enqueueEvent(&example_event);
	em.enqueueEvent(&example_event);
	em.enqueueEvent(&example_event);
	
	em.pollEvents();

	return EXIT_SUCCESS;
}
