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
                        webview_advanced.loadURL(urlString: "https://bibledit.org:8091")
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
