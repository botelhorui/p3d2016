Assignment 3:
	- All the program parameters are set in the main2.cpp file

	Scene selection:
		- the parameter "SCENE_FILE" is used to choose the scene to be tested
		- the scene must be imported to the "scenes" folder and the name must be added to the "scene_files" array
		- the allowed scenes file format is .nff

	Ray Tracing Depth:
		- the depth of ray tracing can be changed in the "MAX_DEPTH" parameter
		
	Rendering modes:
		- normal
		- monte carlo anti aliasing
		- depth of field
		- depth of field + monte carlo anti aliasing

		- in order to change the rendering mode, the parameter "DRAW_MODE" accepts the following values:
			NORMAL
			MONTE_CARLO
			DOF
			DOF_MONTE_CARLO

	Soft Shadows:
		- soft shadows can be disabled or enabled by changing the "SOFT_SHADOWS_ON" boolean parameter
		- soft shadows may be cast from an area or sphere light, by changing the "SOFT_SHADOW_TYPE" parameter. The parameter accepts the following values:
			AREA
			SPHERE
			
		- when using an area light, the "LIGHT_AREA_U" and "LIGHT_AREA_V" parameters define the light orientation, and the "LIGHT_AREA_SIDE" parameter define the light area
		- the sphere light size can be changed in the "LIGHT_RADIUS" parameter
		- the samples used can be changed in the "SOFT_SHADOWS_SAMPLES" parameter

	Monte Carlo Anti Aliasing:
		- the maximum divisions that monte carlo can reach are set in the "MAX_DIVISIONS" parameter
		- the threshold value is set in the "MONTE_CARLO_THRESHOLD" parameter

	Depth of Field:
		- the depth of field samples can be set in the "DEPTH_OF_FIELD_SAMPLES" parameter

	Acceleration Grid:
		- the acceleration grid can be disabled or enabled by changing the "GRID_ON" boolean parameter
		- the acceleration grid works with all the configurations below:
			- normal rendering
				- hard shadows
				- no anti aliasing
				- no depth of field

			- soft shadows
			- monte carlo anti aliasing
			- depth of field
			
		- the grid cell factor can be changed in the "GRID_WIDTH_MULTIPLIER" parameter
