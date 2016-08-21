# Unreal Engine Virtual Reality Plugin

UE4 Plugin - Common VR interface functions exposed in Blueprint as Pawn Components and compatible with the VIVE Motion Controllers/Room-Scale VR projects. 

You can also use this project as a lightweight base SteamVR template. Developed in C++ for speed but all functions are available in Blueprint.

**Current engine supported: 4.12.x**

*Click image below to watch the setup/overview video in YouTube:*
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/QFaPWG1H8TM/0.jpg)](https://www.youtube.com/watch?v=QFaPWG1H8TM?vq=hd1080)

Currently published version includes the following Blueprint callable C++ functions:
  - Grabbing function via the Grabber Component (with configurable range)
  - Push function (with configurable range, min & max distance values)
  - Pull function (with configurable range, min & max distance values)
  - Teleport (via the Targetter component) with support for both spawnable Particle System & Static Mesh
  - Fly around VR Space (via the VRMovement component)


Check out the [project wiki](https://github.com/1runeberg/VR_CPP/wiki) for detailed instructions.

Completely free for personal and commercial use under the [MIT] (https://opensource.org/licenses/MIT) License
