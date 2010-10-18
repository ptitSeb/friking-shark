typedef enum eMainMenuAction
{
	eMainMenuAction_UNKNOWN,
	eMainMenuAction_NewGame=0x1000,
	eMainMenuAction_Exit
};

typedef enum eGameMenuAction
{
	eGameMenuAction_UNKNOWN,
	eGameMenuAction_Continue=0x1000,
	eGameMenuAction_EndGame
};

typedef enum eConfirmationDialogAction
{
	eConfirmationDialogAction_UNKNOWN,
	eConfirmationDialogAction_Yes=0x1000,
	eConfirmationDialogAction_No
};
