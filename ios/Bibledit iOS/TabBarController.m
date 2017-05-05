//
//  TabBarController.m
//  Bibledit
//
//  Created by Teus Benschop on 05/05/2017.
//  Copyright © 2017 Teus Benschop. All rights reserved.
//

#import "TabBarController.h"
#import "BibleditController.h"


@interface TabBarController ()

@end

@implementation TabBarController


- (void)viewDidLoad {
    [super viewDidLoad];
    [BibleditController tabBarControllerViewDidLoad:self];
}


@end
