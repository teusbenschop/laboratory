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
NSString * homeUrl = @"https://bibledit.org:8081/";
Boolean plainViewActive = false;


@implementation BibleditController


+ (void) appDelegateDidFinishLaunchingWithOptions
{
    NSLog(@"appDelegateDidFinishLaunchingWithOptions");
    [NSTimer scheduledTimerWithTimeInterval:4.0
                                     target:self
                                   selector:@selector(runRepetitiveTimer:)
                                   userInfo:nil
                                    repeats:YES];
}


+ (void) viewControllerViewDidLoad:(UIView *)view
{
    NSLog(@"plain view loaded");
    uiview = view;
    [self startPlainView:homeUrl];
}


+ (void) tabBarControllerViewDidLoad:(UIView *)view
{
    NSLog(@"tabbed view loaded");
    uiview = view;
    NSArray * urls = @[@"", @"editone/index", @"notes/index", @"resource/index"];
    NSArray * labels = @[@"Home", @"Translate", @"Notes", @"Resources"];
    NSInteger active = 1;
    [self startTabbedView:urls labels:labels active:active];
}


+ (void) runRepetitiveTimer:(NSTimer *)timer
{
    return; // Todo
    NSString * boardName;
    if (plainViewActive) {
        boardName = @"Tabbed";
    } else {
        boardName = @"Plain";
    }
    UIStoryboard *storyBoard = [UIStoryboard storyboardWithName:boardName bundle:nil];
    UIViewController *initViewController = [storyBoard instantiateInitialViewController];
    [uiview.window setRootViewController:initViewController];
}


+ (void) startPlainView:(NSString *)url
{
    [[uiview subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];
    
    WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
    webview = [[WKWebView alloc] initWithFrame:uiview.frame configuration:configuration];
    [uiview addSubview:webview];
    
    NSURL *nsurl = [NSURL URLWithString:url];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:nsurl];
    [webview loadRequest:urlRequest];
    
    plainViewActive = true;
}


+ (void) startTabbedView:(NSArray *)urls labels:(NSArray *)labels active:(NSInteger)active
{
    [[uiview subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];

    UITabBarController * tabBarController = [[UITabBarController alloc] init];

    NSMutableArray * controllers = [[NSMutableArray alloc] init];

    for (int i = 0; i < [urls count]; i++) {
        
        NSString * url = [urls objectAtIndex:i];
        NSString * label = [labels objectAtIndex:i];
        
        UIViewController* viewController = [[UIViewController alloc] init];
        UIImage* image = [UIImage imageNamed:@"home.png"];
        UITabBarItem* tarBarItem = [[UITabBarItem alloc] initWithTitle:label image:image tag:0];
        viewController.tabBarItem = tarBarItem;
        
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        WKWebView * webview = [[WKWebView alloc] initWithFrame:viewController.view.frame configuration:configuration];
        [viewController.view addSubview:webview];
        
        NSMutableString* fullUrl = [[NSMutableString alloc] init];
        [fullUrl appendString:homeUrl];
        [fullUrl appendString:url];
        NSURL *nsurl = [NSURL URLWithString:fullUrl];
        NSURLRequest *urlRequest = [NSURLRequest requestWithURL:nsurl];
        [webview loadRequest:urlRequest];
        
        [controllers addObject:viewController];
    }
    
    tabBarController.viewControllers = controllers;
    
    [uiview addSubview:tabBarController.view];
    
    tabBarController.selectedIndex = 1;
    
    plainViewActive = false;
}


@end


/*
IconBeast Lite | 300 Free iOS Tab Bar Icons for iPhone and iPad
---------------------------------------------------------------

Thank you for downloading IconBeast Lite.

IconBeast Lite is a strip-down version of IconBeast Pro ($75). This 300 icons is free for download and is published under Creative Commons Attribution license. You can use these icons in your all your projects, but we required you to credit us by including a http link back to IconBeast website.

You are also allowed to distribute IconBeast Lite to your website, but you must mentioned that this icon set came from IconBeast. You must also put a link back to us in your website.

You are not allowed to sell these icons.
*/
