// https://mecid.github.io/2019/06/05/swiftui-making-real-world-app/

// SwiftUI is a declarative Component-Oriented framework
// What we have in SwiftUI is a state and view derived from the state.
// When the state changes, SwiftUI rebuilds the UI for that new state.

import SwiftUI
import Foundation
import Combine


// If we want to use the Repo struct below as the model
// which will be used by SwiftUI to build a View,
// it has to conform to the Identifiable protocol.
// The only requirement of the Identifiable protocol is the id property,
// which has to be a Hashable value.
struct Repo: Decodable, Identifiable {
  var id: Int
  let name: String
  let description: String
}


struct SearchResponse: Decodable {
  let items: [Repo]
}


class GithubService {
  private let session: URLSession
  private let decoder: JSONDecoder
  
  init(session: URLSession = .shared, decoder: JSONDecoder = .init()) {
    self.session = session
    self.decoder = decoder
  }
  
  func search(matching query: String, handler: @escaping (Result<[Repo], Error>) -> Void) {
    guard
      var urlComponents = URLComponents(string: "https://api.github.com/search/repositories")
      else { preconditionFailure("Can't create url components...") }
    
    urlComponents.queryItems = [
      URLQueryItem(name: "q", value: query)
    ]
    
    guard
      let url = urlComponents.url
      else { preconditionFailure("Can't create url from url components...") }
    
    session.dataTask(with: url) { [weak self] data, _, error in
      if let error = error {
        handler(.failure(error))
      } else {
        do {
          let data = data ?? Data()
          let response = try self?.decoder.decode(SearchResponse.self, from: data)
          handler(.success(response?.items ?? []))
        } catch {
          handler(.failure(error))
        }
      }
    }.resume()
  }
}


struct RepoRow: View {
  let repo: Repo
  
  var body: some View {
    VStack(alignment: .leading) {
      Text(repo.name)
        .font(.headline)
      Text(repo.description)
        .font(.subheadline)
    }
  }
}


// The ReposStore class should conform ObservableObject protocol.
// It makes it possible to use it inside Environment
// and rebuild the view as soon as any property marked as @Published changes.
class ReposStore: ObservableObject {
  
  // Only the setter is private.
  @Published private(set) var repos: [Repo] = []
  
  private let service: GithubService
  init(service: GithubService) {
    self.service = service
  }
  
  func fetch(matching query: String) {
    service.search(matching: query) { [weak self] result in
      DispatchQueue.main.async {
        switch result {
        case .success(let repos): self?.repos = repos
        case .failure: self?.repos = []
        }
      }
    }
  }
}


// The main difference between @State and @EnvironmentObject is
// that @State is accessible only to a particular view,
// as opposed to the @EnvironmentObject which is available to every view inside the Environment.
// Both of them are used by SwiftUI to track changes and rebuild views when changes are observed.


struct SearchView : View {

  // A query field which is marked as @State.
  // It means that the view is derived from this state.
  // When the state changes, SwiftUI rebuilds the view.
  // The @State lives in a special memory segment,
  // where only corresponding views can access and update them.
  // @State is a new Swift feature called a Property Wrapper.
  // The exciting aspect is the usage of the $ in $query.
  // It means that it gets a reference to the property wrapper, not to the value itself.
  // We use it to connect the TextField and the query variable in a two-way binding.
  @State private var query: String = "Swift"
  
  // The @EnvironmentObject is a part of feature called Environment.
  // You can populate your Environment with all needed service classes,
  // and then access them from any view inside that Environment.
  @EnvironmentObject var repoStore: ReposStore
  
  var body: some View {
    
    VStack {
      TextField("Type something...", text: $query, onCommit: fetch)
      List {
        ForEach(repoStore.repos) { repo in
          RepoRow(repo: repo)
        }
      }
    }.onAppear(perform: fetch)
  }
  
  private func fetch() {
    repoStore.fetch(matching: query)
  }
}

/*
struct GithubServiceView_Previews: PreviewProvider {
  let store = ReposStore(service: .init())
  static var previews: some View {
    SearchView()
  }
}
*/

