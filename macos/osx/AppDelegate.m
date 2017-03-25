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


/*
- (void) webView:(WebView *)webView decidePolicyForNavigationAction:(NSDictionary *)actionInformation
         request:(NSURLRequest *)request
           frame:(WebFrame *)frame
decisionListener:(id < WebPolicyDecisionListener >)listener
{
    NSLog(@"navigating from %@ to: %@", [webView mainFrameURL], [[request URL] absoluteString]);
    int actionKey = [[actionInformation objectForKey: WebActionNavigationTypeKey] intValue];
    if (actionKey == WebNavigationTypeOther)
    {
        [listener use];
        //[listener download];
    }
    else
    {
        [listener use];
    }
    NSLog (@"can handle %d", [NSURLConnection canHandleRequest:request]);
}
*/

- (void)        webView:(WebView *)webView
decidePolicyForMIMEType:(NSString *)type
                request:(NSURLRequest *)request
                  frame:(WebFrame *)frame
       decisionListener:(id < WebPolicyDecisionListener >)listener
{
    NSLog (@"can view %d", [[webView class] canShowMIMEType:type]);
    if (![[webView class] canShowMIMEType:type])
    {
        [listener download];
        NSString * url = [[request URL] absoluteString];
        [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString: url]];
    }
    
    
}



@end
