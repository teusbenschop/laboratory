//
//  ContentView.swift
//  Bibledit
//
//  Created by Teus Benschop on 26/10/2024.
//

import SwiftUI


struct ContentView: View {
  
    let from_cpp = String(cString: jesus_saves())
    
    var body: some View
    {
        VStack
        {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundStyle(.tint)
            Text(from_cpp)
        }
        .padding()
    }
}

#Preview {
    ContentView()
}
