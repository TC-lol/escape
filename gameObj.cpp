#include <map>
#include <string>
#include <forward_list>
#include <iostream>

#include "gameObj.h"

std::map<int,screen*> scenes;

object::object(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,objID SID, bool visib, bool act)
	:x(nx), y(ny),action(eve), sID(SID), visible(visib), interactable(act){
	sprite=sprites[spriteID];
	w=nw<0?al_get_bitmap_width(sprite):nw;
	h=nh<0?al_get_bitmap_height(sprite):nh;
	std::cout<<sID<<std::endl;
}

int object::ret_ID(){return sID;}

void object::send_event(){handler.queue_event(&action);}

itemGatedObject::itemGatedObject(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,event altEve,int ItemID,objID SID,bool visib, bool act)
	:object(nx, ny, nw, nh, spriteID, eve,SID,visib, act),reqItemID(ItemID),altAction(altEve){printf("//////////////////////////////////////////");}

void itemGatedObject::send_event(){
	int num = handler.ret_flag_val(GEV_CUR_HELD_ITEM);
	printf("Expected: %d\nReceived: %d\n",reqItemID,num);
	if(num==reqItemID) 	handler.queue_event(&action);
	else 				handler.queue_event(&altAction);
}

flagGatedObject::flagGatedObject(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,event altEve, std::vector<int> fKey, std::vector<int> fVal, objID SID, bool visib, bool act)
	:object(nx, ny, nw, nh, spriteID, eve,SID,visib, act), flagKey(fKey), flagVal(fVal), altAction(altEve){;}

void flagGatedObject::send_event(){
	for(int i=0;i<flagKey.size();i++)
		if(handler.ret_flag_val((gEventFlags)flagKey[i])!=flagVal[i]) {
			handler.queue_event(&altAction);
			return;
		}
	handler.queue_event(&action);
}

screen* screen::currentScreen = NULL;
int screen::sceneW=0;
int screen::sceneH=0;


screen::screen(int ID, SN_ID spr)
:background(sprites[spr]){
	scenes[ID]=this;
}

screen::~screen(){
//	std::forward_list<object*>::iterator i;
	while(!interact.empty()){
		delete *interact.begin();
		interact.pop_front();
	}
//	for(std::forward_list<object*>::iterator i = interact.begin();i!=interact.end();i++) delete *i;
}

bool screen::delete_child_object(int childSID){
	std::cout<<childSID<<"\nDEL\n";
	if(childSID==OID_GENERIC)return false;
	std::cout<<"DELETE\n";
	for(std::forward_list<object*>::iterator i = interact.before_begin();std::next(i)!=interact.end() ;i++){
		std::cout<<(**std::next(i)).sID<< ' ' << *std::next(i) << std::endl;
		if(!*std::next(i)) continue;
		if((*std::next(i))->sID == childSID){
			
			delete *std::next(i);
			interact.erase_after(i);
			return true;
		}
	}
	return false;
}

void screen::find_object(int x, int y){
	for(auto i: interact){
		if(x>=i->x && x<=i->x+i->w && y>=i->y && y<=i->y+i->h){
			handler.queue_event(&i->action);
		}
	}
}

object* screen::find_object_with_ID(objID ID){
	if(!ID)return NULL;
	std::forward_list<object*>::iterator it = interact.before_begin();
	 
	do{it++;}while(it!=interact.end() && (*it)->sID!= ID);
	
	return it==interact.end() ? NULL : *it;
}
 
bool screen::change_object_event(object* objPtr, event ev){
	if(!objPtr)return false;
	objPtr->action = ev;  
	return true;
}

void add_object(int sceneID, int x,int y,int w,int h,SN_ID bitmapID,event_t eventID, std::vector<int> data,objID SID,bool act, bool visib){
	try{
	event NEW(eventID,data);
	object* temp; 
	
	temp = new object(x,y,w,h,bitmapID,NEW,SID,visib,act);
	
	scenes[sceneID]->interact.push_front(temp);
	}
	catch(...){
		
		std::cout<<"Does this thing even work???\n";
		printf("An error has occured during creation of a level.\nCheck if:\n - The sceneID is not equal to 0\n that is all.\n");
	}
}

void add_object(int sceneID, int x,int y,int w,int h,SN_ID bitmapID,event_t eventID, std::vector<int> data,event_t altEventID, std::vector<int> altData, int rqItem,objID SID,bool act, bool visib){
	try{
	event NEW(eventID,data);
	event els(altEventID,altData);
	object* temp; 
	
	temp = new itemGatedObject(x,y,w,h,bitmapID,NEW,els,rqItem,SID,visib,act);
	
	scenes[sceneID]->interact.push_front(temp);
	}
	catch(...){
		
		std::cout<<"Does this thing even work???\n";
		printf("An error has occured during creation of a level.\nCheck if:\n - The sceneID is not equal to 0\n that is all.\n");
	}
}

void add_object(int sceneID, int x,int y,int w,int h,SN_ID bitmapID,event_t eventID, std::vector<int> data,event_t altEventID, std::vector<int> altData, std::vector<int> flagKey, std::vector<int> flagVal	,objID SID,bool act, bool visib){
	try{
	event NEW(eventID,data);
	event els(altEventID,altData);
	object* temp; 
	
	temp = new flagGatedObject(x,y,w,h,bitmapID,NEW,els,flagKey,flagVal,SID,visib,act);
	
	scenes[sceneID]->interact.push_front(temp);
	}
	catch(...){
		
		std::cout<<"Does this thing even work???\n";
		printf("An error has occured during creation of a level.\nCheck if:\n - The sceneID is not equal to 0\n that is all.\n");
	}
}

screen* change_scene(int sc){
	if(scenes[sc]==NULL)return NULL;
	graphMNG.clear_draw_queue();
//	graphMNG.add_to_queue(std::make_pair(0,0), std::make_pair(-1,-1),scenes[sc]->background);
	
	
	for(const auto& i: scenes[sc]->interact) 
		if(i->visible)graphMNG.add_to_queue(std::make_pair(i->x,i->y),std::make_pair(i->w,i->h),i->sprite);
	graphMNG.add_to_queue(std::make_pair(0,0), std::make_pair(screen::sceneW,screen::sceneH),scenes[sc]->background);
	return scenes[sc];
}

void action(int x, int y, screen* scene){
//	printf("Mouse clicked!\n");
	if(scene == NULL || scene->interact.empty()) return;
//	std::cout<<"Scene is not empty!\nMouse position: " << x << ", " << y <<"\n";
	for(const auto& i: scene->interact){
//		printf("Item position: %d, %d\n%d, %d\n",i->x,i->y,i->x+i->w,i->y+i->h);
		if(i->interactable)if(x>=i->x && x<= i->x+i->w && y>= i->y && y<=i->y+i->h){
			i->send_event();//handler.queue_event(&i->action);
			return;
// Player ought to be able to only execute the event in the first object, otherwise
// weird things, like changing the event data of the wrong object, and locking the player out of 
// properly interacting with it. I should've thought of it way sooner than 16.10.25 21:31;
// 23.11.25 17:32 EDIT: I mightactually end up changing the entities to hold lists of events,
//		that will be fully executed upon being clicked. Might(read as: will) require a rework of the object class;
		}
	}
}

SC_ID add_scene(SC_ID ID, SN_ID spr){
	assert(scenes[ID]==0 && "Attempting to overwrite an existing scene!");
	screen * temp = new screen(ID,spr);
	return ID;
}

void destroy_all_scenes(){
	for(std::map<int,screen*>::iterator i = scenes.begin();i!=scenes.end();i++) delete i->second;
}

object* find_object_at_scene(int objID, screen* sc1, int sc2){
	std::forward_list<object*>::iterator it;
	if(sc1!=NULL) it=sc1->interact.before_begin();
	else
		if(sc2!=INT_MAX||sc2!=INT_MIN)it= scenes[sc2]->interact.before_begin();
		else return NULL;
	 
	do{it++;}while((*it)->ret_ID()!=objID);
	return *it;
}