//
//  accordance.h
//  Bibledit
//
//  Created by Teus Benschop on 25/12/2020.
//  Copyright © 2020 Teus Benschop. All rights reserved.
//

#ifndef accordance_h
#define accordance_h


#import <Foundation/Foundation.h>

@interface Accordance:NSObject {
   double length;    // Length of a box
   double breadth;   // Breadth of a box
   double height;    // Height of a box
}

@property(nonatomic, readwrite) double height;  // Property
- (double) volume;
- (void)send:(NSString*)reference;


@end

#endif /* accordance_h */
