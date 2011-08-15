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

#define MAX_SAVEDGAMES 5

enum eMainMenuAction
{
	eMainMenuAction_UNKNOWN,
	eMainMenuAction_NewGame=0x1000,
	eMainMenuAction_Continue,
	eMainMenuAction_Load,
	eMainMenuAction_Options,
	eMainMenuAction_HighScores,
	eMainMenuAction_Credits,
	eMainMenuAction_Exit
};

enum eOptionsMenuAction
{
	eOptionsMenuAction_UNKNOWN,
	eOptionsMenuAction_Controls=0x1000,
	eOptionsMenuAction_Audio,
	eOptionsMenuAction_Back
};

enum eGameMenuAction
{
	eGameMenuAction_UNKNOWN,
	eGameMenuAction_Continue=0x1000,
	eGameMenuAction_Save,
	eGameMenuAction_Options,
	eGameMenuAction_EndGame
};

enum eConfirmationDialogAction
{
	eConfirmationDialogAction_UNKNOWN,
	eConfirmationDialogAction_Yes=0x1000,
	eConfirmationDialogAction_No
};

enum EGameMode 
{
	eGameMode_Normal=0,
	eGameMode_God,
	eGameMode_InfiniteLives,
	eGameMode_Count
};

enum EGameDifficulty
{
	eGameDifficulty_Easy=0,
	eGameDifficulty_Normal,
	eGameDifficulty_Hard,
	eGameDifficulty_VeryHard,
	eGameDifficulty_Count
};

DECLARE_SERIALIZABLE_ENUMERATION(EGameMode)
DECLARE_SERIALIZABLE_ENUMERATION(EGameDifficulty)

struct SGameState
{
	int nLevel;
	int nCheckpoint;
	unsigned int nPoints;
	unsigned int nWeapon;
	unsigned int nBombs;
	unsigned int nLivesLeft;
	EGameMode    eMode;
	EGameDifficulty eDifficulty;
	
	SGameState(){nPoints=0;nWeapon=0;nLevel=-1;nCheckpoint=-1;nBombs=0;nLivesLeft=0;eMode=eGameMode_Normal;eDifficulty=eGameDifficulty_Easy;}
};

