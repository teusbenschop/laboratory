#include "client_wss.hpp"

using namespace std;

int main() {

  using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

  // Second Client() parameter is for whether to do certificate verification.
  WssClient client("api.bitfinex.com/ws/2", true);

  client.on_message = [](shared_ptr<WssClient::Connection> connection, shared_ptr<WssClient::Message> message)
  {
    auto message_str = message->string();
    
    cout << "Message received: \"" << message_str << "\"" << endl;
    
    //cout << "Client: Sending close connection" << endl;
    //connection->send_close(1000);
    (void) connection;
  };
  
  client.on_open = [](shared_ptr<WssClient::Connection> connection)
  {
    cout << "Opened connection" << endl;
    
    string message = "{ \"event\": \"subscribe\", \"channel\": \"ticker\", \"symbol\": \"tBTCUSD\" }";
    cout << "Sending message: \"" << message << "\"" << endl;
    
    auto send_stream = make_shared<WssClient::SendStream>();
    *send_stream << message;
    connection->send(send_stream);
  };
  
  client.on_close = [](shared_ptr<WssClient::Connection>, int status, const string &)
  {
    cout << "Closed connection with status code " << status << endl;
  };
  
  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  client.on_error = [](shared_ptr<WssClient::Connection> /*connection*/, const SimpleWeb::error_code &ec)
  {
    cout << "Error: " << ec << ", error message: " << ec.message() << endl;
  };
  
  client.start();
}

