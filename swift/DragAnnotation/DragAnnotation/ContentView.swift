import SwiftUI
import MapKit
import CoreLocation


let amsterdam = CLLocationCoordinate2D(latitude: 52.37271356292965, longitude: 4.900406782664972)


let region = MKCoordinateRegion(
    center: amsterdam,
    span: MKCoordinateSpan(latitudeDelta: 0.005, longitudeDelta: 0.005)
)


func dragStateToString(state: MKAnnotationView.DragState) -> String
{
    switch state {
    case .none:
        return "none"
    case .starting:
        return "starting"
    case .dragging:
        return "dragging"
    case .canceling:
        return "canceling"
    case .ending:
        return "ending"
    default:
        return "unknown"
    }
}


class DraggableAnnotation: NSObject, MKAnnotation {

    var coordinate: CLLocationCoordinate2D
    var index: Int
    
    init(coordinate: CLLocationCoordinate2D, index: Int) {
        self.coordinate = coordinate
        self.index = index
        super.init()
    }
}


class DraggableAnnotationView: MKAnnotationView {
    override var annotation: MKAnnotation? {
        didSet {
            self.isDraggable = true
            self.canShowCallout = false
            updateAppearance()
        }
    }
    
    override func setDragState(_ newState: MKAnnotationView.DragState, animated: Bool) {
        super.setDragState(newState, animated: animated)
        print("Drag state changed to", dragStateToString(state: newState))
        updateAppearance()
    }
    
    private func updateAppearance() {
        let dragging = dragState != .none
        let scale: CGFloat = dragging ? 1.3 : 1.0
        let name = dragging ? "arrow.up.and.down.and.arrow.left.and.right" : "circle.fill"
        
        UIView.animate(withDuration: 0.5) {
            self.transform = CGAffineTransform(scaleX: scale, y: scale)
            self.image = UIImage(systemName: name)?.withTintColor(.red, renderingMode: .alwaysOriginal)
        }
    }
}


struct MapView: UIViewRepresentable {
    var region: MKCoordinateRegion
    
    func makeUIView(context: Context) -> MKMapView {
        let mapView = MKMapView()
        mapView.delegate = context.coordinator
        mapView.mapType = .standard
        let tapRecognizer = UITapGestureRecognizer(target: context.coordinator, action: #selector(Coordinator.handleTap(_:)))
        mapView.addGestureRecognizer(tapRecognizer)
        let annotation = DraggableAnnotation(coordinate: amsterdam, index: 1)
        mapView.addAnnotation(annotation)
        return mapView
    }
    
    func updateUIView(_ uiView: MKMapView, context: Context) {
        uiView.setRegion(region, animated: true)
    }
    
    func makeCoordinator() -> Coordinator {
        Coordinator(self)
    }
    
    class Coordinator: NSObject, MKMapViewDelegate {
        var parent: MapView
        
        init(_ parent: MapView) {
            self.parent = parent
            super.init()
        }
        
        @objc func handleTap(_ gestureRecognizer: UITapGestureRecognizer) {
            let mapView = gestureRecognizer.view as! MKMapView
            let touchPoint = gestureRecognizer.location(in: mapView)
            let coordinate = mapView.convert(touchPoint, toCoordinateFrom: mapView)
            print ("tap on", coordinate)
        }
        
        func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
            if annotation is DraggableAnnotation {
                let annotationView = DraggableAnnotationView(annotation: annotation, reuseIdentifier: "draggablePin")
                annotationView.isDraggable = true
                print("Created new DraggableAnnotationView")
                return annotationView
            }
            return nil
        }
        
        func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, didChange newState: MKAnnotationView.DragState, fromOldState oldState: MKAnnotationView.DragState) {
            guard let annotation = view.annotation as? DraggableAnnotation else { return }
            print ("annotation index", annotation.index)

            print("Drag state changed from", dragStateToString(state: oldState), "to", dragStateToString(state: newState))
            
            switch newState {
            case .starting:
                view.dragState = .dragging
            case .dragging:
                break
            case .ending, .canceling:
                view.dragState = .none
            case .none:
                break
            default:
                break
            }
        }
        
        func mapView(_ mapView: MKMapView, rendererFor overlay: MKOverlay) -> MKOverlayRenderer {
            if let polyline = overlay as? MKPolyline {
                let renderer = MKPolylineRenderer(polyline: polyline)
                renderer.strokeColor = .blue
                renderer.lineWidth = 3
                return renderer
            }
            return MKOverlayRenderer()
        }
    }
}

struct ContentView: View {
    var body: some View {
        MapView(region: region)
            .edgesIgnoringSafeArea(.all)
    }
}

