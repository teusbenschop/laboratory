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


@implementation BibleditController





/*
- (void) webView:(WebView *)webView decidePolicyForMIMEType:(NSString *)type request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id < WebPolicyDecisionListener >)listener
{
    if (![[webView class] canShowMIMEType:type]) [listener download];
}


- (void)download:(NSURLDownload *)download decideDestinationWithSuggestedFilename:(NSString *)filename
{
    NSString * directory = @"/tmp";
    struct passwd *pw = getpwuid(getuid());
    if (pw->pw_dir) directory = [NSString stringWithUTF8String:pw->pw_dir];
    if (filename == nil) filename = @"bibledit-download";
    NSString *destinationPath = [NSString stringWithFormat:@"%@/%@/%@", directory, @"Downloads", filename];
    NSAlert *alert = [[NSAlert alloc] init];
    alert.messageText = @"Download";
    [alert addButtonWithTitle:@"OK"];
    alert.informativeText = [NSString stringWithFormat:@"Will be saved to: %@", destinationPath];
    [alert runModal];
    [download setDestination:destinationPath allowOverwrite:YES];
}


- (void)webView:(WebView *)sender runOpenPanelForFileButtonWithResultListener:(id < WebOpenPanelResultListener >)resultListener
{
    // Create the file open dialog.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    // Enable the selection of files in the dialog.
    [openDlg setCanChooseFiles:YES];
    
    // Enable the selection of directories in the dialog.
    [openDlg setCanChooseDirectories:NO];
    
    // Run it.
    if ( [openDlg runModal] == NSModalResponseOK )
    {
        NSArray* files = [[openDlg URLs]valueForKey:@"relativePath"];
        [resultListener chooseFilenames:files];
    }
}
 */




@end
