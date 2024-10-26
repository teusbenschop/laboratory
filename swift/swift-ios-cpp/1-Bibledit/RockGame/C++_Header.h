//
//  C++_Header.h
//  Rockgame
//
//  Created by Aniket Bane on 12/02/24.
//

// MARK: - Interface

#import <Foundation/Foundation.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


@interface ComputerCppWrapper : NSObject

- (NSString *)playGame:(NSString *)userSelection;

@end
