//
//  MyOpenGLView.h
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#include "ParticleApp.h"

//#include <QuartzCore/CVDisplayLink.h>

@interface MyOpenGLView : NSOpenGLView<NSWindowDelegate>
{
//    CVDisplayLinkRef displayLink; //display link for managing rendering thread
    ParticleApp app;
}

- (void) awakeFromNib;
- (void) prepareOpenGL;
- (void) drawRect: (NSRect) bounds;
- (void) drawParticles;


- (void)mouseDragged:(NSEvent *)theEvent;
- (void)mouseDown:(NSEvent *)theEvent;


//- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime;
//- (void)dealloc;

//// This is the renderer output callback function
//static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
//{
//    CVReturn result = [(MyOpenGLView*)CFBridgingRelease(displayLinkContext) getFrameForTime:outputTime];
//    return result;
//}

@end
