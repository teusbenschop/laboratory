//
//  BibleditPaths.m
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import "BibleditController.h"


UIView * uiview;
WKWebView *webview;
NSString * homeUrl = @"http://bibledit.org:8080";


@implementation BibleditController


+ (void) appDelegateDidFinishLaunchingWithOptions
{
    NSLog(@"appDelegateDidFinishLaunchingWithOptions");
    [NSTimer scheduledTimerWithTimeInterval:1.0
                                     target:self
                                   selector:@selector(runRepetitiveTimer:)
                                   userInfo:nil
                                    repeats:YES];
}


+ (void) viewControllerViewDidLoad:(UIView *)view
{
    uiview = view;
}


+ (void) runRepetitiveTimer:(NSTimer *)timer
{
    NSLog(@"Timer");
    /*
    if (webview != NULL) {
        [[self.view subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
        webview = NULL;
        NSArray * urls;
        NSLog(@"Tabbed view@");
        [self startTabbedView:urls];
    } else {
        [[self.view subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
        NSLog(@"Plain view %@", homeUrl);
        [self startPlainView:homeUrl];
    }
     */
}


@end
