How to build:

1) Install the following packages:

	mesa-libGL-devel.i386 
	mesa-libGLU-devel.i386
	libXrandr-devel.i386
	libopenal-dev 
	libalut-dev
	libogg-dev
	libvorbis-dev

2) Run "make". Faster with make -j<number of cpus> :)

	The global makefile at the root folder  is just a batch calling the makefiles on the other folders.
	It does not stop if a project compilation fails, run more than once to be sure it built correctly.


How to use:

Change to Demo/Bin/Release

	Play the game: ./GameEngine
	Scenario Editor: ./GameEngine ScenarioEditor
	Formation Editor: ./GameEngine FormationEditor
	Entity Editor: ./GameEngine EntityEditor

Scenario Editor Keys:

	Edit mode:
		ASWD/Arrows: FPS-like movement.
		Ctrl+ASWD/Arrows: Rotate camera
		R: Up
		F: Down
		Home: Focus selected entity, formation or the whole scenario.
		Insert: Add a new route point to an entity
		Supr: Remove selected route point

	Game simulation mode:
		ASWD/Arrows: Move player.
		Ctrl: Fire bullets (unlimmited ammo)
		Alt: Fire bomb (uses ammo)
		Pause: Pause/Resume the game
		Space: Process just one frame and pause.
	
	Mode change:
		P: 	Show/Hide game simulation starting position
		F5: Start/Restart game simulation 
		Shit+F5: End game simulation 
		F1: Switch between edition and game simulation cameras. Useful to view the game from any perspective.

	File:
		F3: Open
		F2: Save

	Rendering:
		H: Activate/Deactivate shaders
		T: Activate/Deactivate textures
		L: Activate/Deactivate solid/wireframe
		I: Activate/Deactivate lighting
		O: Activate/Deactivate shadows
		B: Activate/Deactivate blending
		G: Activate/Deactivate fog

	

