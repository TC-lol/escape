#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <forward_list>
#include <map>
#include <string>
#include <atomic> 
#include <thread>

#include <mutex>
#include <condition_variable>

#include "sprites.h"

#include "globalEnums.h"

typedef std::pair<int,int> pos;
typedef std::pair<pos,pos> dimensions;
 
extern std::map<SN_ID,ALLEGRO_BITMAP*> sprites;

short load_graphics();

class graphicManager{
private:
	std::thread* drawingLoop, *drawingUILoop;
	std::atomic_flag startDraw, startDrawUI, drawFinished;
	std::mutex gLoop, dLoop;
	
//	std::unique_lock<std::mutex> gameLoopLock;
	std::condition_variable cvG,cvD;
	
	ALLEGRO_BITMAP* buffer;
	ALLEGRO_BITMAP* UI_buffer;
	
	std::forward_list<std::pair<dimensions,ALLEGRO_BITMAP*>> drawQueue;
	
	void draw_UI();
	void draw();
public:
	ALLEGRO_DISPLAY* disp;
	
	graphicManager();
	
	void add_to_queue(pos XY,pos WH, ALLEGRO_BITMAP* image);
	void clear_draw_queue();
	
	void begin_draw_loop();
	void end_draw_loop();
	void await_draw_finish();
	void set_target_disp(ALLEGRO_DISPLAY* dis);
};

extern graphicManager graphMNG;

//#include "graphics.cpp"

#endif