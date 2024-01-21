/*
 Copyright (©) 2003-2024 Teus Benschop.
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

import SwiftUI
import CxxStdlib

func test_one ()
{
        let s: std.string = "Hello C++ world!"
        print(s)
        let swiftString = "This is " + "a Swift string"
        let cxxString = std.string(swiftString)
        print (cxxString)
}

func test_two ()
{
    let cxxString = std.string("This is a C++ string")
    let swiftString = String(cxxString)
    print (swiftString)
}

struct ContentView: View {
    var body: some View {
        VStack {
            Image(systemName: "globe")
                .imageScale(.large)
                .foregroundStyle(.tint)
            Text("Hello, world!")
        }
        .padding()
        .onAppear() {
            print("The VStack appeared")
            test_one()
            test_two()
        }
        .onDisappear() {
            print ("The VStack disappeared")
        }
    }
}

#Preview {
    ContentView()
}
