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

struct ContentView: View {
    
    let timer = Timer.publish(every: 5, on: .main, in: .common).autoconnect()
    
    @State var view_state : ViewState = ViewState.splash
    
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
                TabView {
                    webview_translate
                        .tabItem {
                            Label("Translate", systemImage: "doc")
                        }
                        .onAppear() {
                            webview_translate.loadURL(urlString: "https://bibledit.org:8091/editone2/index")
                        }
                    webview_resources
                        .tabItem {
                            Label("Resources", systemImage: "book")
                        }
                        .onAppear() {
                            webview_resources.loadURL(urlString: "https://bibledit.org:8091/resource/index")
                        }
                    webview_notes
                        .tabItem {
                            Label("Notes", systemImage: "note")
                        }
                        .onAppear() {
                            webview_notes.loadURL(urlString: "https://bibledit.org:8091/notes/index")
                        }
                    webview_settings
                        .tabItem {
                            Label("Settings", systemImage: "gear")
                        }
                        .onAppear() {
                            webview_settings.loadURL(urlString: "https://bibledit.org:8091/index/index?item=settings")
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
                        webview_advanced.loadURL(urlString: "https://bibledit.org:8091")
                    }
            }

        }
        .onReceive(timer) { input in
            if view_state == ViewState.splash {
                view_state = ViewState.basic
            }
            else if view_state == ViewState.basic {
                view_state = ViewState.advanced
            }
            else if view_state == ViewState.advanced {
                view_state = ViewState.splash
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
}
