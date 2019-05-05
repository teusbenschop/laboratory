//
//  ViewController.swift
//  HelloWorld
//
//  Created by Teus Benschop on 05/05/2019.
//  Copyright © 2019 Teus Benschop. All rights reserved.
//

import Cocoa

class ViewController: NSViewController {

  @IBOutlet weak var textfield: NSTextField!
  @IBOutlet weak var theButton: NSButton!

  @IBOutlet weak var theLabel2: NSTextField!
  
  override func viewDidLoad() {
    super.viewDidLoad()

    // Do any additional setup after loading the view.
  }

  override var representedObject: Any? {
    didSet {
    // Update the view, if already loaded.
    }
  }
  
  @IBAction func theButtonClick(_ sender: Any) {
    var name = textfield.stringValue
    print (name)
    if name.isEmpty {
      name = "World"
    }
    let greeting = "Hello" + name
    print (greeting)
    theLabel2.stringValue = greeting
  }


}

