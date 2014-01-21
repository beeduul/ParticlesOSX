//
//  MyOpenGLView.m
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#import "MyOpenGLView.h"

#include <OpenGL/gl3.h>

// http://devcry.heiho.net/2012/06/os-x-cocoa-game-development-using.html
@implementation MyOpenGLView

static NSTimer *timer = nil;

const float FRAME_INTERVAL = 1/60.0;

//- (void)windowDidResignMain:(NSNotification *)notification {
//    //    NSLog(@"window did resign main");
//    [timer invalidate];
//    
//    app.deactivate();      // freeze, pause
//    [self setNeedsDisplay:YES];
//}
//
//- (void)windowDidBecomeMain:(NSNotification *)notification {
//    //    NSLog(@"window did become main");
//    
//    app.activate();
//    [self setNeedsDisplay:YES];
//    
//}

- (void)timerEvent:(NSTimer *)t {

    if (app.initialize()) {
        app.update();
        
    }
    
    [self setNeedsDisplay:YES];
}

- (void)awakeFromNib {
    
    timer = [NSTimer timerWithTimeInterval:FRAME_INTERVAL
                                    target:self
                                  selector:@selector(timerEvent:)
                                  userInfo:nil
                                   repeats:YES];
    
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
    
    
}
//
//- (id)initWithFrame:(NSRect)frame
//{
//    self = [super initWithFrame:frame];
//    if (self) {
//    }
//    return self;
//}

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];

	[self drawParticles];
    
    glFlush();
    // the correct way to do double buffering on Mac is this:
    [[self openGLContext] flushBuffer];
    
    int err;
    if ((err = glGetError()) != 0)
        NSLog(@"glGetError(): %d", err);

}

- (void)reshape {
    //  NSLog(@"view reshape {%.02f %.02f}", [self frame].size.width, [self frame].size.height);
    
    // window resize; width and height are in pixel coordinates
    // but they are floats
    float screen_w = [self frame].size.width;
    float screen_h = [self frame].size.height;
    
    // here I cast floats to ints; most systems use integer coordinate systems
    app.resize((int)screen_w, (int)screen_h);
}

- (void) drawParticles
{
    app.draw();
}

- (void)prepareOpenGL {
    app.init_gl();
    
    // this sets swap interval for double buffering
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // this enables alpha in the frame buffer (commented now)
    //  GLint opaque = 0;
    //  [[self openGLContext] setValues:&opaque forParameter:NSOpenGLCPSurfaceOpacity];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseDragged:(NSEvent *)theEvent {
    NSPoint aPoint = [theEvent locationInWindow];
    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];
    
    app.mouseDraggedAt(aPoint.x, aPoint.y);
}

- (void)mouseDown:(NSEvent *)theEvent {

    NSPoint aPoint = [theEvent locationInWindow];
    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];

    app.mouseDownAt(aPoint.x, aPoint.y);
    
}


- (void)mouseUp:(NSEvent *)theEvent {
}

//- (void)prepareOpenGL
//{
//    // Synchronize buffer swaps with vertical refresh rate
//    GLint swapInt = 1;
//    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
//    
//    // Create a display link capable of being used with all active displays
//    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
//    
//    // Set the renderer output callback function
//    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
//    
//    // Set the display link for the current renderer
//    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
//    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
//    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
//    
//    // Activate the display link
//    CVDisplayLinkStart(displayLink);
//}
//
//- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
//{
//    // Add your drawing codes here
//    
//    return kCVReturnSuccess;
//}
//
//- (void)dealloc
//{
//    // Release the display link
//    CVDisplayLinkRelease(displayLink);
//    
////    [super dealloc];
//}

@end
