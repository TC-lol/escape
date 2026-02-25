#ifndef UI_H
#define UI_H

#include <forward_list>
#include <vector>
#include <string>
#include <map>

#include "eventQueue.h"
#include "graphics.h"

#include "globalEnums.h"

union widgetTypes;

class panel;

extern ALLEGRO_COLOR colorScheme[10];
extern std::map<int,panel*> savedPanels;
extern panel* curPanel; 


class baseWidget{
friend void draw();
friend class panel;
protected:
	int x;
	int y;
	int w;
	int h;
	int sID;
public: 
	int type;
	
	baseWidget(int X, int Y, int W, int H, bool visible, int SID=0);
	void get_dims(int*X,int*Y,int*W,int*H);
	virtual bool check_collisions(int Mx, int My);
}; 

class panel : public baseWidget{ 
friend class graphicManager;
friend void add_widget(int,int,int,int,short,short,bool,std::string,event_t,data,SN_ID,int);
protected:
	std::forward_list<baseWidget*> childObj;
public:
	panel(int X, int Y, int W, int H, bool visible,int SID=0);
	~panel();
	bool check_collisions(int Mx, int My);
	baseWidget* return_child(data t);
};

class button : public baseWidget{
friend void draw();
friend class graphicManager;
friend class panel;
protected:
	SN_ID sprID;
	char *dispText;
	
	event onClick;
public:
	button(int X, int Y, int W, int H, bool visible, event Ev,SN_ID sprID,char* textPtr,int SID=0);
	~button();
	bool check_collisions(int Mx, int My);
	void change_property(event t);
	void change_property(SN_ID spriteID);
	void change_property(char* newText);
	void print();
};

class text : public baseWidget{
friend class graphicManager;
protected:
	std::string textStr;
public:
	static text* theTextBox;
	text(int X, int Y, int W, int H, bool visible, std::string dispText,int SID=0);
	void change_text(std::string *newText);
	bool check_collisions(int Mx, int My);
};

short create_panel(int X, int Y, int W, int H,short ID, bool visible);


void add_widget(int X, int Y, int W, int H,short panelID, short type, bool visible,std::string sprite_text ="",event_t eventID=EN_PROP_EVENT, data t={},SN_ID sprID=invis, int SID=0);

void destroy_all_UI();

union widgetTypes{
	baseWidget* bWidgetPoint;
	panel*		panelPoint;
	button*		buttonPoint;
	text*		textPoint;
};

#endif