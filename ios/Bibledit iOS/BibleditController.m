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
    NSLog(@"viewControllerViewDidLoad");
    uiview = view;
    [self startPlainView:homeUrl];
}


+ (void) runRepetitiveTimer:(NSTimer *)timer
{
    if (webview != nil) {
        NSLog(@"Tabbed view");
        webview = nil;
        NSArray * urls = @[@"", @"editone/index", @"notes/index", @"resource/index"];
        NSArray * labels = @[@"Home", @"Translate", @"Notes", @"Resources"];
        NSInteger active = 1;
        [self startTabbedView:urls
                       labels:labels
                       active:active];
    } else {
        //NSLog(@"Plain view");
        //[self startPlainView:homeUrl];
    }
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
}


+ (void) startTabbedView:(NSArray *)urls labels:(NSArray *)labels active:(NSInteger)active
{
    // https://developer.apple.com/library/content/documentation/WindowsViews/Conceptual/ViewControllerCatalog/Chapters/TabBarControllers.html

    [[uiview subviews] makeObjectsPerformSelector:@selector(removeFromSuperview)];

    UITabBarController * tabBarController = [[UITabBarController alloc] init];
    
    UIViewController* viewController1 = [[UIViewController alloc] init];
    {
        UIImage* image = [UIImage imageNamed:@"home.png"];
        UITabBarItem* tarBarItem = [[UITabBarItem alloc] initWithTitle:@"Resources" image:image tag:0];
        viewController1.tabBarItem = tarBarItem;

        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        WKWebView * webview = [[WKWebView alloc] initWithFrame:viewController1.view.frame configuration:configuration];
        [viewController1.view addSubview:webview];
        
        NSMutableString* url = [[NSMutableString alloc] init];
        [url appendString:homeUrl];
        [url appendString:@"resource/index"];
        NSURL *nsurl = [NSURL URLWithString:url];
        NSURLRequest *urlRequest = [NSURLRequest requestWithURL:nsurl];
        [webview loadRequest:urlRequest];
    }
    
    UIViewController* viewController2 = [[UIViewController alloc] init];
    {
        UIImage* image = [UIImage imageNamed:@"briefcase.png"];
        UITabBarItem* tarBarItem = [[UITabBarItem alloc] initWithTitle:@"Translate" image:image tag:0];
        viewController2.tabBarItem = tarBarItem;
        
        WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
        WKWebView * webview = [[WKWebView alloc] initWithFrame:viewController2.view.frame configuration:configuration];
        [viewController2.view addSubview:webview];
        
        NSMutableString* url = [[NSMutableString alloc] init];
        [url appendString:homeUrl];
        [url appendString:@"editone/index"];
        NSURL *nsurl = [NSURL URLWithString:url];
        NSURLRequest *urlRequest = [NSURLRequest requestWithURL:nsurl];
        [webview loadRequest:urlRequest];
    }
    
    NSArray * controllers = [[NSArray alloc] init];
    controllers = [NSArray arrayWithObjects:viewController1, viewController2, nil];
    tabBarController.viewControllers = controllers;
    
    [uiview addSubview:tabBarController.view];
    
    tabBarController.selectedIndex = 1;
    
    tabBarController.delegate = self;
}


-(void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController {
    NSLog(@"tabBarController");
}


- (void)tabBar:(UITabBar *)tabBar didSelectItem:(UITabBarItem *)item
{
    NSLog(@"tabBar");
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
