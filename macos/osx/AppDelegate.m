//
//  AppDelegate.m
//
//  Copyright (c) 2015-2017 Teus Benschop. All rights reserved.
//

#import "AppDelegate.h"
#import <WebKit/WebKit.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <assert.h>

@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;

@end


@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSURL *url = [NSURL URLWithString:@"http://bibledit.org:8080"];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
    [self.window setContentView:self.webview];
    [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(timerTimeout) userInfo:nil repeats:YES];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    NSLog (@"terminate");
}


-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}


- (void)timerTimeout
{
    NSLog (@"timer");
}


@end
