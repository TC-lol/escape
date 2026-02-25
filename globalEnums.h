#ifndef G_ENUMS_H
#define G_ENUMS_H
/*
*This file contains all of the enums any function or class may need.
*The only exception to this rule is the SPRITE_NAMES enumeration as
*it requires to be in a completely separate file, as otherwise, great
*hassle would be required to make it work from this file.
*/
typedef enum EVENT_NAMES{
	EN_PROP_EVENT=0, //Literally does nothing
	EN_TEST,
	EN_CHANGE_SCENE,
	EN_START_GAME,
	EN_ADD_ITEM_TO_INV, 
	EN_DESTROY_OBJECT_AT_SCENE,
	EN_DESTROY_OBJECT_AT_THIS_SCENE,
	EN_DESTROY_ALL_OBJECT_INSTANCES,
	EN_CHANGE_SELECTED_ITEM,
	EN_CREATE_TEXT_BOX_PROMPT,
	EN_CHANGE_TEXT_PROMPT,
}event_t;

typedef enum GLOBAL_EVENT_FLAGS{
	GEV_PROP_FLAG_FIRST=0,
	
	GEV_GAME_RUNNING,
	GEV_CUR_HELD_ITEM,
	GEV_OBT_PLACEHOLDER,
	GEV_OBT_SCREWDRIVER,
	GEV_FUSE2_POS,
	GEV_FUSE1_POS,
	GEV_FUSE3_POS,
	GEV_FUSE4_POS,
	GEV_FUSE5_POS,
	GEV_FUSE6_POS,
	
	GEV_PROP_FLAG_LAST,
} gEventFlags;

typedef enum SCENES{
	SC_NULL=0,
	B_ROOM_L,
	B_ROOM_F,
	B_ROOM_TABLE,
	B_ROOM_B,
	B_ROOM_R,
	B_ROOM_DOOR,
	B_ROOM_ELEC_BOX,
}SC_ID;

enum SPECIAL_WIDG_ID{
	WID_GENERIC_WIDG=0,
	WID_INV,
	WID_TEXT_BOX,
}; 

enum WIDGET_T{
	W_BASIC=0,
	W_PANEL,
	W_BUTTON,
	W_TEXT,
};

enum UI_COLOR_SCHEME{
	C_BORDER=0,
	C_BUTTON_BG,
	C_TEXT,
	C_PANEL_BG,
};

enum PANELS{
	MAIN_MENU=0,
	INV,
	SETTINGS,
};

typedef enum SPECIAL_OBJ_ID{
	OID_GENERIC=0,
	OID_PLACEHOLDER,
	OID_TEXT_PROMPT,
	OID_OMINOUS_DOOR,
	OID_VISUAL_PROP_1,
	OID_VISUAL_PROP_2,
	OID_VISUAL_PROP_3,
	OID_VISUAL_PROP_4,
	OID_ELEC_LID,
	OID_SCREWDRIVER,
	OID_FUSE1,
	OID_FUSE2,
	OID_FUSE3,
	OID_FUSE4,
	OID_FUSE5,
	OID_FUSE6,
	OID_TEST_SUBJECT,
}objID;

 
/*
	SPECIAL_OBJ_ID's are reserved for objects which have
	their properties modified in a way, during the runtime
	and thus require a special identifier to be found.
	This accounts for objects which:
	 - Get deleted
	 - Are added to the inventory
	 - Get rotated/moved
	 - Have their sprite changed
	 - ETC.
*/

#endif