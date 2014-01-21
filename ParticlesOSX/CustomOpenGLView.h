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

@class NSOpenGLContext, NSOpenGLPixelFormat;

@interface CustomOpenGLView : NSView
{
    @private
        NSOpenGLContext*     _openGLContext;
        NSOpenGLPixelFormat* _pixelFormat;
}

//- (void)awakeFromNib;

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;

- (void)setOpenGLContext:(NSOpenGLContext*)context;
- (NSOpenGLContext*)openGLContext;
- (void)clearGLContext;

- (void)prepareOpenGL;

- (void)update;

+ (NSOpenGLPixelFormat*)defaultPixelFormat;
- (void)setPixelFormat:(NSOpenGLPixelFormat*)pixelFormat;
- (NSOpenGLPixelFormat*)pixelFormat;

@end
