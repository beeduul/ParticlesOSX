//
//  CustomOpenGLView.m
//  ParticlesOSX
//
//  Created by Jeremy Biddle on 1/18/14.
//  Copyright (c) 2014 beeduul. All rights reserved.
//

#import "CustomOpenGLView.h"
#include "ParticleApp.h"

#import "MyGLUtilities.h"

//void decipherKeyEvent((NSEvent *) anEvent, int& ch, int& modifiers)
//{
//    // given anEvent (NSEvent*) figure out what key
//    // and modifiers we actually want to look at,
//    // to compare it with a menu key description
//    
//    unsigned int quals = [anEvent modifierFlags];
//    
//    NSString* str = [anEvent characters];
//    NSString* strWithout = [anEvent charactersIgnoringModifiers];
//    
//    unichar ch = [str length] ? [str characterAtIndex:0] : 0;
//    unichar without = [strWithout length] ? [strWithout characterAtIndex:0] : 0;
//    
//    if(!(quals & NSNumericPadKeyMask))
//    {
//        if((quals & NSControlKeyMask))
//        {
//            ch = without;
//        }
//        else if(quals & NSAlternateKeyMask)
//        {
//            if(0x20 < ch && ch < 0x7f && ch != without)
//                quals &= ~NSAlternateKeyMask;
//            else  ch = without;
//        }
//        else if((quals & (NSCommandKeyMask | NSShiftKeyMask)) == (NSCommandKeyMask | NSShiftKeyMask))
//        {
//            ch = without;
//        }
//        
//        if((0x20 < ch && ch < 0x7f) || ch == 0x19)
//            quals &= ~NSShiftKeyMask;
//    }
//    
//    // the resulting values
//    key = ch;
//    modifiers = quals & (NSNumericPadKeyMask | NSShiftKeyMask | NSControlKeyMask | NSAlternateKeyMask | NSCommandKeyMask);
//    
//}

@interface CustomOpenGLView (PrivateMethods)
- (void) initGL;
@end

@implementation CustomOpenGLView

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
	// There is no autorelease pool when this method is called
	// because it will be called from a background thread.
	// It's important to create one or app can leak objects.
    @autoreleasepool {
        //[self drawView];
	
        return kCVReturnSuccess;
    }
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
									  const CVTimeStamp* now,
									  const CVTimeStamp* outputTime,
									  CVOptionFlags flagsIn,
									  CVOptionFlags* flagsOut,
									  void* displayLinkContext)
{
    // http://stackoverflow.com/questions/8835956/make-cvdisplaylink-automatic-reference-counting-play-well-together
    @autoreleasepool {
        //CVReturn result = [(CustomOpenGLView*)displayLinkContext getFrameForTime:outputTime];
        CVReturn result = [(__bridge CustomOpenGLView*)displayLinkContext getFrameForTime:outputTime];
        return result;
    }
}


- (void)awakeFromNib
{
    [[self window] makeFirstResponder:self];
    
    NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		// Must specify the 3.2 Core Profile to use OpenGL 3.2
//#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3
		NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
//#endif
		0
	};
	
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
	if (!pf)
	{
		NSLog(@"No OpenGL pixel format");
	}
    
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    
//#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 && defined(DEBUG)
	// When we're using a CoreProfile context, crash if we call a legacy OpenGL function
	// This will make it much more obvious where and when such a function call is made so
	// that we can remove such calls.
	// Without this we'd simply get GL_INVALID_OPERATION error for calling legacy functions
	// but it would be more difficult to see where that function was called.
	CGLEnable((CGLContextObj) [context CGLContextObj], kCGLCECrashOnRemovedFunctions);
//#endif
	
    [self setPixelFormat:pf];
    
    [self setOpenGLContext:context];
    
#if SUPPORT_RETINA_RESOLUTION
    // Opt-In to Retina resolution
    [self setWantsBestResolutionOpenGLSurface:YES];
#endif // SUPPORT_RETINA_RESOLUTION
    
    renderTimer = [NSTimer timerWithTimeInterval:1.0/60.0
                                          target:self
                                        selector:@selector(timerFired:)
                                        userInfo:nil
                                         repeats:YES];
    
    [[NSRunLoop currentRunLoop] addTimer:renderTimer
                                 forMode:NSDefaultRunLoopMode];
    [[NSRunLoop currentRunLoop] addTimer:renderTimer
                                 forMode:NSEventTrackingRunLoopMode]; //Ensure timer fires during resize

}

// Timer callback method
- (void)timerFired:(id)sender
{

    CGLLockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);

    app.update();

    CGLUnlockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);

    // It is good practice in a Cocoa application to allow the system to send the -drawRect:
    // message when it needs to draw, and not to invoke it directly from the timer.
    // All we do here is tell the display it needs a refresh
    [self setNeedsDisplay:YES];
}


- (void)prepareOpenGL
{
	[super prepareOpenGL];
	
	// Make all the OpenGL calls to setup rendering
	//  and build the necessary rendering objects
	[self initGL];
	
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (void *)CFBridgingRetain(self));
	
	// Set the display link for the current renderer
	CGLContextObj cglContext = (CGLContextObj) [[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj) [[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	
	// Activate the display link
	CVDisplayLinkStart(displayLink);
	
	// Register to be notified when the window closes so we can stop the displaylink
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowWillClose:)
												 name:NSWindowWillCloseNotification
											   object:[self window]];

    app.initialize();
}

- (void) windowWillClose:(NSNotification*)notification
{
	// Stop the display link when the window is closing because default
	// OpenGL render buffers will be destroyed.  If display link continues to
	// fire without renderbuffers, OpenGL draw calls will set errors.
	
	CVDisplayLinkStop(displayLink);
}

- (void) initGL
{
	// The reshape function may have changed the thread to which our OpenGL
	// context is attached before prepareOpenGL and initGL are called.  So call
	// makeCurrentContext to ensure that our OpenGL context current to this
	// thread (i.e. makeCurrentContext directs all OpenGL calls on this thread
	// to [self openGLContext])
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	// Init our renderer.  Use 0 for the defaultFBO which is appropriate for
	// OSX (but not iOS since iOS apps must create their own FBO)
	// m_renderer = [[OpenGLRenderer alloc] initWithDefaultFBO:0];
    
}

- (void) reshape
{
	[super reshape];
	
	// We draw on a secondary thread through the display link. However, when
	// resizing the view, -drawRect is called on the main thread.
	// Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing.
	CGLLockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);
    
	// Get the view size in Points
	NSRect viewRectPoints = [self bounds];
    
#if SUPPORT_RETINA_RESOLUTION
    
    // Rendering at retina resolutions will reduce aliasing, but at the potential
    // cost of framerate and battery life due to the GPU needing to render more
    // pixels.
    
    // Any calculations the renderer does which use pixel dimentions, must be
    // in "retina" space.  [NSView convertRectToBacking] converts point sizes
    // to pixel sizes.  Thus the renderer gets the size in pixels, not points,
    // so that it can set it's viewport and perform and other pixel based
    // calculations appropriately.
    // viewRectPixels will be larger (2x) than viewRectPoints for retina displays.
    // viewRectPixels will be the same as viewRectPoints for non-retina displays
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
#else //if !SUPPORT_RETINA_RESOLUTION
    
    // App will typically render faster and use less power rendering at
    // non-retina resolutions since the GPU needs to render less pixels.  There
    // is the cost of more aliasing, but it will be no-worse than on a Mac
    // without a retina display.
    
    // Points:Pixels is always 1:1 when not supporting retina resolutions
    NSRect viewRectPixels = viewRectPoints;
    
#endif // !SUPPORT_RETINA_RESOLUTION
    
	// Set the new dimensions in our renderer
	//[m_renderer resizeWithWidth:viewRectPixels.size.width
    //                  AndHeight:viewRectPixels.size.height];
	
    app.resize(viewRectPixels.size.width, viewRectPixels.size.height);
    
	CGLUnlockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);
}


- (void)renewGState
{
	// Called whenever graphics state updated (such as window resize)
	
	// OpenGL rendering is not synchronous with other rendering on the OSX.
	// Therefore, call disableScreenUpdatesUntilFlush so the window server
	// doesn't render non-OpenGL content in the window asynchronously from
	// OpenGL content, which could cause flickering.  (non-OpenGL content
	// includes the title bar and drawing done by the app with other APIs)
	[[self window] disableScreenUpdatesUntilFlush];
    
	[super renewGState];
}

- (void) drawRect: (NSRect) theRect
{
	// Called during resize operations
	
	// Avoid flickering during resize by drawiing
	[self drawView];
}

- (void) drawView
{
	[[self openGLContext] makeCurrentContext];

	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main
	// thread. Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing
	CGLLockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);
    
    app.draw();

	// [m_renderer render];
    
	CGLFlushDrawable((CGLContextObj) [[self openGLContext] CGLContextObj]);
	CGLUnlockContext((CGLContextObj) [[self openGLContext] CGLContextObj]);
}

- (void)keyDown:(NSEvent *)theEvent {
    int key;

//    decipherKeyEvent(theEvent, key, modifiers);
    
    NSString *keyCharacters = [theEvent charactersIgnoringModifiers];
    for (int i = 0; i < [keyCharacters length]; i++) {
        unichar c = [keyCharacters characterAtIndex: i];
        app.keyDown(c, [self modifiers]);
    }
}

- (void)keyUp:(NSEvent *)theEvent {
    int key;

//    decipherKeyEvent(theEvent, key, modifiers);

    NSString *keyCharacters = [theEvent charactersIgnoringModifiers];
    for (int i = 0; i < [keyCharacters length]; i++) {
        unichar c = [keyCharacters characterAtIndex: i];
        app.keyUp(c, [self modifiers]);
    }
    
}

- (PKeyModifier) modifiers
{
    NSUInteger nsModifiers = [NSEvent modifierFlags];
    PKeyModifier mods = 0;
    if (nsModifiers & NSAlphaShiftKeyMask)
        mods |= KeyModifiers_CapsLock;
    if (nsModifiers & NSShiftKeyMask)
        mods |= KeyModifiers_Shift;
    if (nsModifiers & NSControlKeyMask)
        mods |= KeyModifiers_Control;
    if (nsModifiers & NSAlternateKeyMask)
        mods |= KeyModifiers_Alt;
    if (nsModifiers & NSCommandKeyMask)
        mods |= KeyModifiers_Command;

    return mods;
}


- (NSPoint)convertToAppCoordinateSystem:(NSPoint)thePoint {
    // Get the view size in Points
	NSRect viewRectPoints = [self bounds];
    
#if SUPPORT_RETINA_RESOLUTION
    
    // Rendering at retina resolutions will reduce aliasing, but at the potential
    // cost of framerate and battery life due to the GPU needing to render more
    // pixels.
    
    // Any calculations the renderer does which use pixel dimentions, must be
    // in "retina" space.  [NSView convertRectToBacking] converts point sizes
    // to pixel sizes.  Thus the renderer gets the size in pixels, not points,
    // so that it can set it's viewport and perform and other pixel based
    // calculations appropriately.
    // viewRectPixels will be larger (2x) than viewRectPoints for retina displays.
    // viewRectPixels will be the same as viewRectPoints for non-retina displays
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
#else //if !SUPPORT_RETINA_RESOLUTION
    
    // App will typically render faster and use less power rendering at
    // non-retina resolutions since the GPU needs to render less pixels.  There
    // is the cost of more aliasing, but it will be no-worse than on a Mac
    // without a retina display.
    
    // Points:Pixels is always 1:1 when not supporting retina resolutions
    NSRect viewRectPixels = viewRectPoints;
    
#endif // !SUPPORT_RETINA_RESOLUTION
    
	// Set the new dimensions in our renderer
	//[m_renderer resizeWithWidth:viewRectPixels.size.width
    //                  AndHeight:viewRectPixels.size.height];
	
    NSPoint appPoint;
    appPoint.x = thePoint.x;
    appPoint.y = viewRectPixels.size.height - thePoint.y;
    return appPoint;

}

- (void)mouseDragged:(NSEvent *)theEvent {
    NSPoint aPoint = [theEvent locationInWindow];
//    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];
    NSPoint appPoint = [self convertToAppCoordinateSystem:aPoint];
    app.mouseDraggedAt(appPoint.x, appPoint.y);
}

- (void)mouseDown:(NSEvent *)theEvent {
    
    NSPoint aPoint = [theEvent locationInWindow];
//    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];
    
    NSPoint appPoint = [self convertToAppCoordinateSystem:aPoint];
    app.mouseDownAt(appPoint.x, appPoint.y);
}


- (void)mouseUp:(NSEvent *)theEvent {
    NSPoint aPoint = [theEvent locationInWindow];
//    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];
    
    NSPoint appPoint = [self convertToAppCoordinateSystem:aPoint];
    app.mouseUpAt(appPoint.x, appPoint.y);
}

- (void)mouseMoved:(NSEvent *)theEvent {
    NSPoint aPoint = [theEvent locationInWindow];
    //    NSPoint localPoint = [self convertPoint:aPoint fromView:nil];
    
    NSPoint appPoint = [self convertToAppCoordinateSystem:aPoint];
    app.mouseMovedAt(appPoint.x, appPoint.y);
}


- (void)mouseEntered:(NSEvent *)theEvent {
//    wasAcceptingMouseEvents = [[self window] acceptsMouseMovedEvents];
    [[self window] setAcceptsMouseMovedEvents:YES];
//    [[self window] makeFirstResponder:self];
//    NSPoint eyeCenter = [self convertPoint:[theEvent locationInWindow] fromView:nil];
//    eyeBox = NSMakeRect((eyeCenter.x-10.0), (eyeCenter.y-10.0), 20.0, 20.0);
//    [self setNeedsDisplayInRect:eyeBox];
//    [self displayIfNeeded];
}

- (void)mouseExited:(NSEvent *)theEvent {
    [[self window] setAcceptsMouseMovedEvents:YES];
}


- (void) dealloc
{
	// Stop the display link BEFORE releasing anything in the view
    // otherwise the display link thread may call into the view and crash
    // when it encounters something that has been release
	CVDisplayLinkStop(displayLink);
    
	CVDisplayLinkRelease(displayLink);
    
	// Release the display link AFTER display link has been released
	//[m_renderer release];
}

@end
