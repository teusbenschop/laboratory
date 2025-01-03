import SwiftUI
import WebKit

let about_blank : String = "about:blank"

let webview_advanced : WebView = WebView()
let webview_translate : WebView = WebView()
let webview_resources : WebView = WebView()
let webview_notes : WebView = WebView()
let webview_settings : WebView = WebView()

enum ViewState {
    case splash
    case basic
    case advanced
}

//let web_content_downloader : WebContentDownloader = WebContentDownloader()


struct ContentView: View {
    
    let timer = Timer.publish(every: 5, on: .main, in: .common).autoconnect()
    
    @State var view_state : ViewState = ViewState.advanced
    
    @State var tab_number : Int = 0
    
    var body: some View {
        VStack {
            if view_state == ViewState.splash {
                VStack {
                    Spacer()
                    Text("Bibledit")
                        .dynamicTypeSize(.xxxLarge)
                    Spacer()
                    ProgressView()
                        .dynamicTypeSize(.xxxLarge)
                    Spacer()
                    Image(systemName: "globe")
                        .imageScale(.large)
                        .scaledToFit()
                    Spacer()
                }
                .onAppear(){
                }
            }
            if view_state == ViewState.basic {
                TabView(selection: $tab_number) {
                    webview_translate
                        .tabItem {
                            Label("Translate", systemImage: "doc")
                        }
                        .tag(0)
                        .onAppear() {
                            webview_translate.loadURL(urlString: "https://bibledit.org:8091/editone2/index")
                        }
                    webview_resources
                        .tabItem {
                            Label("Resources", systemImage: "book")
                        }
                        .tag(1)
                        .onAppear() {
                            webview_resources.loadURL(urlString: "https://bibledit.org:8091/resource/index")
                        }
                    webview_notes
                        .tabItem {
                            Label("Notes", systemImage: "note")
                        }
                        .tag(2)
                        .onAppear() {
                            webview_notes.loadURL(urlString: "https://bibledit.org:8091/notes/index")
                        }
                    webview_settings
                        .tabItem {
                            Label("Settings", systemImage: "gear")
                        }
                        .tag(3)
                        .onAppear() {
                            webview_settings.loadURL(urlString: "https://bibledit.org:8091/personalize/index")
                        }
                }
                .onAppear() {
                    webview_advanced.loadURL(urlString: about_blank)
                }
            }
            if view_state == ViewState.advanced {
                webview_advanced
                    .onAppear() {
                        webview_translate.loadURL(urlString: about_blank)
                        webview_resources.loadURL(urlString: about_blank)
                        webview_notes.loadURL(urlString: about_blank)
                        webview_settings.loadURL(urlString: about_blank)
                        webview_advanced.loadURL(urlString: "https://bibledit.org:8091/exports/Sample/usfm/full")
                    }
            }

        }
        .onReceive(timer) { input in
            if view_state == ViewState.splash {
                view_state = ViewState.basic
                if tab_number < 3 {
                    tab_number += 1
                }
                else {
                    tab_number = 0
                }
            }
            else if view_state == ViewState.basic {
                view_state = ViewState.advanced
            }
            else if view_state == ViewState.advanced {
//                view_state = ViewState.splash
            }
        }
    }
    
    init() {
    }
}


struct WebView: UIViewRepresentable {
    
    let webView: WKWebView
    
    init() {
        self.webView = WKWebView()
    }
    
    func makeUIView(context: Context) -> WKWebView {
        webView.allowsBackForwardNavigationGestures = true
        return webView
    }
    
    func updateUIView(_ uiView: WKWebView, context: Context) {
    }
    
    func goBack(){
        webView.goBack()
    }
    
    func goForward(){
        webView.goForward()
    }
    
    func loadURL(urlString: String) {
        webView.load(URLRequest(url: URL(string: urlString)!))
    }
    
//    func webView(_ webView: WKWebView, decidePolicyFor navigationAction: WKNavigationAction, preferences: WKWebpagePreferences, decisionHandler: @escaping (WKNavigationActionPolicy, WKWebpagePreferences) -> Void) {
//        if navigationAction.shouldPerformDownload {
//            decisionHandler(.download, preferences)
//            print ("decision handler download preferences")
//        } else {
//            decisionHandler(.allow, preferences)
//            print ("decision handler allow preferences")
//        }
//    }
    
//    func webView(_ webView: WKWebView, decidePolicyFor navigationResponse: WKNavigationResponse, decisionHandler: @escaping (WKNavigationResponsePolicy) -> Void) {
//        if navigationResponse.canShowMIMEType {
//            decisionHandler(.allow)
//            print ("decision handler allow")
//        } else {
//            decisionHandler(.download)
//            print ("decision handler download")
//        }
//    }
    
//    func download(_ download: WKDownload, decideDestinationUsing
//                  response: URLResponse, suggestedFilename: String,
//                  completionHandler: @escaping (URL?) -> Void) {
//        completionHandler(URL(string: "/tmp/ios-download"))
//    }
    
//    func webView(_ webView: WKWebView, navigationAction: WKNavigationAction, didBecome download: WKDownload) {
//        download.delegate = web_content_downloader
//    }

//    func downloadDidFinish(_ download: WKDownload) {
//        print ("download did finish")
//    }
    
//    public func download(_ download: WKDownload, didFailWithError error: Error, resumeData: Data?) {
//        print ("download error")
//    }
}


//protocol WebDownloadable: WKDownloadDelegate {
//    func downloadDidFinish(fileResultPath: URL)
//    func downloadDidFail(error: Error, resumeData: Data?)
//}

//class WebContentDownloader: NSObject {
//    
//    private var filePathDestination: URL?
//    
//    weak var downloadDelegate: WebDownloadable?
//    
//    func generateTempFile(with suggestedFileName: String?) -> URL {
//        let temporaryDirectoryFolder = URL(fileURLWithPath: NSTemporaryDirectory(), isDirectory: true)
//        print ("generate temp file")
//        return temporaryDirectoryFolder.appendingPathComponent(suggestedFileName ?? ProcessInfo().globallyUniqueString)
//    }
//    
//    func downloadFileOldWay(fileURL: URL, optionSessionCookies: [HTTPCookie]?) {
//        print ("download file old way")
//        // Your classic URL Session Data Task
//    }
//    
//    private func cleanUp() {
//        filePathDestination = nil
//        print ("cleanup")
//    }
//}


//extension WebContentDownloader: WKDownloadDelegate {
//    
//    func downloadDidFinish(_ download: WKDownload) {
//        
//        guard let filePathDestination = filePathDestination else {
//            return
//        }
//        downloadDelegate?.downloadDidFinish(fileResultPath: filePathDestination)
//        cleanUp()
//    }
//    
//    public func download(_ download: WKDownload,
//                         didFailWithError error: Error,
//                         resumeData: Data?) {
//        downloadDelegate?.downloadDidFail(error: error, resumeData: resumeData)
//        print ("download 1 within extension")
//    }
//    
//    func download(_ download: WKDownload, decideDestinationUsing
//                  response: URLResponse, suggestedFilename: String,
//                  completionHandler: @escaping (URL?) -> Void) {
//        
//        print ("download 2 within extension")
//        filePathDestination = generateTempFile(with: suggestedFilename)
//        completionHandler(filePathDestination)
//    }
//    
//    func webView(_ webView: WKWebView, navigationAction: WKNavigationAction, didBecome download: WKDownload) {
//        print ("set download delegate")
//        download.delegate = downloadDelegate
//    }
//}
