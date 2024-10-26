//
//
//  Created by Teus Benschop on 26/10/2024.
//

#pragma once

// Why is this Objective-C header file needed?
// The answer is, as mentioned earlier, Swift lacks direct communication with C++.
// It requires an intermediary for communication,
// and in this case, that intermediary is Objective-C++.
// So the order of interaction is:
// 1. Swift
// 2. Objective-C
// 3. Objective-C++
// 4. C++

#import <Foundation/Foundation.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

@interface computer_Cpp_Wrapper : NSObject

- (NSString *)playGame:(NSString *)userSelection;

@end

