#ifndef GAMEOBJ_H
#define GAMEOBJ_H
 
#include <map>
#include <string>
#include <forward_list>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "eventQueue.h"
#include "graphics.h"

#include "globalEnums.h"

typedef std::vector<std::vector<int>> dataSet;
typedef std::vector<std::pair<std::vector<int>,int>> eventDataPairSet;

class screen;

class object;

extern std::map<int,screen*> scenes; //<scene id, scene>

void next_text_box_prompt(data t);

object* find_object_at_scene(screen* sc1, int sc2, int objID);
 
class object{ //base object
friend class screen;
friend screen* change_scene(int);
friend void action(int,int,screen*); 

protected:
	int x;
	int y;
	int w;
	int h;
	objID sID;
	ALLEGRO_BITMAP* sprite;
	event action;
	virtual void send_event();
public:
	bool visible;
	bool interactable;
	
	object(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,objID SID=OID_GENERIC, bool visib=true, bool act=true);

	int ret_ID();
}; 

//30.11.25 23:11 EDIT: The class above will turn into a new derived class from yet-to-be-finalised baseObject class which will be the one below
//30.11.25 23:13 EDIT: The idea above me has officially been scrapped. The object class will simply be modified to also contain eventDataPairSet, it will
//		it will simply be used to execute multiple events at once, which will(read as: maybe-probably-hopefully-whoknows) save me time on creating custom
//		events. In case of a itemGatedObject, using an item will activate an event paired with it. It does not allow for executing multiple events. however
//		itemGatedObjects will have no need for them. We shall see. Obviously, it will require modifying the base class and all of its methods, but it is a
//		sacrifice I am willing too make.
//25.02.26 09:07 EDIT: I completly forgot about it, LOL, LMAO even. Forgive me for the previous digression, I do not know whether I should pursue this matter
//		further, as it would lead to a whole load of things I, frankly, do not widh to be involved with



class itemGatedObject : public object{ //An object that requires 
private:
	event altAction;
	int reqItemID;
	virtual void send_event();
public:
	itemGatedObject(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,event altEve,int ItemID,objID SID=OID_GENERIC, bool visib=true, bool act=true);
};

class flagGatedObject : public object{
private:
	std::vector<int> flagKey;
	std::vector<int> flagVal;
	event altAction;
	
	virtual void send_event();
public:
	
	flagGatedObject(int nx, int ny, int nw, int nh,SN_ID spriteID,event eve,event altEve, std::vector<int> fKey, std::vector<int> fVal, objID SID=OID_GENERIC, bool visib=true, bool act=true);
};

//void destroy_all_scenes();

class screen{
	   
friend void action(int, int, screen*);
friend void add_object(int, int,int,int,int,SN_ID,event_t,std::vector<int>,event_t,std::vector<int>,int,objID,bool,bool);
friend void add_object(int, int,int,int,int,SN_ID,event_t,std::vector<int>,event_t,std::vector<int>,std::vector<int>,std::vector<int>,objID,bool,bool);
friend void add_object(int, int,int,int,int,SN_ID,event_t,std::vector<int>,objID,bool,bool);
friend void next_text_box_prompt(data);
friend screen* change_scene(int);
friend void destroy_all_scenes();
friend object* find_object_at_scene(int,screen*,int);
friend int main();

	ALLEGRO_BITMAP* background;
	
	std::forward_list<object*> interact;
	void find_object(int x, int y);
	
	static int sceneW;
	static int sceneH;
public:
	~screen();
	screen(int ID,SN_ID spr);
	
	static screen* currentScreen;
	
	object* find_object_with_ID(objID ID);
	bool delete_child_object(int childSID);
	bool change_object_event(object* objPtr, event ev);

};

screen *change_scene(int sc=0);
/*
	Empties the drawing queue and fills it with elements associated with the given screen
	Also, returns a pointer to the, now current screen.
	23.10.25 16:01 : As of now, it reloads the current scene if sc = 0, or no parameter is provided.
	Surely it will not cause any problems down the line.
*/
void action(int x, int y, screen* scene);
/*
	Iterates over the objects associated with a given scene, finding the first object
	overlapping with the given position and sending its event ID with a data package
	to the event handler to be executed
*/
void add_object(int sceneID						, int x						, int y			, int w				,
				int h							, SN_ID bitmapID			, event_t eventID	,
				std::vector<int> data			, objID SID=OID_GENERIC		, bool act=true	, bool visib=true);
//				event_t altEventID=EN_PROP_EVENT,std::vector<int> altData={}, std::string rqItem="");
				
void add_object(int sceneID				, int x						, int y			, int w				,
				int h					, SN_ID bitmapID			, event_t eventID	,
				std::vector<int> data			,
				event_t altEventID		, std::vector<int>			, int rqItem,
				objID SID=OID_GENERIC	, bool act=true			, bool visib=true);

void add_object(int sceneID				, int x						, int y			, int w	,
				int h					, SN_ID bitmapID			, event_t eventID		,
				std::vector<int> data	, event_t altEventID		,std::vector<int>altData,
				std::vector<int> flagKey, std::vector<int> flagVal	,
				objID SID=OID_GENERIC	, bool act=true				, bool visib=true		);

/*
	Adds an object to a scene with a specified ID, with origin point at designated position,
	with specified dimensions, a specified sprite, event ID and a data package to be sent with it
*/
SC_ID add_scene(SC_ID ID, SN_ID spr);
/*
	Creates a scene at a given position in a vaector, with a given sprite name as backdrop
	TO ADD: return false when trying to write over an existing scene.
*/

object* find_object_at_scene(int objID, screen* sc1, int sc2=0);

void destroy_all_scenes();
#endif
