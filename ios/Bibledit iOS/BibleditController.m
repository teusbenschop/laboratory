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
#import "BibleditPaths.h"
#import "BibleditInstallation.h"


@implementation BibleditController


UIView * mainUIView = NULL;
WKWebView *webview;
NSString * homeUrl = @"http://bibledit.org:8080";


+ (void) bibleditViewHasLoaded:(UIView *)uiview
{
    mainUIView = uiview;
    WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
    webview = [[WKWebView alloc] initWithFrame:mainUIView.frame configuration:configuration];
    [mainUIView addSubview:webview];
    NSLog(@"To URL %@", homeUrl);
    NSURL *url = [NSURL URLWithString:homeUrl];
    NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
    [webview loadRequest:urlRequest];
}


@end
