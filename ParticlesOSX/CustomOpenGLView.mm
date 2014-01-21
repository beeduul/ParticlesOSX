//
//  CustomOpenGLView.m
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#import "CustomOpenGLView.h"


@implementation CustomOpenGLView

- (void)awakeFromNib {

    // unsure about all this
    if (self != nil) {
        _pixelFormat = [[self class] defaultPixelFormat];
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_surfaceNeedsUpdate:)
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
    }

}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
    self = [super initWithFrame:frameRect];
    if (self != nil) {
        _pixelFormat   = format;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_surfaceNeedsUpdate:)
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
    }
    return self;
}

- (void) _surfaceNeedsUpdate:(NSNotification*)notification
{
    [self update];
}


+ (NSOpenGLPixelFormat*) defaultPixelFormat
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        // Must specify the 3.2 Core Profile to use OpenGL 3.2
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        0
    };

    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    return pf;
}

- (void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat
{
    _pixelFormat = pixelFormat;
}

- (NSOpenGLPixelFormat*)pixelFormat
{
    return _pixelFormat;
}


- (void)setOpenGLContext:(NSOpenGLContext*)context
{
    _openGLContext = context;
}

- (NSOpenGLContext*)openGLContext
{
    return _openGLContext;
}

- (void) clearGLContext
{
    
}

- (void)prepareOpenGL
{
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    // Add your drawing codes here

    return kCVReturnSuccess;
}

- (void)lockFocus
{
    NSOpenGLContext* context = [self openGLContext];
    
    [super lockFocus];
    if ([context view] != self) {
        [context setView:self];
    }
    [context makeCurrentContext];
}

- (void)update
{
    NSOpenGLContext* context = [self openGLContext];
    [context update];
}

-(void) drawRect
{
    NSOpenGLContext* context = [self openGLContext];

    [context makeCurrentContext];

    //Perform drawing here
    
    [context flushBuffer];
}

@end
