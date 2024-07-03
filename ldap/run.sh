#!/bin/bash

rm -f /tmp/ldap

# Since OpenLDAP 2.3, part of the C API has been deprecated.
# This means that the deprecated C functions are present in the libraries,
# and thus compatibility with legacy clients is ensured,
# but their declarations have been removed from the headers.
# This typically causes lots of warning when compiling applications
# that make extensive use of deprecated functions,
# but it may also lead to subtle issues when the compiler makes wrong guesses
# at the argument and return value types of no longer declared functions.
# To circumvent these issues,
# one can compile legacy applications with CPPFLAGS=-DLDAP_DEPRECATED
# to have the deprecated declarations back in the header files.
# It is recommended that legacy applications be upgraded
# to make use of replacements to deprecated functions.

g++ -Wall -Wextra -pedantic -std=c++17 -lpthread -DLDAP_DEPRECATED -I/opt/local/include -L/opt/local/lib -lldap -o /tmp/ldap main.cpp
if [ $? -ne 0 ]; then exit; fi

/tmp/ldap
