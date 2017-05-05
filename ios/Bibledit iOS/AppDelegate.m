//
//  AppDelegate.m
//  Bibledit iOS
//
//  Created by Mini on 29-08-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import "AppDelegate.h"
#import "BibleditController.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [BibleditController appDelegateDidFinishLaunchingWithOptions];
    return YES;
}


- (void)applicationWillResignActive:(UIApplication *)application
{
}


- (void)applicationDidEnterBackground:(UIApplication *)application
{
}


- (void)applicationWillEnterForeground:(UIApplication *)application
{
}


- (void)applicationDidBecomeActive:(UIApplication *)application
{
}


- (void)applicationWillTerminate:(UIApplication *)application
{
}


@end
