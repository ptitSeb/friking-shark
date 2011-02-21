enum eMainMenuAction
{
	eMainMenuAction_UNKNOWN,
	eMainMenuAction_NewGame=0x1000,
	eMainMenuAction_Exit
};

enum eGameMenuAction
{
	eGameMenuAction_UNKNOWN,
	eGameMenuAction_Continue=0x1000,
	eGameMenuAction_EndGame
};

enum eConfirmationDialogAction
{
	eConfirmationDialogAction_UNKNOWN,
	eConfirmationDialogAction_Yes=0x1000,
	eConfirmationDialogAction_No
};
