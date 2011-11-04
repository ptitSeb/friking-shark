SET (SRCS_GUISYSTEMS

	./GUISystems/GameInterface.h
	./GUISystems/GameMenu.h
	./GUISystems/GUIDefs.h
	./GUISystems/GUISystems.h
	./GUISystems/LevelOptions.h
	./GUISystems/MainMenu.h
	./GUISystems/MainWindow.h
	./GUISystems/resource.h
	./GUISystems/stdafx.h
	./GUISystems/GameOverDialog.h
	./GUISystems/HighScoresDialog.h
	./GUISystems/HighScoresTable.h
	./GUISystems/ControlsDialog.h
	./GUISystems/KeyCaptureDialog.h
	./GUISystems/CreditsDialog.h
	./GUISystems/LoadDialog.h
	./GUISystems/SaveDialog.h
	./GUISystems/SavedGameRow.h
	./GUISystems/OptionsMenu.h
	./GUISystems/AudioOptions.h

	./GUISystems/GameInterface.cpp
	./GUISystems/GameMenu.cpp
	./GUISystems/GUISystems.cpp
	./GUISystems/LevelOptions.cpp
	./GUISystems/MainMenu.cpp
	./GUISystems/MainWindow.cpp
	./GUISystems/stdafx.cpp
	./GUISystems/GameOverDialog.cpp
	./GUISystems/HighScoresDialog.cpp
	./GUISystems/HighScoresTable.cpp 
	./GUISystems/ControlsDialog.cpp
	./GUISystems/KeyCaptureDialog.cpp
	./GUISystems/CreditsDialog.cpp
	./GUISystems/LoadDialog.cpp
	./GUISystems/SaveDialog.cpp
	./GUISystems/SavedGameRow.cpp
	./GUISystems/OptionsMenu.cpp
	./GUISystems/AudioOptions.cpp
)

IF(ANDROID)
	ADD_LIBRARY ( GUISystems ${SRCS_GUISYSTEMS} )
ELSE()
	ADD_LIBRARY ( GUISystems SHARED ${SRCS_GUISYSTEMS} )

	TARGET_LINK_LIBRARIES (
			GUISystems
		GameGUILib
		GameRunTimeLib
		VectorLib
		GameEntityLib
		${ANDROID_LOG_LIBRARY}
	)
ENDIF()
