table EventOne
{
id:uint;
timestamp:ulong;
adress:string;
}

table EventTwo
{
id:uint;
timestamp:ulong;
strength:float;
}

union Events {EventOne, EventTwo}

table EventHolder
{
theEvent:Events;
}

root_type EventHolder;

```
#include "myevents_generated.h"

using namespace MyEvents;

int main() {
  flatbuffers::FlatBufferBuilder builder;

  // Create an EventOne object.
  auto event_one_offset = CreateEventOne(builder, 1, 1234567890, builder.CreateString("123 Main St"));

  // Create an EventTwo object.
  auto event_two_offset = CreateEventTwo(builder, 2, 1234567891, 1.23f);

  // Create an Events union object that holds an EventOne object.
  auto events_offset = CreateEvents(builder, EventsUnion::EventsUnion_EventOne, event_one_offset.Union());

  // Create an EventHolder object that holds the Events union object.
  auto event_holder_offset = CreateEventHolder(builder, events_offset.Union());

  // Serialize the EventHolder object to a buffer.
  builder.Finish(event_holder_offset);

  // Deserialize the buffer back into an EventHolder object.
  auto event_holder_ptr = GetEventHolder(builder.GetBufferPointer());

  // Access the EventOne or EventTwo object stored in the Events union object.
  if (event_holder_ptr->theEvent_type() == EventsUnion::EventsUnion_EventOne) {
    auto event_one_ptr = static_cast<const EventOne*>(event_holder_ptr->theEvent());
    std::cout << "EventOne id: " << event_one_ptr->id() << std::endl;
    std::cout << "EventOne timestamp: " << event_one_ptr->timestamp() << std::endl;
    std::cout << "EventOne address: " << event_one_ptr->adress()->str() << std::endl;
  } else if (event_holder_ptr->theEvent_type() == EventsUnion::EventsUnion_EventTwo) {
    auto event_two_ptr = static_cast<const EventTwo*>(event_holder_ptr->theEvent());
    std::cout << "EventTwo id: " << event_two_ptr->id() << std::endl;
    std::cout << "EventTwo timestamp: " << event_two_ptr->timestamp() << std::endl;
    std::cout << "EventTwo strength: " << event_two_ptr->strength() << std::endl;
  }

  return 0;
}
```

Can I only send one event instead of 2 ?

```
#include "myevents_generated.h"

using namespace MyEvents;

int main() {
  flatbuffers::FlatBufferBuilder builder;

  // Create an EventOne object.
  auto event_one_offset = CreateEventOne(builder, 1, 1234567890, builder.CreateString("123 Main St"));

  // Create an Events union object that holds the EventOne object.
  auto events_offset = CreateEvents(builder, EventsUnion::EventsUnion_EventOne, event_one_offset.Union());

  // Create an EventHolder object that holds the Events union object.
  auto event_holder_offset = CreateEventHolder(builder, events_offset.Union());

  // Serialize the EventHolder object to a buffer.
  builder.Finish(event_holder_offset);

  // Deserialize the buffer back into an EventHolder object.
  auto event_holder_ptr = GetEventHolder(builder.GetBufferPointer());

  // Access the EventOne object stored in the Events union object.
  if (event_holder_ptr->theEvent_type() == EventsUnion::EventsUnion_EventOne) {
    auto event_one_ptr = static_cast<const EventOne*>(event_holder_ptr->theEvent());
    std::cout << "EventOne id: " << event_one_ptr->id() << std::endl;
    std::cout << "EventOne timestamp: " << event_one_ptr->timestamp() << std::endl;
    std::cout << "EventOne address: " << event_one_ptr->adress()->str() << std::endl;
  }

  return 0;
}
```
