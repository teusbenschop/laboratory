import Cocoa
import SwiftUI

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

  var window: NSWindow!


  func applicationDidFinishLaunching(_ aNotification: Notification) {

    // Create the window and set the content view. 
    window = NSWindow(
        contentRect: NSRect(x: 0, y: 0, width: 640, height: 480),
        styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
        backing: .buffered, defer: false)
    window.center()
    window.setFrameAutosaveName("SwiftUI Laboratory")

    //let contentView = ContentView()
    //window.contentView = NSHostingView(rootView: contentView)

    /*
    let store = ReposStore(service: .init())
    window.contentView = NSHostingView(
        rootView: SearchView().environmentObject(store)
    )
    */

    /*
    // Create a new Environment.
    let environmentView = EnvironmentView()
    window.contentView = NSHostingView(
      rootView: environmentView
        // You can modify the Environment variables.
        .environment(\.multilineTextAlignment, .center)
        .environment(\.lineLimit, nil)
        .environment(\.lineSpacing, 8)
    )
    */

    let productView = ProductView()
    window.contentView = NSHostingView(rootView: productView)

    window.makeKeyAndOrderFront(nil)
  }

  func applicationWillTerminate(_ aNotification: Notification) {
    // Insert code here to tear down your application
  }


}

