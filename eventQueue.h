#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <vector> 
#include <map>
#include <utility>
#include <string>
#include <forward_list>
  
#include "globalEnums.h"

#

typedef std::vector<int> data;
typedef std::pair<event_t, data> event;

event  make_event(event_t name, data t);
 
class eventHandler{
	std::map<event_t, void (*)(data*)> actions;
	std::map<gEventFlags, short> eventFlags;
	
	std::forward_list<event*> queue;
	std::forward_list<event*> queueREAD;
public:
	short ret_flag_val(gEventFlags key);
	short set_flag_val(gEventFlags key, short nVal);
	void reset_all_flags();
	
	
	void add_event(event_t ID, void (*act)(data*) );
	void queue_event(event* q);
	void execute_events();
};

extern eventHandler handler;

//#include "eventQueue.cpp"

#endif
