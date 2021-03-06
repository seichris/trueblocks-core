/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * copyright (c) 2018, 2019 TrueBlocks, LLC (http://trueblocks.io)
 * All Rights Reserved
 *------------------------------------------------------------------------*/
#include "etherlib.h"
#include "options.h"

#define RETURN(a)                                                                                                      \
    bool ret = (a);                                                                                                    \
    if (!getEnvStr("TEST_MODE").empty()) {                                                                             \
        EXIT_NOMSG8(0);                                                                                                \
    } else {                                                                                                           \
        EXIT_NOMSG8(ret);                                                                                              \
    }

//--------------------------------------------------------------
int main(int argc, const char* argv[]) {
    nodeNotRequired();  // not every command needs a node
    acctlib_init(quickQuitHandler);

    ENTER8("chifra");
    COptions options;
    if (!options.prepareArguments(argc, argv)) {
        RETURN(1);
    }

    for (auto command : options.commandLines) {
        if (!options.parseArguments(command)) {
            RETURN(1);
        }

        if (options.mode == "list") {
            RETURN(options.handle_list())  //
        } else if (options.mode == "export") {
            RETURN(options.handle_export())  //
        } else if (options.mode == "leech") {
            RETURN(options.handle_leech())
        } else if (options.mode == "scrape") {
            RETURN(options.handle_scrape())  //
        } else if (options.mode == "slurp") {
            RETURN(options.handle_slurp())  //
        } else if (options.mode == "quotes") {
            RETURN(options.handle_quotes())
        } else if (options.mode == "status") {
            RETURN(options.handle_status())
        } else if (options.mode == "data") {
            RETURN(options.handle_data())
        } else if (options.mode == "state") {
            RETURN(options.handle_data())
        } else if (options.mode == "settings") {
            RETURN(options.handle_settings())  //
        } else if (options.mode == "rm") {
            RETURN(options.handle_rm())  //
        } else if (isTestMode()) {
            map<string, string> cmdMap;
            cmdMap["where"] = "whereBlock";
            cmdMap["tokens"] = "getTokenInfo";
            cmdMap["blooms"] = "getBloom";
            if (cmdMap[options.mode] != "") {
                ostringstream os;
                os << cmdMap[options.mode] << " " << options.tool_flags;
                for (auto addr : options.addrs)
                    os << " " << addr;
                LOG_CALL(os.str());
                // clang-format off
                if (system(os.str().c_str())) {}  // Don't remove cruft. Silences compiler warnings
                // clang-format on
            } else {
                cerr << "Should not happen.";
            }
        }
    }

    acctlib_cleanup();
    EXIT_NOMSG8(0);
}
