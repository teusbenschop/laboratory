#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <thread>
#include <string.h>
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma GCC diagnostic pop
#include "ldap.h"
#include "main.h"


// Some slightly older documentation that shows the basic of an LDAP client.
// https://docs.oracle.com/cd/E19957-01/817-6707/writing.html#wp25467


int main([[maybe_unused]]int argc, [[maybe_unused]]char *argv[])
{
    // Get a pointer to an opaque LDAP structure and keep it for subsequent calls.
    constexpr const auto uri {"ldap://192.168.135.125:389"};
    std::cout << "Initializing LDAP structure for URI " << std::quoted(uri) << std::endl;
    LDAP* ld {nullptr};
    int rc = ldap_initialize(&ld, uri);
    if (rc != LDAP_SUCCESS) {
        std::cerr << "Could not initialize LDAP structure: " << ldap_err2string(rc) << " (" << rc << ")" << std::endl;
        return EXIT_FAILURE;
    }

    // Set client library debug level.
    // LDAP_DEBUG_ANY, LDAP_DEBUG_ARGS, LDAP_DEBUG_BER, LDAP_DEBUG_CONNS, LDAP_DEBUG_NONE, LDAP_DEBUG_PACKETS, LDAP_DEBUG_PARSE, LDAP_DEBUG_TRACE
    //const int debug_level {65535};
    //ldap_set_option(nullptr, LDAP_OPT_DEBUG_LEVEL, &debug_level);

    // Specify that this is an LDAPv3 client.
    std::cout << "Specify LDAPv3 client" << std::endl;
    const auto version {LDAP_VERSION3};
    ldap_set_option (ld, LDAP_OPT_PROTOCOL_VERSION, &version);

    // Bind to the server.
    // A bind request should be sent if:
    // 1. You want to authenticate to the server to add or modify entries in a directory
    //    that requires authentication as a user with certain access privileges.
    // 2. You are connecting to a LDAPv2 server.
    //    LDAPv2 servers typically require clients to bind before any operations can be performed.
    constexpr const auto admin_dn {"cn=admin,dc=dev,dc=com"};
    constexpr const auto admin_password {"test123"};
    std::cout << "Bind to the server with dn " << std::quoted(admin_dn) << " password " << std::quoted(admin_password) << std::endl;
    rc = ldap_bind_s (ld, admin_dn, admin_password, LDAP_AUTH_SIMPLE);
    if (rc != LDAP_SUCCESS) {
        std::cerr << "Could not bind to the server: " << ldap_err2string(rc) << " (" << rc << ")" << std::endl;
        return EXIT_FAILURE;
    }

    // Search the LDAP directory for entries.
    constexpr const auto base_dn {"dc=dev,dc=com"};
    constexpr const auto scope {LDAP_SCOPE_SUBTREE};
    constexpr const auto filter {"(cn=lassy)"};
    std::cout << "Searching the directory for entries" << std::endl;
    std::cout << "Base DN " << std::quoted(base_dn) << std::endl;
    std::cout << "Scope " << scope << " = subtree" << std::endl;
    std::cout << "Search filter " << std::quoted(filter) << std::endl;
    LDAPMessage* result {nullptr};
    rc = ldap_search_ext_s (ld, base_dn, scope, filter, NULL, 0, NULL, NULL, NULL, 0, &result);
    if (rc != LDAP_SUCCESS) {
        std::cerr << "Failed to search: " << ldap_err2string(rc) << " (" << rc << ")" << std::endl;
        return EXIT_FAILURE;
    }

    // Display the entries that were found.
    std::cout << "Displaying the results" << std::endl;
    LDAPMessage* entry {nullptr};
    for (entry = ldap_first_entry (ld, result);
         entry != nullptr;
         entry = ldap_next_entry (ld, entry))
    {
        // Allocate a copy of the Distinguished Name from the entry.
        char* dn = ldap_get_dn (ld, entry);
        std::cout << "dn: " << dn << std::endl;
        // Free the allocated memory.
        if (dn) 
            ldap_memfree (dn);
    }
             
    // Free the allocated search result.
    ldap_msgfree (result);

    // Unbind from the server directory, terminate the current association,
    // and free the resources contained in the LDAP structure.
    std::cout << "Disconnect and free resources" << std::endl;
    ldap_unbind (ld);

    // Done.
    return EXIT_SUCCESS;
}
