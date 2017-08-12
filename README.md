# Unreal Engine VR Plugin v2.73

Unreal Engine Virtual Reality Pawn and Components plugin for common VR mechanics. Can be used for either Blueprint or C++ VR projects. All nodes (Blueprint) and Functions (C++) have a range of parameters for full flexibility and compatible with the VIVE Motion Controllers/Room-Scale VR projects. 

Also available in the [Unreal Engine Marketplace](https://www.unrealengine.com/marketplace/vr-pawn-components-plugin)

**Supports both SteamVR/Vive & the Oculus Rift**

### Notes:
**For a version compatible with UE 4.16 [CLICK HERE](https://github.com/1runeberg/RunebergVRPlugin/tree/4.16)**
  
## Latest Release Notes (UE 4.17)
2.73:
    - Bugfix Teleport HMD Type check causes crashes when no HMD device is available

2.72:
    - Allow Grabber origin to respect Relative Rotation and Location defined in the Components Tab (Grabber)
	- Added Acceleration to Gravity's influence (VRPawn)
	- Added StepUp Rate in Uneven Terrain Pawn Z/Height adjustments (VRPawn)

2.71:
  - Removed remnants of SteamVR specific references in code base
  
2.7:
  - Enable VR Pawn to react/move properly through uneven terrain (VRPawn)
  - Dedicated node for 360 movement with thumbstick/thumbpad of Motion controllers (Movement component)
  - Various fixes and polish for Gaze, Movement and VR Pawn

2.65:
  - Migrated to UE4.17

2.6:
  - Automatically set HMD Tracking Origin (VRPawn)
  - Added gravity (VRPawn)
  - Customizable target mesh (Beta, Gaze Component)
  - Full 360 Yaw Movement Freedom for use with thumbstick input (Beta, Movement Component)
  - Fix for Oculus height offset issue (VRPawn)


2.5:
  - Added Gaze interaction mechanic (new)
  - Added function to check if HMD is currently worn (in VRPawn)
  - Automatically set camera FOV to 110 to suit current released hardware, overridable (in VRPawn)


## What you get:
  - Pre-configured VR Pawn
  - Gestures Component (Record & Recognize custom VR Gestures)
  - Movement Component (free axis, locked axis, dash/timed motion, bounce from VR bounds & optional use of Nav Mesh bounds)
  - Teleport (Arc, Ray, Blink)
  - Grabber (precision, lock, scan only, grab sky/sun with cycle day and night)
  - Gaze interaction
  - Scalable meshes
  - Climb

## Preview:
https://youtu.be/0Z49S7Q5lpw

## Tutorials:

#### Playlist
https://www.youtube.com/playlist?list=PLngZ5l9HmlOQBtL8dzDD1lTAdXhsVEm4g

#### 1. Setting up a new VR project & enabling VR in existing projects
https://youtu.be/BOPFAYPeCAo

#### 2. Movement / VR Locomotion Component Deep Dive
https://youtu.be/QulxyAc2E08

#### 3. Teleport Component Deep Dive
https://youtu.be/kNZZf6j6T2M

#### 4. Grabber Component Deep Dive
https://youtu.be/cdUKwm9HesU

#### 5. VR Gestures Component Deep Dive
https://youtu.be/z78CMOwGWxM

#### 6. RunebergVR Plugin v2 - Gaze Component Deep Dive
https://youtu.be/tkYi9lnHbVw

#### 7. RunebergVR Plugin v2 - Pawn Gravity, Uneven Terrain, Thumbstick 360 Movement 
https://youtu.be/73Y1tcBl_Lw


## Support Channels:
a. UE4 Forums: 
https://forums.unrealengine.com/showthread.php?109058-VR-Ready-C-Blueprintable-Pawn-with-common-VR-functions-Also-good-as-a-SteamVR-project-template

b. GitHub:
https://github.com/1runeberg/RunebergVRPlugin/issues

c. Reddit:
https://www.reddit.com/r/Vive/comments/4yx3q1/ue4_vr_components_plugin_easily_call_common_vr/


Completely free for personal and commercial use under the [MIT] (https://opensource.org/licenses/MIT) License

*... more tutorials to follow!*


## Credits (for assets in the demo project)

Most assets made by:
Ty Hegner

Surfboard model:
Zaurian868
https://free3d.com/3d-model/surfboard-31485.html

Sword:
lucasfalcao
http://www.blendswap.com/blends/view/70259

Voice:
http://onlinetonegenerator.com/voice-generator.html

Other Third Party:

1. SFX by Freesound.org
2. UE4 Starter & Sample Content
3. UE4 Demoes:Kite, Elemental
4. UE4 Particle Effects Project
5. UE Landscape Project
6. Sonniss.com GDC Bundle + Bulk SFX Samples for Fruity Loops
7. Earth Satellite pic from NASA

To the best of my knowledge, all art & sound assets used in this project are provided either royalty-free or are in the public domain - contact me for any questions.
