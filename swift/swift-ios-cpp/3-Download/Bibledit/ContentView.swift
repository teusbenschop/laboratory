import SwiftUI
@preconcurrency import WebKit

struct ContentView: View {
    let web_view = WebView(request: URLRequest(url: URL(string: "https://bibledit.org:8091/exports/Sample/usfm/full")!))
    var body: some View {
        web_view
    }
}


class Coordinator: NSObject {
    var parent: WebView
    fileprivate var downloadDestinationURL: URL?
    init(parent: WebView) {
        self.parent = parent
    }
}


extension Coordinator: WKNavigationDelegate {
    
    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction, preferences: WKWebpagePreferences, decisionHandler: @escaping (WKNavigationActionPolicy, WKWebpagePreferences) -> Void) {
        if navigationAction.shouldPerformDownload {
            print ("action decision download")
            decisionHandler(.download, preferences)
        } else {
            print ("action decision allow")
            decisionHandler(.allow, preferences)
        }
    }
    
    func webView(_ webView: WKWebView, decidePolicyFor navigationResponse: WKNavigationResponse, decisionHandler: @escaping (WKNavigationResponsePolicy) -> Void) {
        if navigationResponse.canShowMIMEType {
            print ("response decision allow")
            decisionHandler(.allow)
        } else {
            print ("response decision download")
            decisionHandler(.download)
        }
    }
}


extension Coordinator: WKDownloadDelegate {
    
    func webView(_ webView: WKWebView, navigationAction: WKNavigationAction, didBecome download: WKDownload) {
        download.delegate = self
    }
    
    func download(_ download: WKDownload, decideDestinationUsing response: URLResponse, suggestedFilename: String, completionHandler: @escaping (URL?) -> Void) {
        print ("download decide destination")
        let file_manager = FileManager.default
        let document_directory = file_manager.urls(for: .documentDirectory, in: .userDomainMask)[0]
        let file_url = document_directory.appendingPathComponent("\(suggestedFilename)", isDirectory: false)
        do {
            try file_manager.removeItem(at: file_url)
        }
        catch {
            print(error)
        }
        parent.download_url = file_url
        print(file_url)
        completionHandler(file_url)
    }
    
    func downloadDidFinish(_ download: WKDownload) {
        print ("download did finish 1")
        print (parent.download_url)
        //let activityVC = UIActivityViewController(activityItems: [parent.downloadUrl], applicationActivities: nil)
        //activityVC.popoverPresentationController?.sourceView = self.view
        //activityVC.popoverPresentationController?.sourceRect = self.view.frame
        //activityVC.popoverPresentationController?.barButtonItem = self.navigationItem.rightBarButtonItem
        //self.present(activityVC, animated: true, completion: nil)
    }

    func downloadDidFinish(location url: URL) {
        print ("download did finish 2")
        DispatchQueue.main.async {
            let activityVC = UIActivityViewController(activityItems: [url], applicationActivities: nil)
//            activityVC.popoverPresentationController?.sourceView = self.view
//            activityVC.popoverPresentationController?.sourceRect = self.view.frame
//            activityVC.popoverPresentationController?.barButtonItem = self.navigationItem.rightBarButtonItem
//            self.present(activityVC, animated: true, completion: nil)
        }
    }
    
    func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
        print ("download failed with error")
        print(error.localizedDescription)
        // you can add code here to continue the download in case there was a failure.
    }
}


struct WebView: UIViewRepresentable {
    let request: URLRequest
    private var webView: WKWebView
    var download_url = URL(fileURLWithPath: "")

    init(request: URLRequest) {
        self.webView = WKWebView()
        self.request = request
    }

    func makeCoordinator() -> Coordinator {
        Coordinator(parent: self)
    }
    
    func makeUIView(context: Context) -> WKWebView {
        return webView
    }

    func updateUIView(_ uiView: WKWebView, context: Context) {
        uiView.navigationDelegate = context.coordinator
        uiView.load(request)
    }
    
    func loadURL(urlString: String) {
        webView.load(URLRequest(url: URL(string: urlString)!))
    }

}


/* Todo
private func downloadData(fromURL url:URL,
                          fileName:String,
                          completion:@escaping (Bool, URL?) -> Void) {
    webView.configuration.websiteDataStore.httpCookieStore.getAllCookies() { cookies in
        let session = URLSession.shared
        session.configuration.httpCookieStorage?.setCookies(cookies, for: url, mainDocumentURL: nil)
        let task = session.downloadTask(with: url) { localURL, urlResponse, error in
            if let localURL = localURL {
                let destinationURL = self.moveDownloadedFile(url: localURL, fileName: fileName)
                completion(true, destinationURL)
            }
            else {
                completion(false, nil)
            }
        }
        
        task.resume()
    }
}
 */

private func moveDownloadedFile(url:URL, fileName:String) -> URL {
    let tempDir = NSTemporaryDirectory()
    let destinationPath = tempDir + fileName
    let destinationURL = URL(fileURLWithPath: destinationPath)
    try? FileManager.default.removeItem(at: destinationURL)
    try? FileManager.default.moveItem(at: url, to: destinationURL)
    return destinationURL
}


/* Todo
func webView(_ webView: WKWebView, decidePolicyFor navigationResponse: WKNavigationResponse, decisionHandler: @escaping (WKNavigationResponsePolicy) -> Void) {
    if let mimeType = navigationResponse.response.mimeType {
        if isMimeTypeConfigured(mimeType) {
            if let url = navigationResponse.response.url {
                if #available(iOS 14.5, *) {
                    decisionHandler(.download)
                } else {
                    var fileName = getDefaultFileName(forMimeType: mimeType)
                    if let name = getFileNameFromResponse(navigationResponse.response) {
                        fileName = name
                    }
                    downloadData(fromURL: url, fileName: fileName) { success, destinationURL in
                        if success, let destinationURL = destinationURL {
                            self.delegate.fileDownloadedAtURL(url: destinationURL)
                        }
                    }
                    decisionHandler(.cancel)
                }
                return
            }
        }
    }
    decisionHandler(.allow)
}
 */

/* Todo
extension WKWebviewDownloadHelper: WKDownloadDelegate {
    func download(_ download: WKDownload, decideDestinationUsing response: URLResponse, suggestedFilename: String, completionHandler: @escaping (URL?) -> Void) {
        let temporaryDir = NSTemporaryDirectory()
        let fileName = temporaryDir + "/" + suggestedFilename
        let url = URL(fileURLWithPath: fileName)
        fileDestinationURL = url
        completionHandler(url)
    }
    
   
    func downloadDidFinish(_ download: WKDownload) {
        print("download finish")
        if let url = fileDestinationURL {
            self.delegate.fileDownloadedAtURL(url: url)
        }
    }
}
 */


