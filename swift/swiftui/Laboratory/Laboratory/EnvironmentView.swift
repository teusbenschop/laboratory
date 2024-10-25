//
//  EnvironmentView.swift
//  Laboratory
//
//  Created by Teus Benschop on 27/10/2019.
//  Copyright © 2019 Free Software Consultants. All rights reserved.
//

import SwiftUI

struct EnvironmentView: View {
    var body: some View {
      DynamicButtonsView()
        .frame(maxWidth: .infinity, maxHeight: .infinity)
        // Every View inside SwiftUI inherits the Environment from its parent view.
        // You can override values while creating the child view.
        // See this example:
        .environment(\.layoutDirection, .leftToRight)
    }
}


struct SimpleButtonsView: View {
  
  @Environment(\.presentationMode) var presentation
  
  var body: some View {
    
    Group {
      Button("previous") {
      }
      Button("play") {
      }
      Button("next") {
      }
      Button("print") {
        // You may update values in the Environment in the way displayed here.
        //self.presentation.projectedValue = true
        print (self.presentation.projectedValue)
      }
    }
  }
}



struct DynamicButtonsView: View {

  // By using the @Environment property wrapper,
  // we can read/write and subscribe to changes in the selected value.
  // Here we have a View which reads a Dynamic Type value from Environment
  // and puts buttons in VStack or HStack depending on the size category value.
  // The user can change the Dynamic Type value in the system settings,
  // and as soon as that happens,
  // SwiftUI will recreate the View to respect the changes.
  @Environment(\.sizeCategory) var sizeCategory
  
  var body: some View {
    Group {
      if sizeCategory == .accessibilityExtraExtraExtraLarge {
        VStack {
          SimpleButtonsView()
        }
      } else {
        HStack {
          SimpleButtonsView()
        }
      }
    }
  }
}

struct EnvironmentView_Previews: PreviewProvider {
    static var previews: some View {
        EnvironmentView()
    }
}
