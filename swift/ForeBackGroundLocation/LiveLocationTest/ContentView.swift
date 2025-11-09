import SwiftUI

struct ContentView: View {
    @StateObject private var locationViewModel = LocationManager()

    var body: some View {
        NavigationView {
            VStack {
                Button("Start") {
                    locationViewModel.start()
                }
                .buttonStyle(.borderedProminent)
                Button("Stop") {
                    locationViewModel.stop()
                }
                .buttonStyle(.borderedProminent)
            }

            .onAppear {
                locationViewModel.requestPermission()
//                vm.startLiveUpdates() // Todo resume on relaunch
            }
        }
    }
}
