#include <iostream>
#include <vector>
#include <map> 

#include "UI.h" 
#include "eventQueue.h"

ALLEGRO_COLOR colorScheme[10] = 
	{al_map_rgb(0,0,0),
	al_map_rgb(160,160,160),
	al_map_rgb(0,0,0),
	al_map_rgb(255,255,255),
	al_map_rgb(0,0,0),
	al_map_rgb(0,0,0),
	al_map_rgb(0,0,0), 
	al_map_rgb(0,0,0),
	al_map_rgb(0,0,0),
	al_map_rgb(0,0,0),
	};

std::map<int,panel*> savedPanels;
panel* curPanel=NULL;

baseWidget::baseWidget(int X,int Y, int W, int H, bool visible,int SID)
	:x(X),y(Y),h(H),w(W),type(W_BASIC),sID(SID){
	if(visible){}
}

void baseWidget::get_dims(int*X,int*Y,int*W,int*H){
	*X=x;*Y=y;*W=w;*H=h;
}

bool baseWidget::check_collisions(int Mx, int My){
	return Mx >= x && Mx <= x+w && My >=y && My <= y+h ? true : false;
}

panel::panel(int X, int Y, int W, int H, bool visible,int SID)
	:baseWidget(X,Y, W, H, visible,SID){
	type=W_PANEL;
}

panel::~panel(){
	while(!childObj.empty()){
		delete *childObj.begin();
		childObj.pop_front();
	}
	
//	for(std::forward_list<baseWidget*>::iterator i = childObj.begin();i!=childObj.end();i++) delete *i;
}

bool panel::check_collisions(int Mx, int My){
	bool found=false;
	if(Mx >= x && Mx <= x+w && My >=y && My <= y+h){
		for(std::forward_list<baseWidget*>::iterator i =childObj.begin(); i != childObj.end(); i++) if((*i)->check_collisions(Mx,My))break;
		found =true;
	}
	return found;
} 

baseWidget* panel::return_child(data t){
	int loop =0;
	std::forward_list<baseWidget*>::iterator i;
	widgetTypes specPoint;
	printf("Switch case for t[2] is %d\n",t[2]);
	for(i=childObj.begin();i!=childObj.end();i++){
		printf("Loop number %d\n", loop);
		printf("Object at address %p has the sID of %d\n",*i,(*i)->sID);
		switch(t[2]){
			case 0: //First widget with the right ID
				
				if((*i)->sID == t[1]) return *i;break;
			case 1: //First button with an empty ("") dispText
				if((*i)->sID == t[1] && (*i)->type == W_BUTTON){
					printf("//\n");
					specPoint.buttonPoint = static_cast<button*>(*i);
					specPoint.buttonPoint->print();
//					printf("Text: %s\n",(*specPoint.buttonPoint).dispText.c_str());
					if(*specPoint.buttonPoint->dispText == 0 || specPoint.buttonPoint->sprID == invis)return *i;
				}
				break;
			case 2:
				printf("?\n");
				if((*i)->sID == t[1] && (*i)->type == W_TEXT){
					printf("!!!\n");
					specPoint.textPoint = static_cast<text*>(*i);
					printf("!?!?\n");
					return *i;
				}
				printf("????\n");
				break;
			default: //Invalid t[2] argument
				printf("Invalid t[2] arg; Returning NULL.\n");
				return NULL;break;
		}
		loop++;
	}
	printf("Loop finished, appropriate object not found.\n");
	return NULL;
}
 
button::button(int X, int Y, int W, int H, bool visible, event Ev,SN_ID sprID,char* textPtr,int SID)
	:baseWidget(X,Y, W, H, visible,SID), onClick(Ev), sprID(sprID){
	type=W_BUTTON;
	dispText = (char*)malloc(sizeof(char)*(strlen(textPtr)+1));
	strcpy(dispText,textPtr);
}

button::~button(){
	free(dispText);
}
 
bool button::check_collisions(int Mx, int My){
	if(Mx >= x && Mx <= x+w && My >=y && My <= y+h){
		handler.queue_event(&onClick);
		return true;
	}
	return false;
}

void button::change_property(event t)		{onClick=t;}
void button::change_property(SN_ID spriteID)	{sprID=spriteID;}
void button::change_property(char* newText)	{
	dispText = (char*)realloc(dispText,sizeof(char)*(strlen(newText)+1));
	strcpy(dispText,newText);
}

void button::print(){
	if(dispText==NULL || *dispText==0)return;
	printf("%s\n",dispText);
	std::cout<<dispText<<std::endl;
}

text::text(int X, int Y, int W, int H, bool visible, std::string dispText,int SID)
	:baseWidget(X,Y, W, H, visible,SID), textStr(dispText){
	type=W_TEXT;
	printf("%s\n",textStr.c_str());
}

void text::change_text(std::string * newText){
	textStr=*newText;
}

bool text::check_collisions(int Mx, int My){
	return false;
}

text* text::theTextBox = NULL;

short create_panel(int X, int Y, int W, int H, short ID, bool visible){
	panel* temp =  new panel(X,Y,W,H,visible);
	if(ID>=0) savedPanels[ID]=temp;
	return ID;
}

void add_widget(int X, int Y, int W, int H,short panelID, short type, bool visible,std::string sprite_text,event_t eventID,data t,SN_ID sprID,int SID)
{
	char* tempStr= (char*)sprite_text.data();
	event NEW(eventID,t);
	panel* point = savedPanels[panelID]; 
	baseWidget* temp = NULL;
	switch(type){
		case W_BASIC:
			temp = new baseWidget(X,Y,W,H,visible,SID);
			break;
		case W_BUTTON:
			temp = new button(X,Y,W,H,visible,NEW,sprID,tempStr,SID);
			break;
		case W_TEXT:
			printf("Text Box created!\n");
			temp = new text(X,Y,W,H,visible,sprite_text,WID_TEXT_BOX);
			break;
		default:
			return; //Attempting to add an invalid widget ID (PANEL) or a non-existing one terminates the function
	}
	point->childObj.push_front(temp);
	return;
}

void destroy_all_UI(){
	for(auto &i: savedPanels){
		delete i.second;
	}
}