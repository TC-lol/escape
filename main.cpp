#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <cmath>
#include <map>
#include <string>
#include <fstream>
#include <string>
#include <thread>
 
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "eventQueue.h"
#include "graphics.h"
#include "gameObj.h"
#include "UI.h"

#include "globalEnums.h"

//#include "gameData.h"

//main is whwere all the event-triggered functions are stored
//13.11.25 10:46 EDIT: ALL of the functions that used to be in the main file have been
//		moved to corresponding gameData files. Surprisingly, nothing broke. Might need to investigate.
//13.11.25 10:49 EDIT: Surprisingly, there indeed is nothing broken. As a side note, from this
//		point onwards, comments will be using this style, as /**/ can be annoying. 
//20.11.25 20:56 EDIT: I have reversed the previous decision about moving the functions to a different file.
//		Instead, they have been moved below the main function, and their declarations have remained above.
//27.11.25 18:14 EDIT: This very comment section has officially been converted into the "Dev's logs" section
//		On a more important note, I have converted the arguments of the event functions to pointers instead of objects.
//		Who could have possibly thought that copying entire vectors, most of them multiple items long would be far less
//		efficient than just passing a pointer to them? Anyways, strings in bitmaps' ID's, you are next.
//03.12.25 18:41 EDIT: I am either a genius, or the biggest fool to ever walk this earth. I have turned the file containing the 
//		sprite names into a ".h" file, and made the spritenames into an enum. That file is now responsble for correctly importing the right
//		bitmaps, and giving each one an ID, that is not a string. It will take a while to get the code in place though.
//03.12.25 18:51 EDIT: It does break something - the piece of code responsible for generating buttons with icons. Welp.
//03.12.25 19:09 EDIT: The issue has been resolved in full, alongside all of the other problems that arose with this new solution
//		(Receiving the wrong data type, friended classes and functions falling out due to changed definitions, etc. etc.)
//		Despite all of that, I still think the change was worth it. The less strings need to be stored, the better.
//04.12.25 18:40 EDIT: I have, after all of the implications my previous discovery had made clear have settled, I have moved each and every
//		enumeration into the newly created "globalEnums.h" file - making them accessible from any file. I had also decided to turn some of
//		the ints that were supposed to take in values of the respective enumerations into enum classes, which, oh dear, was a bit of a hassle
//		for me considering that enum classes take in only values from their respective enums, and not any filthy, common integers.
//		That aside, I have also implemented into the eventHandler class a map of variables, that event functions will be able to read from
//		and write to, in order to have some form of comunication between each other (now, when I finally get to making a puzzle, I will not have
//		to make yet another derrived class of game object, which is a big upside).
//11.12.25 13:29 LOG: good news - threaded draw method is working correctly, has reduced the CPU usage down from ~15-20% to ~1%!.
//		BAD NEWS - the game irreversibly freezes itself, not accepting any inputs from the user, and requires usage of task manager in order to be closed,
//		if the game is set to run at FPS higher than ~70. Welp. Could be better, but it also could heve been worse. I will try to further optimise it.
//18.12.25 12:36 LOG: spilting the operations of drawing UI and the game assets into two separate methods that execute asynchronously was a great idea,
//		allowing the game to run at 96 fps without freezing.
//18.12.25 12:50 LOG: The freezing issue that used to happen at fps above 96 has been resolved - obviously, now at high fps the game just skips frames,
//		but that can be resolved by further optimisation;
//18.12.25 13:05 LOG: Nevermind! The game is still prone to freezing once the fps starts nearing 180.
//08.01.26 10:45 LOG: Disregard the previous nevermind! As it turns out, I must've accidentally pressed one of the UI buttons that force-freezes the program
//		until the user interacts with the console window. The game itself does not freeze at all, it simply is unable to keep up wth the framerate when it is set
//		to a value high enough, effectiely hardcapping the FPS. Also, thanks to multithreading and other optimisations, it does not use up as many resources as
//		it used to - the usage of the graphical processor is still quite abnormal. I might need to ask about it online.
//29.01.26 17:04 LOG: Now that I am looking at the whole of what I've managed to implement, the code is "basically"*(There is still a heck ton of ptimisation left
//		to do) done, what is left is implementing all of the different functions to make a coherent game... which includes manually setting everything up,... and
//		making all of the sprites in gimp,... and dealing with all of the edgecases that may arise during development,... I'm starting to consider the idea of outsourcing
//		this part of development to one of my siblings, despite the fact that none of them can code. On a side note, not all of the sideeffects of changing how pointers
//		to bitmaps were stored, the last piece of code affected by it, namely the one responsible for drawing buttons with items, has finally been fixed, which also
//		resulted in making it so, that buttons can now, simultaneously display both an image and a piece of text. 
std::map<int,const char*> invItems={
										{OID_PLACEHOLDER,"placeHolder"},
										{OID_SCREWDRIVER,"screwdriver"}
										};
//Apparently, due to, lord knows what circumstances, when trying to store a string in a map, it does not work. const *char has to be utilised//
//EDIT: Apparently, printf() simply is not compatibile with std::string, and the Error was caused be the function returning NULL...

const bool testBranch=true;
//bool PLAYING = true;
//04.12.25 18:54 : Deprecated - functionality has ben passes to handler's eventFlags
//screen* currentScr = NULL;
//23.10.25 16:07 : Deprecated - I have remembered about the existence of "static".

//int selectedItem=-1;
//04.12.25 18:51 : Deprecated - functionality has been moved to eventFlags
//text* theTextBox =NULL;
//04.12.25 19:11 : Deprecated - the knowledge of "static" has struck once again

std::map<int,std::string> textContainer={
		
		{INT_MIN,"(...)"},
		{-1,"...Where...where am I...? What is this place...?"},
		{-2,"Test 2"},
		{-3,"tesT #3"},
		
		{-4,"Well, that is a rather imposing and... ominous... door. I really hope there is a different way out of here."},
		{-5,"I'd rather really not have to open this door. Actually, where even are it's hinges? It's' just... floating... menacingly..."},
		{-6,"... ... phew, it's locked."},
		
		{-1000-OID_PLACEHOLDER,"(I got... something... Actually, what the heck is this?)"},
		{-1000-OID_SCREWDRIVER,"(An old screwdriver - worn, but could prove useful.)"},
		
		{50*B_ROOM_L+0,"(I turned to the left side of the room...)"},
		
		{50*B_ROOM_F+0,"(I turned to the front of the room...)"},
		{50*B_ROOM_F+1,"(I stepped away from the crooked table...)"},
		{50*B_ROOM_F+2,"(I went back to the centre of the room...)"},
		
		{50*B_ROOM_R+0,"(I turned to the right side of the room...)"},
		{50*B_ROOM_B+0,"(I turned to the back of the room...)"},
		{50*B_ROOM_TABLE+0,"(I approached the weird table...)"},
		
//		{,""},
//		{50*,""},
//		{50*,""},
		};



bool set_text_box_pointer();

//For clarity's sake, write the object placing code right after the scene it adds objects to.
void build_levels();

void build_UI();

void user_input(const ALLEGRO_EVENT& ev);

void example_event(data*t);

void scene_change(data*t);

void destroy_object_at_scene(data*t);
/*
 *	Destroys object with special ID of t->at(1) located
 *	in t->at(0) scene's list of child objects and removes it
 */

void destroy_object_at_this_scene(data*t);

void destroy_all_object_instances(data*t);

void add_item_to_inv(data*t);
/*
	Adds the object with special ID of t->at(1) located 
	in t->at(0) scene's list of child objects to the
	invventory, and subsequently destoys it by
	calling destroy_object(). Afterwards, it reloads
	the scene by calling change_scene().
	REQ:
	 - t must be of size at least 3
	 - t->at(2) must be equal //EDIT: equal to what? I'll never know, because I did not finish writing that sesntence
	//24.02.26 11:20 EDIT: I know why I've never finished writing that comment - this entire event is extremely scuffed, due to how ret_child of screen class works
	//		A re-write, or a major  alteration is in order. Also, t->at(2) must be equal to 1, or we have a major problem;

*/

void change_selected_item(data*t);
/*
void next_text_box_prompt(data*t)
{	
	theTextBox->change_text(&textContainer[t->at(0)]);
	if(t->at(0)==t->at(1)){
		screen::currentScreen->delete_child_object(OID_TEXT_PROMPT);
		return;
	}	
	if(object* it = screen::currentScreen->find_object_with_ID(OID_TEXT_PROMPT)) 
		screen::currentScreen->change_object_event(it,make_event(EN_NEXT_TEXT_BOX_PROMPT,{t->at(0)-1,t->at(1)}));
}
*/
/*	22.10.25 18:22
 *	This event actually sucks, to be honest.
 *	It easily screws itself over by:
 *	- changing the event of the first object in the current screen without checking the objects type;
 *	- once the dialog advences enough it removes the first object on the screen, AGAIN without checking the type;
 *	- is difficult to expand;
 *	I have no idea what I was thinking while writing this. To be remade.
 *	22.10.25 19:17: Point 1 resolved
 *	22.10.25 19:37: Point 2 is actually false - it does check, or rather removes the object with id of: OID_TEXT_PROMPT,
 *	as to point 3, a new event will do; This specific event has been relegated to handling only the OID_TEXT_PROMPT objects;
 *	23.10.25 17:02: This function has (fianlly) been deprecated, its responsibilities shifted to change_text_prompt.
 */

void create_text_box_prompt(data*t);
/*
 *	INFO (for the 2 above functions):
 *		- t->at(0) stores the starting and later the current
 *			position of the text in the map;
 *		- t->at(1) stores the final text position within
 *			the map;
 *		- Reaching t->at(1) changes the text and removes
 *			the object holding this event;
 *		- t->at(2) contains the scene the prompt is set to appear;
 *		- t->at(3) is not important now;
 *		- t->at(4) scene the object is supposed to appear in;
 */
 
void show_text_prompt_of_object(data*t);
 
void change_text_prompt(data*t);
/*INFO for 2 functions above:
 *	- t->at(0) - the position of within textContainer with text to display;
 *	- t->at(1) - target(last) position within textContainer that is to be displayed;
 *	- t->at(2) - sID of the object;
 *	- t->at(3) - action to be taken upon reaching the target
 *	- t[3+n (n>=1)] - parameters for the action, depending on its type.
 *
 *		23.10.25 10:53: I an actually contemplating deprecating the next_text_box_pompt
 *	In favour of this function - It does everything the old one is doing, but better.
 */

void start_game(data*t);

//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////  GREAT DIVIDER - FUNCTIONS BELOW MAKE USE OF THE EVENT FLAG SYSTEM  //////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void alter_flag(data*t);
/*INFO: A generic function meant to be used as a part of other event to easily alter values of event flags
 *REQ: Ever vector read by this function must have at least three members.
 *	- t->at(0) - The flag meant to be altered;
 *	- t->at(1) - The mode used to alter the event flag, numbers corresponding to:
 *			0 - set / 1 - addition / 2 - subtraction / 3 - multiplication / 4 - division / everything else - cancel the operation;
 *	- t->at(2) - number used by the operation.
 */
 
void fuse_turn(data*t);

void register_events();

void game_loop(){;}
//07.12.25 22:05 NOTICE: This will be an experimental function,
//		Meant to test whether it is possible to reduce cpu usage of the project, via
//07.12.25 22:07 EDIT: This is DUMB
//07.12.25 22:09 EDIT: After additional tests (2 to bee exact), it turns out that the
//		drawing process is the one that takes the most energy. Now, I will either have to revert the
//		draw method from a thread to a regular function, or dabble in some mutexes - which would be fun, 
//		if not for how screwed my schedule is.


int main() {
	

#ifdef _WIN32
	printf("This is like some king of Macaroonsoft OS...\n");
#else
	printf("This is some different #@$& we are running on! ABORT!! ABORT!!!\n");
	return 2137;
//	return 11308;
#endif

//	std::cout << __LINE__ << std::endl << __DATE__ << std::endl << __TIME__ <<std::endl;
	
	std::cout << sprites[TEST];
	
	std::cout <<"This is a test to check whether my idea has worked " << baseScreen<<'\n';
	
	if( !al_init() ) {
		std::cout << "Failed to initialize allegro!\n";
		return - 1;
	}
	if(!al_install_mouse()) {
		std::cout << "Failed to install mouse!/n";
		return -1;
	}


	al_init_primitives_addon();
	al_init_font_addon();
	al_init_image_addon();
	
	ALLEGRO_DISPLAY *scr =NULL;
	int displayW = 1200, displayH = 700;
	screen::sceneW=1000;screen::sceneH=700;
/*	The displays parameters are :
 *	Height: the full height of a scene
 *	Width: The width of a Scene(1000px) plus space for an equipment bar
 *	Moreover,(usually) 140px on either side of the Scene are used to navigate around the Room.
 *	EDIT: Last point has been scrapped in fovour of navigation arrows.
*/
//*
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES,4, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SUPPORTED_ORIENTATIONS,ALLEGRO_DISPLAY_ORIENTATION_ALL, ALLEGRO_SUGGEST);
//*/
	scr= al_create_display(displayW,displayH);
	graphMNG.set_target_disp(scr);

	load_graphics();
	
	std::cout << sprites[TEST];
	
	ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue(),
	*frames = al_create_event_queue();

	ALLEGRO_EVENT ev;
	ALLEGRO_EVENT click, bin;

	ALLEGRO_MOUSE_STATE mouse;
	
	ALLEGRO_TIMER *fps = al_create_timer(1.0/128.0);

	

	std::cout <<"\ntest\n";

	al_register_event_source(queue,al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(scr));
	al_register_event_source(frames,al_get_timer_event_source(fps));

	printf("test?\n");
	register_events();
	printf("test\n");
	
	build_levels();
	screen::currentScreen = change_scene(-1);
	
	printf("test\n");
	build_UI();
	
	printf("test\n");
	al_start_timer(fps);
	
	printf("test\n");
	if(!set_text_box_pointer())return -1;
	
	while(!handler.ret_flag_val(GEV_GAME_RUNNING)){
		
		graphMNG.begin_draw_loop();

		al_wait_for_event(frames,&bin);

		while(al_get_next_event(queue,&ev))user_input(ev);

		graphMNG.await_draw_finish();

		handler.execute_events();
	}
//	std::system("pause");
	destroy_all_scenes();
//	std::system("pause");
	destroy_all_UI();
//	std::system("pause");
	return 0;
}

/////////////////////////////////////////////////////
//////////Function definitions///////////////////////
/////////////////////////////////////////////////////

bool set_text_box_pointer(){
	text::theTextBox = static_cast<text*>(savedPanels[INV]->return_child({0,WID_TEXT_BOX,2}));
	return text::theTextBox != NULL ? 1:0;
}

void build_levels(){
 
	 
	SC_ID i;
	i =add_scene(B_ROOM_L,baseScreen); 
	
	add_object(i,0,500,100,100,arrowRed_left,EN_CHANGE_SCENE,{B_ROOM_B});
	add_object(i,900,500,100,100,arrowRed_right,EN_CHANGE_SCENE,{B_ROOM_F});
	add_object(i,20,20,50,50,TEST,EN_TEST,{0});
	//
	i=add_scene(B_ROOM_F,Scene1);
	
	add_object(i,0,500,100,100,arrowRed_left,EN_CHANGE_SCENE,{B_ROOM_L});
	add_object(i,900,500,100,100,arrowRed_right,EN_CHANGE_SCENE,{B_ROOM_R});	
//	add_object(i,680,400,40,40,placeHolder,EN_PROP_EVENT,{},OID_SCREWDRIVER);
	add_object(i,650,400,175,200,table,EN_CHANGE_SCENE,{B_ROOM_TABLE});
//	add_object(i,680,430,40,40,screwdriver,EN_PROP_EVENT,{},OID_SCREWDRIVER,false);
	add_object(i,390,185,205,355,door_closed,EN_CHANGE_SCENE,{B_ROOM_DOOR});
	add_object(i,640,140,200,200,electrical_box,EN_CHANGE_SCENE,{B_ROOM_ELEC_BOX});
	add_object(i,655,155,170,170,electrical_box_lid,EN_PROP_EVENT,{},OID_ELEC_LID,0);
	//test temporary
	add_object(i,60,60,50,50,TEST,EN_CHANGE_TEXT_PROMPT,{-1,-3,OID_TEST_SUBJECT,0,B_ROOM_F},OID_TEST_SUBJECT);
	//
// *Stress testing the drawing engine
//	for(int i=1; i<100;i++)for(int j=1;j<10;j++) add_object(B_ROOM_F,100+i,100+j,40,40,placeHolder,EN_PROP_EVENT,{},OID_PLACEHOLDER,0);
//

	
	i=add_scene(B_ROOM_TABLE,baseScreenTable);
	
	add_object(i,200,620,220,50,arrowRed_down,EN_CHANGE_SCENE,{B_ROOM_F,1});
	add_object(i,680,500,225,75,letter,EN_PROP_EVENT,{});
//	add_object(i,400,310,200,200,placeHolder,EN_ADD_ITEM_TO_INV,{B_ROOM_TABLE,OID_PLACEHOLDER,1,placeHolder},OID_PLACEHOLDER);
	//
	i=add_scene(B_ROOM_B,baseScreen);
	
	add_object(i,0,500,100,100,arrowRed_left,EN_CHANGE_SCENE,{B_ROOM_R});
	add_object(i,900,500,100,100,arrowRed_right,EN_CHANGE_SCENE,{B_ROOM_L});
	//
	i=add_scene(B_ROOM_R,baseScreen);
	
	add_object(i,835,515,-1,-1,screwdriver,EN_ADD_ITEM_TO_INV,{OID_SCREWDRIVER,WID_INV,1,screwdriver},OID_SCREWDRIVER);
	add_object(i,0,500,100,100,arrowRed_left,EN_CHANGE_SCENE,{B_ROOM_F});
	add_object(i,900,500,100,100,arrowRed_right,EN_CHANGE_SCENE,{B_ROOM_B});

	i=add_scene(B_ROOM_DOOR,baseScreenDoor);
	
	add_object(i,100,620,220,50,arrowRed_down,EN_CHANGE_SCENE,{B_ROOM_F,2});
	add_object(i,310,65,350,600,door_closed,EN_CHANGE_TEXT_PROMPT,{-4,-6,OID_OMINOUS_DOOR,3},OID_OMINOUS_DOOR);
	
	i=add_scene(B_ROOM_ELEC_BOX,invis);
	
	add_object(i,300,50,600,600,electrical_box,EN_PROP_EVENT,{},OID_GENERIC,0);
	
	add_object(i,495,155,-1,-1,powerJunction,EN_PROP_EVENT,{});
	add_object(i,495,315,-1,-1,powerJunction,EN_PROP_EVENT,{});
	add_object(i,495,475,-1,-1,powerJunction,EN_PROP_EVENT,{});
	add_object(i,655,155,-1,-1,powerJunction,EN_PROP_EVENT,{});
	add_object(i,655,315,-1,-1,powerJunction,EN_PROP_EVENT,{});
	add_object(i,655,475,-1,-1,powerJunction,EN_PROP_EVENT,{});
	
	add_object(i,346,96,509,509,electrical_box_lid,EN_DESTROY_ALL_OBJECT_INSTANCES,{OID_ELEC_LID},EN_TEST,{0},OID_SCREWDRIVER,OID_ELEC_LID);
	add_object(i,90,550,75,100,arrowRed_down,EN_CHANGE_SCENE,{B_ROOM_F});
//	add_object(i,)
	
}

void build_UI(){
//reference:
//void add_widget(int X, int Y, int W, int H,short panelID, short type, bool visible,std::string sprite_text ="",event_t eventID=EN_PROP_EVENT, data t={},SN_ID sprID=invis, int SID=0);
	int id = create_panel(0,0,1200,700,MAIN_MENU,1); 
	add_widget(500,300,200,100,id,W_BUTTON,1,"START",EN_START_GAME,{},invis,0 );
	id = create_panel(1000,0,200,700,INV,1);
	
	add_widget(1050,525,100,100,id,W_BUTTON,1,"",EN_TEST,{1,2},invis,WID_INV);
	
	add_widget(1050,400,100,100,id,W_BUTTON,1,"",EN_TEST,{1,2},invis,WID_INV);
	
	add_widget(1050,275,100,100,id,W_BUTTON,1,"SN_invis",EN_TEST,{1,2},invis,WID_INV);
	add_widget(1050,150,100,100,id,W_BUTTON,1,"SN_invis",EN_TEST,{1,2},invis,WID_INV);
	add_widget(1050,25,100,100,id,W_BUTTON,1,"SN_invis",EN_TEST,{0},invis,WID_INV);
	add_widget(10,680,980,20,id,W_TEXT,1,textContainer[INT_MIN],EN_PROP_EVENT,{},invis,WID_TEXT_BOX);
	add_widget(0,679,1000,50,id,W_BASIC,1);
	

	curPanel=savedPanels[MAIN_MENU];
}

void user_input(const ALLEGRO_EVENT& ev){
	switch(ev.type){
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
			handler.set_flag_val(GEV_GAME_RUNNING,true);
			graphMNG.end_draw_loop();
			al_destroy_display(graphMNG.disp);
			
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
			printf("Mouse click detected at:\nX: %d\tY: %d\n",ev.mouse.x,ev.mouse.y);
			if(!curPanel->check_collisions(ev.mouse.x,ev.mouse.y))
			action(ev.mouse.x,ev.mouse.y,screen::currentScreen);
			break;
		default:
			break;
	}
}

void example_event(data*t){
	printf("Commencing TEST EVENT\n");
	for(int i = 0; i< t->size();i+=1)
		switch(t->at(i)){
			case 0:
				std::cout<<"Test event execution test\nIf You are reading this, you are on a test branch";
				break;
			case 1:
				std::cout<<"TEST\nThis is a new test message, added recently\n";
				break;
			case 2:
				std:: cout<<"TeSt\nThis is the last message, except for the default one, that was made for this event\n";
				break;
			default:
				std::cout<<"invalid data\n";
				break; 
		}
	
	std::system("pause");
}

void scene_change(data*t){
	if(scenes[t->at(0)]!=NULL && scenes[t->at(0)]!=screen::currentScreen){
		 //Checks whether You are reloading a scene or loading a new one.
		screen::currentScreen=change_scene(t->at(0));
		if(t->size()!=1)
			text::theTextBox->change_text(&textContainer[50*t->at(0)+t->at(1)]);
		else
			text::theTextBox->change_text(&textContainer[50*t->at(0)]);
	}
	if(testBranch)std::cout<<"Scene changed to: " << t->at(0)<< "\n";		
}

void destroy_object_at_scene(data*t){
	scenes[t->at(0)]->delete_child_object(t->at(1));
//	change_scene(t->at(0)); is not required, as the object is at a different scene;
}

void destroy_object_at_this_scene(data*t){
	screen::currentScreen->delete_child_object(t->at(1));
	change_scene(t->at(0));
}

void destroy_all_object_instances(data*t){

	int re;
	for(std::map<int,screen*>::iterator i = scenes.begin(); i != scenes.end(); i++){
		if(i->second) while(i->second->delete_child_object(t->at(0))){;};
		if(screen::currentScreen == i->second)re=i->first;
	}
	change_scene(re);
	
}

void add_item_to_inv(data*t){
	data tempD={t->at(0)};
	std::string welp =invItems.at(t->at(0));
	
//	t->at(1)=WID_INV;
	
	button* temp = (button*)(savedPanels[INV]->return_child(*t));
	
	temp->change_property(make_event(EN_CHANGE_SELECTED_ITEM,{t->at(0)}));
	temp->change_property((char*)welp.c_str());
	temp->change_property(SN_ID(t->at(3)));
	
	text::theTextBox->change_text(&textContainer[-1000-t->at(0)]);
	destroy_all_object_instances(&tempD);
}

void change_selected_item(data*t){
	std::string temp = "";
	if(handler.set_flag_val(GEV_CUR_HELD_ITEM,t->at(0)!= handler.ret_flag_val(GEV_CUR_HELD_ITEM) ? t->at(0) : -1) != -1){
		((temp.append("(I get a hold of the ")).append(invItems[t->at(0)])).append(")");
	}
	else temp = "(I put whatever I was holding away to free my hands)";
	text::theTextBox->change_text(&temp);
	printf("Changed selected item to: %s\n",invItems[handler.ret_flag_val(GEV_CUR_HELD_ITEM)]);
	
}

void create_text_box_prompt(data*t){add_object(t->at(4),0,0,1000,700,invis,EN_CHANGE_TEXT_PROMPT,*t,OID_TEXT_PROMPT);}

void show_text_prompt_of_object(data*t){;}

void change_text_prompt(data*t){
	text::theTextBox->change_text(&textContainer[t->at(0)]);
	object* ptr = screen::currentScreen->find_object_with_ID(objID(t->at(2)));
	printf("Cur bit of dialogue: %d\nTarget bit of dialogue: %d\n",t->at(0),t->at(1));
	if(t->at(0)>t->at(1) && t->at(0) != INT_MIN){
		t->at(0)-=1;
		screen::currentScreen->change_object_event(ptr,make_event(EN_CHANGE_TEXT_PROMPT,*t));
	}
	else{
		std::vector<int> temp={};
		int i;
		switch(t->at(3)){
			case 0: //The object is to be deleted.
			
				screen::currentScreen->delete_child_object(t->at(2));
				if(!change_scene(t->at(4)))printf("The scene with ID of %d does not EXIST.\n",t->at(4));
				break;
			case 1: //Changes t->at(0) to a different value, which may result in: replaying the current dialogue, or changing the prompt to "(...)"
				t->at(0)=t->at(4);
				break;
			case 2: //Creates a new dialogue; for the object with a different terget;
				for(i = t->at(4);i < t->size();i++)temp.push_back(t->at(i));
				screen::currentScreen->change_object_event(ptr,make_event(EN_CHANGE_TEXT_PROMPT,temp));
			case 3: //Keeps replaying the same bit of dialogue;
				break;
		}
	}
}

void start_game(data*t){
	handler.reset_all_flags();

	screen::currentScreen = change_scene(B_ROOM_F);
	curPanel=savedPanels[INV];
	data temp={-1,-3,OID_TEXT_PROMPT,0,B_ROOM_F};
	create_text_box_prompt(&temp);


	printf("Problem? (*>*)\n");
}

void alter_flag(data*t){
	gEventFlags key = (gEventFlags)t->at(0);
	int val = handler.ret_flag_val(key);
	switch(t->at(1)){
		case 0: handler.set_flag_val(key,t->at(2));break;
		case 1: handler.set_flag_val(key,val+t->at(2));break;
		case 2: handler.set_flag_val(key,val-t->at(2));break;
		case 3: handler.set_flag_val(key,val*t->at(2));break;
		case 4: handler.set_flag_val(key,val/t->at(2));break;
		case 5: handler.set_flag_val(key,val%t->at(2));break;
	}
}

void fuse_turn(data*t){
	alter_flag(t);
	handler.set_flag_val((gEventFlags)t->at(0),handler.ret_flag_val((gEventFlags)t->at(0))%6);
	if(handler.ret_flag_val(GEV_FUSE1_POS)==1 && handler.ret_flag_val(GEV_FUSE1_POS)==2 && handler.ret_flag_val(GEV_FUSE1_POS)==3 
	&& handler.ret_flag_val(GEV_FUSE1_POS)==4 && handler.ret_flag_val(GEV_FUSE1_POS)==5 && handler.ret_flag_val(GEV_FUSE1_POS)==6);
}

void register_events(){
	handler.add_event(EN_PROP_EVENT, NULL);
	handler.add_event(EN_TEST, &example_event);
	handler.add_event(EN_CHANGE_SCENE, &scene_change);
	handler.add_event(EN_START_GAME, &start_game);
	handler.add_event(EN_ADD_ITEM_TO_INV, &add_item_to_inv);
	handler.add_event(EN_DESTROY_OBJECT_AT_SCENE, &destroy_object_at_scene);
	handler.add_event(EN_DESTROY_OBJECT_AT_THIS_SCENE, &destroy_object_at_this_scene);
	handler.add_event(EN_DESTROY_ALL_OBJECT_INSTANCES, &destroy_all_object_instances);
	handler.add_event(EN_CHANGE_SELECTED_ITEM, &change_selected_item);
	handler.add_event(EN_CREATE_TEXT_BOX_PROMPT, &create_text_box_prompt);
	handler.add_event(EN_CHANGE_TEXT_PROMPT, &change_text_prompt);
}
//NOTICE: All of the events that are meant to be used in the game, must be regstered in the 
//		eventhandler's event-function map, using the function above