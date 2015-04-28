/*
 * FileContentScreen.cpp
 *
 *  Created on: 20/04/2015
 *      Author: DelbenAfonso
 */

#include "FileContentScreen.h"
#include "libs/Kernel.h"
#include "libs/SerialMessage.h"
#include "Panel.h"
#include "PanelScreen.h"
#include "MainMenuScreen.h"
#include "ControlScreen.h"
#include "libs/nuts_bolts.h"
#include "libs/utils.h"
#include <string>
#include "modules/robot/RobotPublicAccess.h"
#include "PublicData.h"
#include "checksumm.h"
#include "LcdBase.h"

#include "SerialConsole.h"

#include <math.h>
#include <stdio.h>

FileContentScreen::FileContentScreen() {
	// TODO Auto-generated constructor stub

}

// When entering this screen
void FileContentScreen::on_enter(){
    THEPANEL->enter_menu_mode();
    THEPANEL->setup_menu(this->count_file_content());
    this->refresh_menu();
}

// For every ( potential ) refresh of the screen
void FileContentScreen::on_refresh()
{
    if ( THEPANEL->menu_change() ) {
        THEKERNEL->serial->printf("menu change");
        this->refresh_menu();
    }
    if ( THEPANEL->click() ) {
        this->clicked_line(THEPANEL->get_menu_current_line());
    }

}

void FileContentScreen::on_exit()
{

}

void FileContentScreen::clicked_line(uint16_t line)
{
	switch ( line ) {

	        case 0:
	        fclose(this->current_file);
	        THEPANEL->enter_screen(this->parent);
	        break;
	}

}

void FileContentScreen::display_menu_line(uint16_t line)
{
    // in menu mode
    switch ( line ) {
		case 0: THEPANEL->lcd->printf("Back");  break;
		default: THEPANEL->lcd->printf("%s",this->lineContent[line-1]);
				// THEKERNEL->serial->printf("%s\n",this->lineContent[line-1]);
				 break;
    }
}

uint16_t FileContentScreen::count_file_content()
{
	uint16_t lineCount = 0;
	uint8_t i = 0;
//	char p;
	 do{
		 memset(this->lineContent[i],0x00,sizeof(this->lineContent[i]));
		 i++;
	 }while( i<8);
	 i = 0;
	this->current_file = fopen(this->current_path.c_str(),"r");
	 if(this->current_file == NULL) {
		return 0;
	}
	char buf[130]; // lines upto 21 characters are allowed, anything longer is discarded
	bool discard = false;
	while(fgets(buf, sizeof(buf), this->current_file) != NULL) {
	 int len = strlen(buf);
	 if(buf[len - 1] == '\n' || feof(this->current_file)) {
		 if(discard) { // we are discarding a long line
			 discard = false;
			 continue;
		 }
		 if(len == 1) continue; // empty line
		 do{
			 this->lineContent[lineCount][i] = buf[i];
			 i++;
		 }while((buf[i] != '\r') && (buf[i] != '\n') && (i<21));
		 THEKERNEL->serial->printf("\r%s",buf);
		 i = 0;
		 //THEKERNEL->serial->printf("Line number:%d\n\r",lineCount);

		 lineCount++;
		 if (lineCount > 6) return 8; // we feed one line per main loop
	}
	  else {
		 // discard long line
		 THEKERNEL->serial->printf("Warning: Discarded long line\n");
		 discard = true;
	 }
	}
	return lineCount;
}

void FileContentScreen::setCurrentPath(std::string path)
{
	this->current_path = path;
	THEKERNEL->serial->printf("Caminho: %s",this->current_path.c_str());
}

void FileContentScreen::get_file_content()
{

}
