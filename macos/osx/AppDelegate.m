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
@property (weak) IBOutlet NSMenu *menu;
@property (weak) IBOutlet NSMenuItem *viewmenu;
- (IBAction)Safari:(id)sender;

@end


@implementation AppDelegate


- (void) windowDidResize:(NSNotification *) notification
{
    NSSize size = self.window.contentView.frame.size;
    [[self webview] setFrame:CGRectMake(0, 0, size.width, size.height)];
}


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSURL *url = [NSURL URLWithString:@"https://bibledit.org:8081"];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
    [self.window setContentView:self.webview];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowDidResize:) name:NSWindowDidResizeNotification object:self.window];
    
    [self.webview setPolicyDelegate:self];
    [self.webview setDownloadDelegate:self];

}


- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    NSLog (@"terminate");
}


-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app
{
    return YES;
}


- (IBAction)Safari:(id)sender {
    WebFrame *frame = [self.webview mainFrame];
    NSString * url = [[[[frame dataSource] request] URL] absoluteString];
    NSLog (@"%@", url);
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString: url]];
}


- (void) webView:(WebView *)webView decidePolicyForMIMEType:(NSString *)type request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id < WebPolicyDecisionListener >)listener
{
    if (![[webView class] canShowMIMEType:type]) [listener download];
}


- (void)download:(NSURLDownload *)download decideDestinationWithSuggestedFilename:(NSString *)filename
{
    NSString * directory = @"/tmp";
    struct passwd *pw = getpwuid(getuid());
    if (pw->pw_dir) directory = [NSString stringWithUTF8String:pw->pw_dir];
    if (filename == nil) filename = @"bibledit-download";
    NSString *destinationPath = [NSString stringWithFormat:@"%@/%@/%@", directory, @"Downloads", filename];
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Download";
    [alert addButtonWithTitle:@"OK"];
    alert.informativeText = [NSString stringWithFormat:@"Will be saved to: %@", destinationPath];
    [alert runModal];
    [download setDestination:destinationPath allowOverwrite:YES];
}


- (void)webView:(WebView *)sender runOpenPanelForFileButtonWithResultListener:(id < WebOpenPanelResultListener >)resultListener
{
    // Create the File Open Dialog class.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    // Enable the selection of files in the dialog.
    [openDlg setCanChooseFiles:YES];
    
    // Enable the selection of directories in the dialog.
    [openDlg setCanChooseDirectories:NO];
    
    
    if ( [openDlg runModal] == NSModalResponseOK )
    {
        NSArray* files = [[openDlg URLs]valueForKey:@"relativePath"];
        [resultListener chooseFilenames:files];
    }
}


@end
