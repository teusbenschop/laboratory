import SwiftUI

struct ContentView: View {
    @StateObject private var locationManager = LocationManager()

    var body: some View {
        NavigationView {
            VStack {
                Text("Latitude " + locationManager.location.coordinate.latitude.description)
                Text("Longitude " + locationManager.location.coordinate.longitude.description)
                Text("Speed " + locationManager.location.speed.description)
                Text("Time " + locationManager.location.timestamp.description)
                Text("Sequence " + locationManager.counter.description)
                    .padding(.bottom)
                Button("Start") {
                    locationManager.start()
                }
                .buttonStyle(.borderedProminent)
                Button("Stop") {
                    locationManager.stop()
                }
                .buttonStyle(.borderedProminent)
            }

            .onAppear {
                locationManager.requestPermission()
//                vm.startLiveUpdates() // Todo resume on relaunch
            }
        }
    }
}
