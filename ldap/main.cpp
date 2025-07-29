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


// Some slightly older documentation that shows the basics of an LDAP client.
// https://docs.oracle.com/cd/E19957-01/817-6707/writing.html#wp25467


int main([[maybe_unused]]int argc, [[maybe_unused]]char *argv[])
{
    // Get a pointer to an opaque LDAP structure and keep it for subsequent calls.
    constexpr const auto uri {"ldap://10.211.55.64:389"};
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
    constexpr const char* user_dn {"cn=ldapuser,ou=People,dc=dev,dc=com"};
    constexpr const char* password {"ldappass"};
    std::cout << "Bind to the server with dn " << std::quoted(user_dn) << " password " << std::quoted(password) << std::endl;
    {
        rc = ldap_bind_s (ld, user_dn, password, LDAP_AUTH_SIMPLE);
        if (rc != LDAP_SUCCESS) {
            std::cerr << "Could not bind to the server: " << ldap_err2string(rc) << " (" << rc << ")" << std::endl;
            return EXIT_FAILURE;
        }
    }
    {
        berval credentials {
            .bv_len = strlen(password),
            .bv_val = const_cast<char*>(password),
        };
        rc = ldap_sasl_bind_s(ld,
                              user_dn,
                              LDAP_SASL_SIMPLE,
                              &credentials,
                              nullptr, // serverctrls - not used for simple authentication.
                              nullptr, // clientctrls - not used for simple authentication.
                              nullptr  // servercredp - not used for simple authentication.
                              );
        if (rc != LDAP_SUCCESS) {
            std::cerr << "Could not bind to the server: " << ldap_err2string(rc) << " (" << rc << ")" << std::endl;
            return EXIT_FAILURE;
        }
        // No need to do this due to how the .bv_val is allocated.
        //ber_bvfree(&credentials);
    }

    // Search the LDAP directory for entries.
    constexpr const auto base_dn {"dc=dev,dc=com"};
    constexpr const auto scope {LDAP_SCOPE_SUBTREE};
    constexpr const auto filter {"(cn=ldapuser)"};
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
        // Get the values for "cn".
        berval** values = ldap_get_values_len(ld, entry, "cn");
        berval* const* list = values;
        while (list && *list) {
            berval* value = *list++;
            std::string s (value->bv_val, value->bv_len);
            std::cout << "cn: " << s << std::endl; // Todo crashes.
        }
        // Free the allocated memory for values.
        if (values)
            ldap_value_free_len(values);
    }
             
    // Free the allocated search result.
    ldap_msgfree (result);

    // Unbind from the server directory, terminate the current association,
    // and free the resources contained in the LDAP structure.
    std::cout << "Disconnect and free resources" << std::endl;
    ldap_unbind_ext (ld, nullptr, nullptr);

    // Done.
    std::cout << "Ready" << std::endl;
    return EXIT_SUCCESS;
}
