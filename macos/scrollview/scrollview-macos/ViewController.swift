import Cocoa

class ViewController: NSViewController, NSWindowDelegate {
  
  override func viewDidAppear() {
    print(view.window!)
    view.window!.delegate = self
  }

  override func viewWillAppear() {
  }

  override func viewDidLoad() {
    super.viewDidLoad()
  }

  override var representedObject: Any? {
    didSet {
    }
  }

  func windowWillResize(sender: NSWindow, toSize frameSize: NSSize) -> NSSize {
    print(#line)
    return frameSize
  }

  private func windowDidResize(_ notification: NSNotification) {
    print(#line)
    print(notification)
  }

}

