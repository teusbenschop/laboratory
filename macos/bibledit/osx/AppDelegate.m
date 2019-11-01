//  Copyright (c) 2015-2017 Teus Benschop. All rights reserved.


#import "AppDelegate.h"
#import <WebKit/WebKit.h>


@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;

@end


@implementation AppDelegate


NSString * searchText = @"";


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSURL *url = [NSURL URLWithString:@"http://bibledit.org:8080"];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
    [self.window setContentView:self.webview];
    [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^(NSEvent *incomingEvent) {
        NSEvent *result = incomingEvent;
        NSWindow *targetWindowForEvent = incomingEvent.window;
        if (targetWindowForEvent == self.window) {
            if (incomingEvent.type == NSEventTypeKeyDown) {
                NSEventModifierFlags modifier = incomingEvent.modifierFlags;
                if (modifier & NSEventModifierFlagCommand) {
                    NSString *characters = incomingEvent.characters;
                    if ([characters isEqualToString:@"f"]) {
                        NSAlert *alert = [[NSAlert alloc] init];
                        alert.messageText = @"Search";
                        [alert addButtonWithTitle:@"OK"];
                        [alert addButtonWithTitle:@"Cancel"];
                        alert.informativeText = [NSString stringWithFormat:@"Search for"];
                        NSTextField *input = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, 200, 24)];
                        [input setStringValue:searchText];
                        [alert setAccessoryView:input];
                        [[alert window] setInitialFirstResponder: input];
                        NSInteger button = [alert runModal];
                        if (button == NSAlertFirstButtonReturn) {
                            searchText = [input stringValue];
                            if (searchText.length) {
                                [self.webview searchFor:searchText direction:TRUE caseSensitive:FALSE wrap:TRUE];
                            } else {
                                [self.webview setSelectedDOMRange:nil affinity:NSSelectionAffinityDownstream];
                            }
                        }
                        result = nil;
                    }
                    if ([characters isEqualToString:@"g"]) {
                        [self.webview searchFor:searchText direction:TRUE caseSensitive:FALSE wrap:TRUE];
                        result = nil;
                    }
                }
            }
        }
        return result;
    }];
}


@end
