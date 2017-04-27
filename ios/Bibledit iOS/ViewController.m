/*
 Copyright (©) 2003-2014 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#import "ViewController.h"
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>
#import <Foundation/Foundation.h>


@interface ViewController ()

@end

@implementation ViewController

UIView * mainUIView = NULL;
WKWebView *webview;
NSString * homeUrl = @"http://bibledit.org:8080";


- (void)viewDidLoad
{
  [super viewDidLoad];
  mainUIView = self.view;
  WKWebViewConfiguration *configuration = [[WKWebViewConfiguration alloc] init];
  webview = [[WKWebView alloc] initWithFrame:mainUIView.frame configuration:configuration];
  [mainUIView addSubview:webview];
  NSLog(@"To URL %@", homeUrl);
  NSURL *url = [NSURL URLWithString:homeUrl];
  NSURLRequest *urlRequest = [NSURLRequest requestWithURL:url];
  [webview loadRequest:urlRequest];
}


- (BOOL) prefersStatusBarHidden
{
  return YES;
}


@end
