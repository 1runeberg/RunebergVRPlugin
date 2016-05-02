# Unreal Engine Virtual Reality-ready Pawn

UE4 C++ VR Pawn for use in both C++ and Blueprint projects (Pawn is Blueprintable so no knowledge of C++ is required to use it). Contains common VR interface functions and compatible with the VIVE Motion Controllers/Room-Scale VR projects.

You can also use this project as a lightweight base SteamVR template.

**Current engine version: 4.11.2**

*Click image below to watch the setup/overview video in YouTube:*
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/QFaPWG1H8TM/0.jpg)](https://www.youtube.com/watch?v=QFaPWG1H8TM?vq=hd1080)

WIP: Currently published version includes the following Blueprint callable C++ functions:
  - Grabbing function via the Grabber Component (with configurable range)
  - Push function (with configurable range, min & max distance values)
  - Pull function (with configurable range, min & max distance values)
  - Teleport (via the Targetter component) with support for both spawnable Particle System & Static Mesh

Upcoming:
  - Fly & Dive Function

Check out the [project wiki](https://github.com/1runeberg/VR_CPP/wiki) for detailed instructions.

Completely free for personal and commercial use under [GPLv3](http://www.gnu.org/licenses/gpl-3.0.en.html) as long as your project/binaries shows links to this github project as credit. 

_If you want to release a closed-source project from this, please contact me and I'll provide a formal non-transferable license waiver completely free with the only stipulations being a link back to this github project page and permission to list your project/game in a publicly available list of projects that uses code from this repository._
