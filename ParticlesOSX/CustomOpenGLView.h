//
//  CustomOpenGLView.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//
// picked from https://developer.apple.com/library/mac/documentation/graphicsimaging/conceptual/OpenGL-MacProgGuide/opengl_drawing/opengl_drawing.html#//apple_ref/doc/uid/TP40001987-CH404-SW3
//

#include "ParticleApp.h"

#import <Cocoa/Cocoa.h>
//#import <QuartzCore/CVDisplayLink.h>
#import <CoreVideo/CVDisplayLink.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CustomOpenGLView : NSOpenGLView
{
	CVDisplayLinkRef displayLink;

    NSTimer *renderTimer;
    
    ParticleApp app;
}

- (NSPoint) convertToAppCoordinateSystem:(NSPoint)thePoint;

- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;

- (unsigned long) modifiers;

- (void)mouseDragged:(NSEvent *)theEvent;
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;

- (void)mouseMoved:(NSEvent *)theEvent;
- (void)mouseEntered:(NSEvent *)theEvent;
- (void)mouseExited:(NSEvent *)theEvent;

- (void)timerFired:(id)sender;

@end
