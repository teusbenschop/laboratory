#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <functional>
#include <memory>
#include <thread>
#include <iomanip>
#include <random>
#include <set>
#include <optional>
#include <unistd.h>
#include <stdlib.h>
#include <microhttpd.h>


// The port that the http(s) server should listen on.
static uint16_t port {8080};

// Http status, normally 200.
static int http_status {MHD_HTTP_OK};

// The error response to give.
std::optional<std::string> error_response {};

// Structure for reading the POSTed request.
struct PostStatus {
    bool status {false};
    char* buff;
};


static MHD_Result on_client_connect([[maybe_unused]] void* cls,
                                    [[maybe_unused]] const sockaddr* addr,
                                    [[maybe_unused]] socklen_t addrlen)
{
    std::cout << "New incoming connection" << std::endl;
    return MHD_YES;
}


static MHD_Result print_http_header([[maybe_unused]] void* cls, 
                                    [[maybe_unused]] const MHD_ValueKind kind,
                                    [[maybe_unused]] const char* key, 
                                    [[maybe_unused]] const char* value)
{
    std::cout << key << ": " << value << std::endl;
    return MHD_YES;
}


static MHD_Result answer_to_connection([[maybe_unused]] void* cls,
                                       MHD_Connection* connection,
                                       [[maybe_unused]] const char* url,
                                       [[maybe_unused]] const char* method, 
                                       [[maybe_unused]] const char* version,
                                       const char* upload_data, 
                                       size_t* upload_data_size,
                                       void** con_cls)
{
    // The "page" contains the response to return.
    // It is static for timing issues so that it contains to exist in memory for processing by the micro HTTP daemon library.
    static std::string page;

    // Clear the response to start with.
    page.clear();

    // Storage for the POSTed data.
    std::string posted_data{};

    // This callback may be called multiple times to process the POSTed data.
    // The first time that it is called, variable con_cls is NULL.
    if (page.empty()) {
        PostStatus* post = static_cast <PostStatus*>(*con_cls);
        if (!post) {
            post = static_cast <PostStatus*>(malloc(sizeof(PostStatus)));
            post->status = false;
            *con_cls = post;
        }
        if (!post->status) {
            post->status = true;
            return MHD_YES;
        }
        else {
            if (*upload_data_size != 0) {
                post->buff = static_cast<char*>(malloc(*upload_data_size + 1));
                snprintf(post->buff, *upload_data_size + 1, "%s", upload_data);
                *upload_data_size = 0;
                return MHD_YES;
            }
            else {
                std::cout << "Handle request " << method << " " << url << " " << version << " - start of headers - :" << std::endl;
                // Print the headers.
                MHD_get_connection_values(connection, MHD_HEADER_KIND, &print_http_header, NULL);
                std::cout << "- end of headers -" << std::endl;
                // Copy the POSTed data.
                posted_data = post->buff;
                free(post->buff);
            }
        }
        if (!post)
            free(post);
    }

    // At this point the POSTed request from UMS has arrived.
    // Print it.
    std::cout << posted_data << std::endl;

    page = "HTTP response: Hello world";

    // Add final new line (although not strictly needed - just for clarity if testing on terminal).
    page.append("\n");

    // Create the response, pass flag to automatically free it once sent off.
    void* void_page = const_cast <void*>(reinterpret_cast <const void*>(page.c_str()));
    MHD_Response* response = MHD_create_response_from_buffer(page.length(), void_page, MHD_RESPMEM_MUST_FREE);

    // Add one header just now.
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "application/json");

    // Queue the response for sending.
    MHD_Result result = MHD_queue_response(connection, http_status, response);

    return result;
}


int main()
{
    MHD_Daemon* daemon {nullptr};
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY /*MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_PEDANTIC_CHECKS*/,
                              port, &on_client_connect, NULL,
                              &answer_to_connection,
                              NULL, MHD_OPTION_END);
    if (!daemon) {
        std::cerr << "Could not create the HTTP server" << std::endl;
        return EXIT_FAILURE;
    }

    // Wait for any signal.
    pause();

    // Clean up.
    MHD_stop_daemon(daemon);
    return EXIT_SUCCESS;
}
