#import "AppDelegate.h"
#import <WebKit/WebKit.h>

@interface AppDelegate ()

@property (unsafe_unretained) IBOutlet NSWindow *window;
@property (unsafe_unretained) IBOutlet WKWebView *wk_web_view;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSString* string = @"http://bibledit.org:8090/workspace/index?bench=1";
    NSURL* url = [NSURL URLWithString:string];
    NSURLRequest* url_request = [NSURLRequest requestWithURL:url];
    self.wk_web_view = [[WKWebView alloc] initWithFrame: CGRectZero];
    [self.wk_web_view loadRequest: url_request];
    [self.window setContentView:self.wk_web_view];
}

@end
