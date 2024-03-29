/*  @example lassy_vms.c
 *  This example creates some scalar registrations that allows
 *  some simple variables to be accessed via SNMP.  In a more
 *  realistic example, it is likely that these variables would also be
 *  manipulated in other ways outside of SNMP gets/sets.
 *
 *  If this module is compiled into an agent, you should be able to
 *  issue snmp commands that look something like (authentication
 *  information not shown in these commands):
 *
 *  - snmpget localhost netSnmpExampleInteger.0
 *  - netSnmpExampleScalars = 42
 *
 *  - snmpset localhost netSnmpExampleInteger.0 = 1234
 *  - netSnmpExampleScalars = 1234
 *
 *  - snmpget localhost netSnmpExampleInteger.0
 *  - netSnmpExampleScalars = 1234
 *
 */


// Include the appropriate header files.
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


// If "--enable-minimalist" has been turned on,
// register the support needed,
// so the needed functions aren't removed at compile time.
netsnmp_feature_require(long_instance);


// Declare the variabless to be accessed.
// Plus their default values.
static long net_snmp_example_integer = 42;
static long lassy_vms_short_error_status = 0;


// Declare the callback handler for printing out more information on SET and GET.
int handle_lassy_vms_object(netsnmp_mib_handler *handler,
                            netsnmp_handler_registration *reginfo,
                            netsnmp_agent_request_info *reqinfo,
                            netsnmp_request_info *requests)
{
    if (reqinfo->mode == MODE_GET) {
        //            net_snmp_example_integer = 100500;
        //            snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
        //                                     &net_snmp_example_integer,
        //                                     sizeof(long));
        printf("Get %s\n", reginfo->handlerName);
        return SNMP_ERR_NOERROR;
    }
    if (MODE_IS_SET(reqinfo->mode)) {
        if (reqinfo->mode == MODE_SET_COMMIT) {
            printf("Set %s\n", reginfo->handlerName);
        }
        return SNMP_ERR_NOERROR;
    }
    printf ("unknown mode (%d) in handler %s\n", reqinfo->mode, reginfo->handlerName);
    return SNMP_ERR_GENERR;
}


// The initialization routine, automatically called by the agent.
// To get called, the function name must match init_FILENAME().
void init_lassy_vms(void)
{
    // The OID to register the integer at.
    // This should be a fully qualified instance.
    // In this case, it's a scalar at:
    // NET-SNMP-EXAMPLES-MIB::netSnmpExampleInteger.0
    // Note the trailing 0 which is required for any instantiation of any scalar object.
    oid net_snmp_example_integer_oid[] = { 1, 3, 6, 1, 4, 1, 8072, 2, 1, 1, 0 };
    oid lassy_vms_short_error_status_oid[] = { 1, 3, 6, 1, 4, 1, 1206, 4, 2, 3, 9, 7, 1, 0 };

    // A debugging statement.
    // Run the agent with -Dexample_lassy_vms to see the output of this debugging statement.
    // DEBUGMSGTL(("example_lassy_vms", "Initalizing example scalar int. Default value = %ld\n", net_snmp_example_integer));
    
    // The lines below register the scalar variables above as accessible and make them writable.
    // A read only version of the same registration would merely call
    // "register_read_only_long_instance()" instead.
    // Also register a callback when the value was retrieved or set.
    // Note that the details of doing this are handled automatically.
    // Register the context name too.
    netsnmp_register_long_instance("my example int variable",
                                   net_snmp_example_integer_oid,
                                   OID_LENGTH(net_snmp_example_integer_oid),
                                   &net_snmp_example_integer,
                                   &handle_lassy_vms_object);
    netsnmp_register_long_instance_context("shortErrorStatus.0",
                                           lassy_vms_short_error_status_oid,
                                           OID_LENGTH(lassy_vms_short_error_status_oid),
                                           &lassy_vms_short_error_status,
                                           &handle_lassy_vms_object,
                                           "VMS1");

    // DEBUGMSGTL(("example_lassy_vms", "Done initalizing example scalar int\n"));
}

