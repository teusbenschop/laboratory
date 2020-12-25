//
//  accordance.m
//  Bibledit
//
//  Created by Teus Benschop on 25/12/2020.
//  Copyright © 2020 Teus Benschop. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "accordance.h"



@implementation Accordance

@synthesize height;

-(id)init {
   self = [super init];
   length = 1.0;
   breadth = 1.0;
   return self;
}

-(double) volume {
   return length*breadth*height;
}


- (void)send:(NSString*)reference
{
  NSLog(@"send start");
  [[NSDistributedNotificationCenter defaultCenter] postNotificationName:@"com.santafemac.scrolledToVerse" object:reference userInfo:nil deliverImmediately:YES];
  NSLog(@"send ready");
}



@end

