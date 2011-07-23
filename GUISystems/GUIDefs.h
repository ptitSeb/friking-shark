//  Friking Shark, a Flying Shark arcade remake.
//  Copyright (C) 2011  Javier Martin Garcia (javiermartingarcia@gmail.com)
//	
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  


enum eMainMenuAction
{
	eMainMenuAction_UNKNOWN,
	eMainMenuAction_NewGame=0x1000,
	eMainMenuAction_Controls,
	eMainMenuAction_HighScores,
	eMainMenuAction_Credits,
	eMainMenuAction_Exit
};

enum eGameMenuAction
{
	eGameMenuAction_UNKNOWN,
	eGameMenuAction_Continue=0x1000,
	eGameMenuAction_Controls,
	eGameMenuAction_EndGame
};

enum eConfirmationDialogAction
{
	eConfirmationDialogAction_UNKNOWN,
	eConfirmationDialogAction_Yes=0x1000,
	eConfirmationDialogAction_No
};
