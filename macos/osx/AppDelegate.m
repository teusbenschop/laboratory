//
//  AppDelegate.m
//
//  Copyright (c) 2015-2017 Teus Benschop. All rights reserved.
//

#import "AppDelegate.h"
#import <WebKit/WebKit.h>

@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;

@end


@implementation AppDelegate


- (void) windowDidResize:(NSNotification *) notification
{
    NSSize size = self.window.contentView.frame.size;
    [[self webview] setFrame:CGRectMake(0, 0, size.width, size.height)];
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSURL *url = [NSURL URLWithString:@"http://localhost:8080"];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
    [self.window setContentView:self.webview];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowDidResize:) name:NSWindowDidResizeNotification object:self.window];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    NSLog (@"terminate");
}


-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}

@end
