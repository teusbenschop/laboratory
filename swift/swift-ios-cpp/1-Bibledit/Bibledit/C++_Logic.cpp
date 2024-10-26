//
//  C++_Logic.cpp
//  Bibledit
//
//  Created by Teus Benschop on 25/10/2024.
//

// This C++ file was added to the project via creating a new file from template,
// choosing C++ file, including header, and adding it to the project.
// When Xcode 15 asks "Would you like to configure an Objective-C bridging header?"
// choose "Create Bridging Header".
// This will automatically set up a bridging header,
// facilitating access to classes between swift and Objective-C.

#include "C++_Logic.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

namespace localFunctions {

std::string compSelection[] = {"stone", "paper", "scissor"};

std::string randomComp() {
    return compSelection[rand() % 3];
}
}

// MARK: - Logic

std::string Computer::playComputer(std::string userSelection) {
    std::transform(userSelection.begin(), userSelection.end(), userSelection.begin(), ::tolower);
    std::string compSelectionSingle = localFunctions::randomComp();
    std::string result;
    
    if (userSelection == compSelectionSingle) {
        result = "Oh dear! You both made the same guess; try again!";
    } else if ((userSelection == "scissor" && compSelectionSingle == "paper") ||
               (userSelection == "paper" && compSelectionSingle == "stone") ||
               (userSelection == "stone" && compSelectionSingle == "scissor")) {
        result = "We regret. This time, computer won.";
    } else if ((userSelection == "scissor" && compSelectionSingle == "stone") ||
               (userSelection == "paper" && compSelectionSingle == "scissor") ||
               (userSelection == "stone" && compSelectionSingle == "paper")) {
        result = "You win for having the right guess.";
    } else {
        result = "";
    }
    
    return result;
}
