//
//  BibleditPaths.h
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>


extern WKWebView *webview;
extern NSString * homeUrl;


@interface BibleditController : NSObject


+ (void) appDelegateDidFinishLaunchingWithOptions;
+ (void) viewControllerViewDidLoad:(UIView *)view;



@end
