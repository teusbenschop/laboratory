//
//  ViewController.swift
//  Bibledit
//
//  Created by Teus Benschop on 25/10/2024.
//


import UIKit

class ViewController: UIViewController {

    // Initializing an instance of the C++ class.
    private let computer = computer_Cpp_Wrapper()
    
    private var userSelected : String = "Stone"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        let result = computer.playGame(userSelected)        // Invoking a method with arguments
        print("Result is: ", result as Any)
    }


}

