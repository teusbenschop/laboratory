#import "AppDelegate.h"
#import <WebKit/WebKit.h>

@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSString* string = @"http://bibledit.org:8090/workspace/index?bench=1";
    NSURL* url = [NSURL URLWithString:string];
    NSURLRequest* url_request = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:url_request];
    [self.window setContentView:self.webview];
}

@end
