import Cocoa
import SwiftUI

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

  var window: NSWindow!


  func applicationDidFinishLaunching(_ aNotification: Notification) {

    // Create the window and set the content view. 
    window = NSWindow(
        contentRect: NSRect(x: 0, y: 0, width: 480, height: 300),
        styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
        backing: .buffered, defer: false)
    window.center()
    window.setFrameAutosaveName("SwiftUI Laboratory")

    //let contentView = ContentView()
    //window.contentView = NSHostingView(rootView: contentView)

    let store = ReposStore(service: .init())
    window.contentView = NSHostingView(
        rootView: SearchView().environmentObject(store)
    )

    window.makeKeyAndOrderFront(nil)
  }

  func applicationWillTerminate(_ aNotification: Notification) {
    // Insert code here to tear down your application
  }


}

