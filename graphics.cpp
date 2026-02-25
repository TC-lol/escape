#include <fstream>
#include <string>
#include <vector>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <iostream>

#include <functional>
#include <atomic>
#include <thread>

#include "graphics.h"
#include "UI.h" 

std::map<SN_ID,ALLEGRO_BITMAP*> sprites; 

ALLEGRO_FONT* font=NULL;

graphicManager::graphicManager(){
	startDraw.test_and_set();
	startDrawUI.test_and_set();
	drawFinished.test_and_set();

	drawingLoop=new std::thread(&graphicManager::draw,this);
	drawingLoop->detach();
	
	drawingUILoop = new std::thread(&graphicManager::draw_UI,this);
	drawingUILoop->detach();
}

short load_graphics(){
	std::ifstream grafika("sprites.h"); 
	std::string odczyt, baza;
	int count=-2;
	while(!grafika.eof()) {
		count++;printf("%d:",count);
		getline(grafika,odczyt);
		std::cout<<odczyt<<std::endl;
		if(count%2==1) continue;
		baza = "./sprites/";
		(baza.append(odczyt)).append(".png");
//		const char * nazwa = baza.c_str();
		printf("%d - ",SN_ID(count/2));
		sprites[SN_ID(count/2)] = al_load_bitmap(baza.c_str());
		printf("%p\n",sprites[SN_ID(count/2)]);

//		delete nazwa;
	
	}
	for(SN_ID i=invis; i<=door_open;i=SN_ID(i+1)){
		std::cout<<i << '-' <<sprites[i]<<std::endl;
	}
	sprites[invis] = al_create_bitmap(0,0);
	font = al_load_font("./sprites/fonttest.png",2000,0);
	grafika.close();
	for(SN_ID i=invis; i<=door_open;i=SN_ID(i+1)){
		std::cout<<i << '-' <<sprites[i]<<std::endl;
	}
	return 0;
};

void graphicManager::add_to_queue(pos XY, pos WH, ALLEGRO_BITMAP* image){drawQueue.push_front(std::make_pair(std::make_pair(XY,WH),image));}

void graphicManager::clear_draw_queue(){drawQueue.clear();}

void graphicManager::begin_draw_loop(){
cvD.notify_all();
startDraw.clear();
startDrawUI.clear();
}

void graphicManager::end_draw_loop(){delete drawingLoop; delete drawingUILoop;}

void graphicManager::await_draw_finish(){
//	std::unique_lock<std::mutex>lck(gLoop); 
//	cvG.wait(lck);
	while(drawFinished.test_and_set())std::this_thread::yield();
}

void graphicManager::draw(){
	
	std::unique_lock<std::mutex>lck(dLoop);
while(true){
	

	
	if(startDraw.test_and_set())cvD.wait(lck);
//	printf("Start Draw...?\n");		
//	while(startDraw.test_and_set())std::this_thread::yield();
//	std::thread drawingUI(&graphicManager::draw_UI,this);
	
	
	
	al_set_target_bitmap(buffer);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	
//	printf("Start Draw...!\n");	
	for(const auto& i: drawQueue){
//		ALLEGRO_BITMAP *TEST = i.second;
		
		//int test = i.first.second.second;
//		printf("DRAWING...\n");
		if(i.second!=NULL && i.second != sprites[invis]) 
			al_draw_scaled_bitmap(i.second,0,0,al_get_bitmap_width(i.second),al_get_bitmap_height(i.second), 	//bitmap, source position and dimensions
				i.first.first.first,i.first.first.second,														//target position
				i.first.second.first < 0 ? al_get_bitmap_width(i.second) : i.first.second.first,				//target width
				i.first.second.second < 0 ? al_get_bitmap_height(i.second) : i.first.second.second,0);			//target height and flags
		else
//			printf("Whyyyyyyy?\n");
			al_draw_filled_rectangle(i.first.first.first,i.first.first.second,i.first.second.first,i.first.second.second,colorScheme[C_PANEL_BG]);
			
		};
	
//	drawingUI.join();
	if(startDraw.test_and_set())cvD.wait(lck);
	
	al_set_target_backbuffer(disp);
	al_draw_bitmap(buffer,0,0,0);
	al_draw_bitmap(UI_buffer,0,0,0);
//	printf("'bout to flip\n");
	al_flip_display();
//	cvG.notify_one();
	
	drawFinished.clear();
//	printf("Finished Draw!\n");
}

};

/*
 *	TO DO: Make the thread executing the draw_UI() method constantly run pararell to the
 *		draw() method instead of being created everytime draw() starts a loop 
 *		- that approach is capital 'S' Slow; Also, look into optimising the redrawing operation some more.
 */

void graphicManager::draw_UI(){
	std::unique_lock<std::mutex>lck(dLoop);

while(true){
//	printf("DRAWING UI\n");
	if(startDrawUI.test_and_set())cvD.wait(lck);
	
	std::forward_list<baseWidget*>::iterator it = curPanel->childObj.before_begin();
	
	al_set_target_bitmap(UI_buffer);
	al_clear_to_color(al_map_rgba(0,0,0,0));
	
	baseWidget* bPoint = curPanel;
	widgetTypes derPoint;
	int x,y,w,h; 
//	printf("Start draw UI...\n");
///*
	do{
		it++;
//		printf("...Drawing UI...\n");
		bPoint->get_dims(&x,&y,&w,&h);
		
		switch(bPoint->type){
			case W_PANEL:
//				printf("IS THIS IT???\n");
				derPoint.panelPoint = static_cast<panel*>(bPoint);
				
				al_draw_filled_rectangle(x,y,x+w,y+h,colorScheme[C_PANEL_BG]);
				al_draw_rectangle(x,y,x+w,y+h,colorScheme[C_BORDER],1);
//				printf("Really???\n");
				break;
			case W_BUTTON:
				derPoint.buttonPoint = static_cast<button*>(bPoint);
				
				al_draw_filled_rectangle(x,y,x+w,y+h,colorScheme[C_BUTTON_BG]);
				al_draw_rectangle(x,y,x+w,y+h,colorScheme[C_BORDER],1);
				if(derPoint.buttonPoint->sprID)
//* 
					
					al_draw_scaled_bitmap(sprites[derPoint.buttonPoint->sprID],0,0,
										al_get_bitmap_width(sprites[derPoint.buttonPoint->sprID]),
										al_get_bitmap_height(sprites[derPoint.buttonPoint->sprID]),
										x,y,w,h,0);
				else
					al_draw_text(font,colorScheme[C_TEXT],x,y,0,derPoint.buttonPoint->dispText);
				break;
			case W_TEXT:
//				printf("WHY HERE???\n");
				derPoint.textPoint=static_cast<text*>(bPoint);
				al_draw_text(font,colorScheme[C_TEXT],x,y,0,derPoint.textPoint->textStr.c_str());
				break;
			case W_BASIC:
				
				al_draw_filled_rectangle(x,y,x+w,y+h,colorScheme[C_BUTTON_BG]);
				al_draw_rectangle(x,y,x+w,y+h,colorScheme[C_BORDER],1);
				break;
			default:
				break;
		}
		
		if(it!=curPanel->childObj.end())bPoint=*it;
//		printf("Finished Drawing A Widget!\n");
	}while(it!=curPanel->childObj.end());
	
	startDraw.clear();
	cvD.notify_all();
//	printf("UI DRAWN\n");
}

}

void graphicManager::set_target_disp(ALLEGRO_DISPLAY* dis){
	disp=dis;
	buffer=al_create_bitmap(al_get_display_width(disp),al_get_display_height(disp));
	UI_buffer=al_create_bitmap(al_get_display_width(disp),al_get_display_height(disp));
}

graphicManager graphMNG;