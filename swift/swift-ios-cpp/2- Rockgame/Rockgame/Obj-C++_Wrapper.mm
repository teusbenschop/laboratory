//
//  Obj-C++_Wrapper.m
//  Rockgame
//
//  Created by Aniket Bane on 12/02/24.
//

#import <Foundation/Foundation.h>
#import "C++_Logic.hpp"
#import "C++_Header.h"

Computer ComputerObject;

@implementation ComputerCppWrapper

- (NSString *)playGame:(NSString *)userSelection;
{
    const std::string selectionOfUser = [userSelection UTF8String];
    const std::string result = ComputerObject.play_computer(selectionOfUser);
 
    NSString *resultString = [NSString stringWithUTF8String:result.c_str()]; //Convert to NSString
    
    return resultString;
}

@end

