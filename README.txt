# Wawrzonowski_Samples
Code samples for recruitment processes.
Projects are described chronologically.

##############
# TrashSoup

A TPP survival action game set in a post-apocalyptic world, combined with a TV reality show theme. Finalist of the Lodz ZTGK 2015 competition.
Created using XNA technology.
Most resources were cut out of the project for the puropse of this showcase.

My work consisits of mainly renderer, resource management and a few gameplay subsystems. Most of it is included in following files/folders:

TrashSoup/Engine/Animator.cs
TrashSoup/Engine/AnimatorState.cs
TrashSoup/Engine/Billboard.cs
TrashSoup/Engine/BoundingFrustumExtended.cs
TrashSoup/Engine/Camera.cs
TrashSoup/Engine/CustomModel.cs
TrashSoup/Engine/DefaultPostEffect.cs
TrashSoup/Engine/Light*
TrashSoup/Engine/Material.cs
TrashSoup/Engine/MirrorMaterial.cs
TrashSoup/Engine/Particle.cs
TrashSoup/Engine/ParticleSystem.cs
TrashSoup/Engine/PostEffect.cs
TrashSoup/Engine/QuadTree.cs
TrashSoup/Engine/ResourceManager.cs
TrashSoup/Engine/SkyboxMaterial.cs
TrashSoup/Engine/Socket.cs
TrashSoup/Engine/Transform.cs
TrashSoup/Engine/WaterMaterial.cs
TrashSoup/Gameplay/DaytimeChange.cs
TrashSoup/Gameplay/Fortification.cs
TrashSoup/Gameplay/FortificationPart.cs
TrashSoup/Gameplay/MessageBox.cs
TrashSoup/Gameplay/PlayerCameraController.cs
TrashSoup/Gameplay/PlayerController.cs			// code related to camera and movement
TrashSoup/Gameplay/PostEffectController.cs
TrashSoup/TrashSoupGame.cs 						// without editor-related code
TrashSoupContent/Resources/Effects/*
SkinnedModelLibrary/*
SkinnedModelPipeline/*

My work does not include serialization code in each of aforementioned files.

##############
# FluidSim

An university project with a goal to create a physical simulation. A fluid simulation was created, using Navier-Stokes' laws, implemented on the GPU (Compute Shaders) for performance.
This project was created in a team of two, my colleague was responsible for the UI, I created the rest.

##############
# Rasterizer

Two university projects combined in one application. One goal was to create a software renderer, starting from triangle rasterization to more complex topics. 
The other was to write an off-line renderer, using ray tracing methods. 
An IRenderer interface was created, so the rendering component can be easily switched and the same scene can be rendered using different methods.

##############
# TheWhiteDeath

Realistic FPP shooter set in the 1939 Winter War. Player takes a role of a Finnish marksman and his objective is to infiltrate and sabotage enemy base. 
Participant of the Digital Dragons Students Talent Show in 2017. Created using Unreal Engine 4 in a team of four (2 programmers, 2 artists).
Most of my responsibilities consist of: 
	- Core gameplay mechanics (moving, shooting, sneaking, jumping, aiming, shooting, interactions),
	- Gun, bullet and hit box definitions and behaviour (recoil, dispersion, etc.),
	- FPP cover system, 
	- Map and objectives UI (all Map*.cpp, HitMarkerWidget.cpp, ObjectiveComponent.cpp).
	
AI, equipment, grenades and skill tree were created by my colleague.

##############
# MorphEngine

A home-made game engine created completely from scratch in a team of two. Project is still unfinished but many core features were implemented and proved to be working.
I was the author of code in following namespaces/folders:
	- animation
	- assetLibrary (w/o MAudio and MAsset interface)
	- core (Engine, Settings, Singleton, StateMachine, Transition)
	- debugging (Statistics)
	- gom (BaseObject, Camera, GameObject, GameObjectComponent, GUITransform, ObjectClickEvaluator, ObjectInitializer, Scene (code related to OctTree and Camera), Transform)
	- memoryManagement
	- renderer
	- resourceManagement (w/o fileSystem)
	- utility (w/o MEnum, MMath, MMatrix, MString and MVector)

##############
# SSDO

An application created for Master Engineer work. A small rendering engine with deferred rendering and post-process framework. Three Screen Space Directional Occlusion algorithms were implemented.
The main goal was to improve the performance of the technique and this was done so by using Statistical Volumetric Obscurance algorithm.
User of the application was able to move and rotate the camera, cycle through post-processes and see current FPS value.

##############