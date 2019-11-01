import Cocoa

class ViewController: NSViewController, NSWindowDelegate {
  
  @IBOutlet var mainView: NSView!

  override func viewDidAppear() {
    view.window?.delegate = self
    print(view.window)
    print(#line)
  }

  override func viewWillAppear() {
    //view.window?.delegate = self
  }

  override func viewDidLoad() {
    super.viewDidLoad()
    Globals.mainView = mainView
    //view.window?.delegate = self
  }

  func windowWillResize(sender: NSWindow, toSize frameSize: NSSize) -> NSSize {
    print(#line)
    return frameSize
  }

  private func windowDidResize(_ notification: NSNotification) {
    print(#line)
    print(notification)
  }
  
  override var representedObject: Any? {
    didSet {
    }
  }

}

