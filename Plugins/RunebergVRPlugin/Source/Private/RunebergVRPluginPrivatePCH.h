// Copyright (C) 2017  Runeberg (github: 1runeberg, UE4 Forums: runeberg)

/*
The MIT License (MIT)
Copyright (c) 2017 runeberg (github: 1runeberg, UE4 Forums: runeberg)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Engine.h"
#include "RunebergVRPlugin.h"
#include "RunebergVR_Pawn.h"
#include "RunebergVR_Grabber.h"
#include "RunebergVR_SimpleGrabber.h"
#include "RunebergVR_Movement.h"
#include "RunebergVR_Teleporter.h"
#include "RunebergVR_Climb.h"
#include "RunebergVR_CustomGravity.h"
#include "RunebergVR_ScalableMesh.h"
#include "RunebergVR_Gestures_Database.h"
#include "RunebergVR_Gestures.h"
#include "RunebergVR_Gaze.h"
#include "CoreUObject.h"

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.