//
//  GLAppController.h
//  ParticlesOSX
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl3.h>
#import <CoreVideo/CVDisplayLink.h>

#define kFailedToInitialiseGLException @"Failed to initialise OpenGL"


@interface GLAppController : NSObject

@property (nonatomic, readwrite, retain) IBOutlet NSWindow *window;
@property (nonatomic, readwrite, retain) IBOutlet NSOpenGLView *view;

@end
