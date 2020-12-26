//  Copyright (c) 2015-2017 Teus Benschop. All rights reserved.


#import "AppDelegate.h"
#import <WebKit/WebKit.h>


@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;


@end


@implementation AppDelegate


- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSURL *url = [NSURL URLWithString:@"https://bibledit.org:8081"];
  NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
  [[[self webview] mainFrame] loadRequest:urlRequest];
  [self.window setContentView:self.webview];
  
  [[NSDistributedNotificationCenter defaultCenter] addObserver:self selector:@selector(accordanceDidScroll:) name:@"com.santafemac.scrolledToVerse" object:nil suspensionBehavior:NSNotificationSuspensionBehaviorCoalesce];

  
  [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^(NSEvent *incomingEvent) {
    NSEvent *result = incomingEvent;
    NSWindow *targetWindowForEvent = incomingEvent.window;
    if (targetWindowForEvent == self.window) {
      if (incomingEvent.type == NSEventTypeKeyDown) {
        NSEventModifierFlags modifier = incomingEvent.modifierFlags;
        if (modifier & NSEventModifierFlagCommand) {
          NSString *characters = incomingEvent.characters;
          if ([characters isEqualToString:@"s"]) {
            NSString * reference = @"2CO 9:2";
            [[NSDistributedNotificationCenter defaultCenter] postNotificationName:@"com.santafemac.scrolledToVerse" object:reference userInfo:nil deliverImmediately:YES];
          }
          if ([characters isEqualToString:@"r"]) {
          }
        }
      }
    }
    return result;
  }];
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
  
  [[NSDistributedNotificationCenter defaultCenter] removeObserver:self name:@"com.santafemac.scrolledToVerse" object:nil];

}


-(void)accordanceDidScroll:(NSNotification *)notification {
  NSLog(@"%@", notification.object);
  NSString * reference = notification.object;
  NSLog(reference);
}


@end
