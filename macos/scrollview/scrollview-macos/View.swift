import Cocoa

class View: NSView {
  
  required init?(coder: NSCoder) {
    super.init(coder: coder)
    
    // Set up the custom view that will become the document view for the scroll view.
    // It gets a more prominent height than the scroll view to get it to scroll.
    // The document views size works as the content size for the scroll view.
    // This will arrange for vertical scrolling.
    // (To get horizontal scrolling, the document view should we wider than the scroll view.)
    let subviewSize = CGSize(width: 640, height: 480 * 2)
    let subviewFrame = CGRect(origin: .zero, size: subviewSize)
    
    let documentView = NSImageView(frame: subviewFrame)
    let image = NSImage(imageLiteralResourceName: "Image")
    documentView.image = image
    
    let scrollViewFrame = CGRect(origin: .zero, size: subviewSize)
    let scrollView = NSScrollView(frame: scrollViewFrame)
    scrollView.documentView = documentView
    scrollView.contentView.scroll(to: .zero)

    self.addSubview(scrollView)
    
  }

  override func draw(_ dirtyRect: NSRect) {
    super.draw(dirtyRect)
  }

  override var isFlipped: Bool { return true }

}




