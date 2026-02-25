#include <iostream>
#include <map>
#include <utility>
#include <vector> 
#include <string>

#include "eventQueue.h" 

event make_event(event_t name, data t){return std::make_pair(name,t);}

short eventHandler::ret_flag_val(gEventFlags key){return eventFlags[key];}

short eventHandler::set_flag_val(gEventFlags key, short nVal){return eventFlags[key]=nVal;}

void eventHandler::add_event(event_t ID, void (*act)(data*)){
	actions[ID] = act;
}
  
void eventHandler::queue_event(event* q){
	std::cout<<"?\n";
	queue.push_front(q);
}


void eventHandler::execute_events(){
	bool eventDet = !queue.empty() ? true : false;

	queueREAD.clear();
	queueREAD.swap(queue);
	std::forward_list<event*>::iterator i;
	while(!queueREAD.empty()){
		i = queueREAD.begin();
		if(actions[(*i)->first]!=NULL)actions[(*i)->first](&(*i)->second);
		queueREAD.pop_front();
	}
	if (eventDet) std::cout<<"Events were executed!\n";
}
/*
 *Accesses the first element of the list, executes the correct event
 *deletes the current first element and repeats until the queue is
 *empty.
 *EDIT 14.10.2025 : Adding a second list allows for adding event to the
 *		future queue without creating an infinite loop. Might be useful for
 *		events that happen after a while(incrementing one of the args with
 *		each execution, until it reaches a specified number)
*/

void eventHandler::reset_all_flags(){
	for(gEventFlags i=GEV_PROP_FLAG_FIRST;i!=GEV_PROP_FLAG_LAST;i=gEventFlags(i+1))eventFlags[i]=0;
}

eventHandler handler;

