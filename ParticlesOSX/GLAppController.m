//
//  GLAppController.m
//  ParticlesOSX
//

#import "GLAppController.h"

@interface GLAppController ()

- (void)createOpenGLView;

- (void)createDisplayLink;

- (void)createOpenGLResources;

- (void)renderForTime:(CVTimeStamp)time;

@end

CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext);

CVReturn displayCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *inNow, const CVTimeStamp *inOutputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext)
{
    GLAppController *controller = (__bridge GLAppController *)displayLinkContext;
    [controller renderForTime:*inOutputTime];
    return kCVReturnSuccess;
}

@implementation GLAppController
{
    CVDisplayLinkRef displayLink;
}

@synthesize view;
@synthesize window;

- (void)awakeFromNib
{
    [self createOpenGLView];
    [self createOpenGLResources];
    [self createDisplayLink];
}

- (void)createOpenGLView
{
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize    , 24                           ,
        NSOpenGLPFAAlphaSize    , 8                            ,
        NSOpenGLPFADoubleBuffer ,
        NSOpenGLPFAAccelerated  ,
        NSOpenGLPFANoRecovery   ,
        0
    };
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    [self setView:[[NSOpenGLView alloc] initWithFrame:[[[self window] contentView] bounds] pixelFormat:pixelFormat] ];
    [[[self window] contentView] addSubview:[self view]];
}

- (void)createDisplayLink
{
    CGDirectDisplayID displayID = CGMainDisplayID();
    CVReturn error = CVDisplayLinkCreateWithCGDisplay(displayID, &displayLink);
    
    if (kCVReturnSuccess == error)
    {
        CVDisplayLinkSetOutputCallback(displayLink, displayCallback, (__bridge void *)(self));
        CVDisplayLinkStart(displayLink);
    }
    else
    {
        NSLog(@"Display Link created with error: %d", error);
        displayLink = NULL;
    }
}

- (void)createOpenGLResources
{
    [[[self view] openGLContext] makeCurrentContext];
}

- (void)renderForTime:(CVTimeStamp)time
{
    [[[self view] openGLContext] makeCurrentContext];
    
    // GL Code here
    
    [[[self view] openGLContext] flushBuffer];
}

- (void)dealloc
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    // [super dealloc];
}

@end
