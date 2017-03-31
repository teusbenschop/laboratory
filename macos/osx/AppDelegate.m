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
}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    NSLog (@"terminate");
}


-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}


/*
    WebFrame *frame = [self.webview mainFrame];
    NSString * url = [[[[frame dataSource] request] URL] absoluteString];
    NSLog (@"%@", url);
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString: url]];
*/


@end
