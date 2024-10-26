//
//  C++_Logic.cpp
//  Rockgame
//
//  Created by Aniket Bane on 11/02/24.
//

#include "C++_Logic.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>


namespace local_functions {
const std::string computer_selection[] = {"stone", "paper", "scissor"};

std::string random_computer() {
    return computer_selection[rand() % 3];
}
}

std::string Computer::play_computer(std::string user_selection) {
    std::transform(user_selection.begin(), user_selection.end(), user_selection.begin(), ::tolower);
    const std::string computer_selection {local_functions::random_computer()};
    std::string result {};

    if (user_selection == computer_selection) {
        result = "Oh dear! You both made the same guess; try again!";
    }
    if ((user_selection == "scissor" && computer_selection == "paper") ||
        (user_selection == "paper" && computer_selection == "stone") ||
        (user_selection == "stone" && computer_selection == "scissor")) {
        result = "We regret. This time, computer won.";
    }
    if ((user_selection == "scissor" && computer_selection == "stone") ||
        (user_selection == "paper" && computer_selection == "scissor") ||
        (user_selection == "stone" && computer_selection == "paper")) {
        result = "You win for having the right guess.";
    }

    return result;
}
