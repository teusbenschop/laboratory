//  Copyright (c) 2015-2017 Teus Benschop. All rights reserved.


#import "AppDelegate.h"
#import <WebKit/WebKit.h>


@interface AppDelegate ()

@property (weak) IBOutlet WebView *webview;
@property (weak) IBOutlet NSWindow *window;
- (IBAction)menuPrint:(id)sender;


@end


@implementation AppDelegate


NSString * searchText = @"";
NSTimer * nsTimer;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  NSString *htmlString =
  @"<html>"
  "<head>"
  "<style>"
  ".center-screen {"
  " display: flex;"
  " flex-direction: column;"
  " justify-content: center;"
  " align-items: center;"
  " text-align: center;"
  " min-height: 100vh;"
  " background: radial-gradient(gold, yellow, white);"
  "}"
  "</style>"
  "</head>"
  "<body>"
  "<h1 class=\"center-screen\">... Bibledit loading ...</h1>"
  "</body>"
  "</html>";
  [[[self webview] mainFrame] loadHTMLString:htmlString baseURL:nil];
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
          if ([characters isEqualToString:@"p"]) {
            [self print:nil];
            result = nil;
          }
        }
      }
    }
    return result;
  }];

  nsTimer = [NSTimer
             scheduledTimerWithTimeInterval:1.0
             target:self
             selector:@selector(timerTimeout)
             userInfo:nil
             repeats:YES];
}


- (IBAction)menuPrint:(id)sender {
  [self print:sender];
}

- (void)print:(id)sender
{
  //  NSLog(@"print start");
  
  // This method works with simple web pages without AJAX or special CSS effect.
  // But it already fails on a basic page like the Bibledit home page.
  // https://developer.apple.com/documentation/webkit/webview
  // self.webview.shouldUpdateWhileOffscreen = true;
  NSPrintInfo *printInfo = [NSPrintInfo sharedPrintInfo];
  NSPrintOperation *printOperation = [NSPrintOperation printOperationWithView: self.webview.mainFrame.frameView.documentView printInfo: printInfo];
  [printOperation setShowsPrintPanel: YES];
  [printOperation runOperation];
  
  // Do screen capture of the Bibledit window.
  // https://stackoverflow.com/questions/48030214/capture-screenshot-of-macos-window
  
  // Get a list of all the windows available on the system.
  // NSArray<NSDictionary*> *windowInfoList = (__bridge_transfer id)
  // CGWindowListCopyWindowInfo(kCGWindowListOptionAll, kCGNullWindowID);
  
  // Get the process ID of Bibledit.
  // NSArray<NSRunningApplication*> *running_apps = [NSRunningApplication runningApplicationsWithBundleIdentifier: @"nl.teus.test"];
  //if (running_apps.count == 0) {
  // return;
  // }
  // pid_t app_pid = running_apps[0].processIdentifier;
  // NSLog(@"App pid is %d", app_pid);
  
  // Filter down the window info list to only windows with a matching owner PID.
  //  NSMutableArray<NSDictionary*> *appWindowsInfoList = [NSMutableArray new];
  //  for (NSDictionary *info in windowInfoList) {
  //      if ([info[(__bridge NSString *)kCGWindowOwnerPID] integerValue] == app_pid) {
  //          [appWindowsInfoList addObject:info];
  //      }
  //  }
  
  // Use the first window in the list.
  //  NSDictionary *appWindowInfo = appWindowsInfoList[0];
  //  CGWindowID windowID = [appWindowInfo[(__bridge NSString *)kCGWindowNumber] unsignedIntValue];
  //  NSLog (@"window ID %d", windowID);
  
  // Do the screen capture of the window.
  //  CGImageRef windowImage = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionOnScreenOnly, windowID, kCGWindowImageBoundsIgnoreFraming | kCGWindowImageNominalResolution);
  
  //  NSLog (@"windowImage %d", windowImage);
  
  //  NSString * path = @"/Users/teus/Library/Containers/nl.teus.test/Data/Documents/screenshot.png";
  //  CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:path];
  //  CGImageDestinationRef destination = CGImageDestinationCreateWithURL(url, kUTTypePNG, 1, NULL);
  //  CGImageDestinationAddImage(destination, windowImage, nil);
  //  CFRelease(destination);
  
  //  NSLog(@"print ready");
}


bool kernel_ready = false;

- (void)timerTimeout
{
  NSURL *url = [NSURL URLWithString:@"http://localhost:8080"];
  NSURLSessionConfiguration *conf = [NSURLSessionConfiguration defaultSessionConfiguration];
  NSURLSession *sessionWithoutADelegate = [NSURLSession sessionWithConfiguration:conf];
  NSURLSessionDataTask * task = [sessionWithoutADelegate dataTaskWithURL:url completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
    if (data) {
      //NSString* returnValue = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
      kernel_ready = true;
    }
    [sessionWithoutADelegate finishTasksAndInvalidate];
  }];
  if (kernel_ready) {
    [nsTimer invalidate];
    nsTimer = nil;
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [[[self webview] mainFrame] loadRequest:urlRequest];
  }
  [task resume];
}


@end
