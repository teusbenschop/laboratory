//
//  ProductView.swift
//  Laboratory
//
//  Created by Teus Benschop on 27/10/2019.
//  Copyright © 2019 Free Software Consultants. All rights reserved.
//

import SwiftUI


struct Product: Decodable, Identifiable {
  var id: Int
  let title: String
  let isFavorited: Bool
}


struct FilterView: View {
    @Binding var showFavorited: Bool

    var body: some View {
        Toggle(isOn: $showFavorited) {
            Text("Change filter")
        }
    }
}


struct ProductView: View {
  
  //let products: [Product]
  
  @State private var showFavorited: Bool = false
  
  var body: some View {
    List {
      FilterView(showFavorited: $showFavorited)
      
      Button(
        // At button press, it changes the value of the state property,
        // and SwiftUI recreates the View.
        action: { self.showFavorited.toggle() },
        label: { Text("Change filter") }
      )
    
      /*
      ForEach(products) { product in
        if !self.showFavorited || product.isFavorited {
          Text(product.title)
        }
      }
 */
    }
  }
}

struct ProductView_Previews: PreviewProvider {
  static var previews: some View {
    ProductView()
  }
}
