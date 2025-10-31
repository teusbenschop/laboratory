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


func annotationStateImageName(selected: Bool, dragging: Bool) -> String
{
    if selected {
        return "arrow.up.and.down.and.arrow.left.and.right"
    }
    return "circle.fill"
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
        updateAppearance()
    }
    
    private func updateAppearance() {
        let dragging = dragState != .none
        let scale: CGFloat = dragging ? 1.3 : 1.0
        let name = annotationStateImageName(selected: self.isSelected, dragging: dragging)
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
        
        func mapView(_ mapView: MKMapView, viewFor annotation: MKAnnotation) -> MKAnnotationView? {
            if annotation is DraggableAnnotation {
                let annotationView = DraggableAnnotationView(annotation: annotation, reuseIdentifier: "draggable")
                annotationView.isDraggable = true
                return annotationView
            }
            return nil
        }

        // Gets fired if an annotation view got selected.
        func mapView(_ mapView: MKMapView, didSelect view: MKAnnotationView) {
            view.image = UIImage(systemName: annotationStateImageName(selected: view.isSelected, dragging: false))
        }
        
        
        // Gets fired if an annotation view got deselected.
        func mapView(_ mapView: MKMapView, didDeselect view: MKAnnotationView) {
            view.image = UIImage(systemName: annotationStateImageName(selected: view.isSelected, dragging: false))
        }

        
        // Gets fired if an annotation got selected.
        func mapView(_ mapView: MKMapView, didSelect annotation: any MKAnnotation) {
        }

        // Gets fired if an annotation got deselected.
        func mapView(_ mapView: MKMapView, didDeselect annotation: any MKAnnotation) {
        }

        func mapView(_ mapView: MKMapView, annotationView view: MKAnnotationView, didChange newState: MKAnnotationView.DragState, fromOldState oldState: MKAnnotationView.DragState) {
            guard let annotation = view.annotation as? DraggableAnnotation else { return }

            print("Drag state changed from", dragStateToString(state: oldState), "to", dragStateToString(state: newState))
            
            switch newState {
            case .starting:
                view.dragState = .dragging
            case .dragging:
                break
            case .ending, .canceling:
                view.dragState = .none
                mapView.deselectAnnotation(annotation, animated: true)
            case .none:
                break
            default:
                break
            }
        }
        
    }
}

struct ContentView: View {
    var body: some View {
        MapView(region: region)
            .edgesIgnoringSafeArea(.all)
    }
}
