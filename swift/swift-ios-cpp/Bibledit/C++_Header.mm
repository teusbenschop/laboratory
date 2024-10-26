//
//
//  Created by Teus Benschop on 26/10/2024.
//

// Add a new file, from template, to the project.
// Choose Objective-C-File.
// This creates a "filename.m".
// Change the suffix to ".mm" so it implies an Objective-C++ file.

#import "C++_Header.h"
#import "C++_Logic.h"

Computer ComputerObject;

@implementation computer_Cpp_Wrapper

- (NSString *)playGame:(NSString *)userSelection;
{
    const std::string selectionOfUser = [userSelection UTF8String];
    std::string result = ComputerObject.playComputer(selectionOfUser);
    
    NSString *resultString = [NSString stringWithUTF8String:result.c_str()]; //Convert to NSString
    
    return resultString;
}

@end

