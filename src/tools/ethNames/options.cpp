/*-------------------------------------------------------------------------------------------
 * qblocks - fast, easily-accessible, fully-decentralized data from blockchains
 * copyright (c) 2018, 2019 TrueBlocks, LLC (http://trueblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
/*
 * Parts of this file were generated with makeClass. Edit only those parts of the code
 * outside of the BEG_CODE/END_CODE sections
 */
#include "options.h"

//---------------------------------------------------------------------------------------------------
static const COption params[] = {
    // BEG_CODE_OPTIONS
    // clang-format off
    COption("terms", "", "list<string>", OPT_REQUIRED | OPT_POSITIONAL, "a space separated list of one or more search terms"),  // NOLINT
    COption("expand", "e", "", OPT_SWITCH, "expand search to include all fields (default searches name, address, and symbol only)"),  // NOLINT
    COption("match_case", "m", "", OPT_SWITCH, "do case-sensitive search"),
    COption("owned", "o", "", OPT_SWITCH, "include personal accounts in the search"),
    COption("custom", "c", "", OPT_SWITCH, "include your custom named accounts"),
    COption("prefund", "p", "", OPT_SWITCH, "include prefund accounts"),
    COption("named", "n", "", OPT_SWITCH, "include well know token and airdrop addresses in the search"),
    COption("other", "t", "", OPT_HIDDEN | OPT_SWITCH, "export other addresses if found"),
    COption("addr", "a", "", OPT_SWITCH, "display only addresses in the results (useful for scripting)"),
    COption("add", "d", "<string>", OPT_HIDDEN | OPT_FLAG, "add a new record to the name database (format: grp+subgrp+addr+name+sym+src+desc)"),  // NOLINT
    COption("groups", "g", "", OPT_HIDDEN | OPT_SWITCH, "export the list of groups and subgroups only"),
    COption("", "", "", OPT_DESCRIPTION, "Query addresses and/or names of well known accounts."),
    // clang-format on
    // END_CODE_OPTIONS
};
static const size_t nParams = sizeof(params) / sizeof(COption);

string_q shortenFormat(const string_q& fmtIn);
string_q getSearchFields(const string_q& fmtIn);
//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(string_q& command) {
    if (!standardOptions(command))
        return false;

    // BEG_CODE_LOCAL_INIT
    CStringArray terms;
    bool expand = false;
    bool owned = false;
    bool custom = false;
    bool prefund = false;
    bool named = false;
    bool other = false;
    bool addr = false;
    string_q add = "";
    // END_CODE_LOCAL_INIT

    string_q format;
    bool deflt = true;
    bool addr_only = false;

    Init();
    explode(arguments, command, ' ');
    for (auto arg : arguments) {
        if (false) {
            // do nothing -- make auto code generation easier
            // BEG_CODE_AUTO
        } else if (arg == "-e" || arg == "--expand") {
            expand = true;

        } else if (arg == "-m" || arg == "--match_case") {
            match_case = true;

        } else if (arg == "-o" || arg == "--owned") {
            owned = true;

        } else if (arg == "-c" || arg == "--custom") {
            custom = true;

        } else if (arg == "-p" || arg == "--prefund") {
            prefund = true;

        } else if (arg == "-n" || arg == "--named") {
            named = true;

        } else if (arg == "-t" || arg == "--other") {
            other = true;

        } else if (arg == "-a" || arg == "--addr") {
            addr = true;

        } else if (startsWith(arg, "-d:") || startsWith(arg, "--add:")) {
            add = substitute(substitute(arg, "-d:", ""), "--add:", "");

        } else if (arg == "-g" || arg == "--groups") {
            groups = true;

        } else if (startsWith(arg, '-')) {  // do not collapse

            if (!builtInCmd(arg)) {
                return usage("Invalid option: " + arg);
            }

        } else {
            if (!parseStringList2(this, terms, arg))
                return false;

            // END_CODE_AUTO
        }
    }

    for (auto term : terms)
        searches.push_back(term);

    if (!add.empty()) {
        cout << add << endl;
        return false;
    }

    if (expand) {
        searchFields = STR_DISPLAY_ACCOUNTNAME;
        format = searchFields;
    }

    if (named) {
        if (deflt) {
            types = 0;
            deflt = false;
        }
        types |= NAMED;
    }
    if (prefund) {
        if (deflt) {
            types = 0;
            deflt = false;
        }
        types |= PREFUND;
    }
    if (custom) {
        if (deflt) {
            types = 0;
            deflt = false;
        }
        types |= CUSTOM;
    }
    if (owned) {
        if (deflt) {
            types = 0;
            deflt = false;
        }
        types |= OWNED;
    }
    if (other) {
        if (deflt) {
            types = 0;
            deflt = false;
        }
        types |= OTHER;
    }

    if (addr) {
        addr_only = true;
        isNoHeader = true;
        format = "[{ADDRESS}]";
        searchFields = "[{ADDRESS}]\t[{NAME}]";
    }

    if (verbose)
        searchFields += "\t[{SOURCE}]";

    if (groups) {
        manageFields("CAccountName:all", false);
        SHOW_FIELD(CAccountName, "group_sort");
        SHOW_FIELD(CAccountName, "raw_group");
        SHOW_FIELD(CAccountName, "subgroup_sort");
        SHOW_FIELD(CAccountName, "raw_subgroup");
        format = "[{GROUP_SORT}]\t[{RAW_GROUP}]\t[{SUBGROUP_SORT}]\t[{RAW_SUBGROUP}]";
        addr_only = false;
        types |= NAMED;
        types |= PREFUND;
        types |= CUSTOM;
        types |= OWNED;
        types |= OTHER;
    }

    // Prepare formatting
    string_q str = (format.empty() ? shortenFormat(STR_DISPLAY_ACCOUNTNAME) : format);
    if (verbose && !contains(format, "{SOURCE}"))
        str += "\t[{SOURCE}]";
    string_q meta = ", \"namePath\": \"" + (isTestMode() ? "--" : getCachePath("names/")) + "\"";

    // Display formatting
    configureDisplay("ethNames", "CAccountName", str, meta);
    if (!groups && (exportFmt == API1 || exportFmt == JSON1))
        manageFields("CAccountName:" + cleanFmt(STR_DISPLAY_ACCOUNTNAME, exportFmt));

    // Collect results for later display
    applyFilter();

    // Data wrangling
    if (addr_only) {
        HIDE_FIELD(CAccountName, "group");
        HIDE_FIELD(CAccountName, "subgroup");
        HIDE_FIELD(CAccountName, "name");
        HIDE_FIELD(CAccountName, "symbol");
        HIDE_FIELD(CAccountName, "description");
        HIDE_FIELD(CAccountName, "source");
        HIDE_FIELD(CAccountName, "logo");
        HIDE_FIELD(CAccountName, "is_contract");
        HIDE_FIELD(CAccountName, "is_private");
        HIDE_FIELD(CAccountName, "is_shared");
    }

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    registerOptions(nParams, params);
    optionOn(OPT_PREFUND | OPT_OUTPUT);

    // BEG_CODE_INIT
    match_case = false;
    groups = false;
    // END_CODE_INIT

    items.clear();
    searches.clear();
    searchFields = getSearchFields(STR_DISPLAY_ACCOUNTNAME);
    types = NAMED;
    minArgs = 0;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void) {
    setSorts(GETRUNTIME_CLASS(CBlock), GETRUNTIME_CLASS(CTransaction), GETRUNTIME_CLASS(CReceipt));

    // If you need the names file, you have to add it in the constructor
    namesFile = CFilename(configPath("names/names.txt"));
    establishFolder(namesFile.getPath());
    loadNames();
    Init();
    // BEG_CODE_NOTES
    // clang-format off
    notes.push_back("With a single search term, the tool searches both `name` and `address`.");
    notes.push_back("With two search terms, the first term must match the `address` field, and the second term must match the `name` field.");  // NOLINT
    notes.push_back("When there are two search terms, both must match.");
    notes.push_back("The `--match_case` option requires case sensitive matching. It works with all other options.");
    notes.push_back("To customize the list of names add a `custom` section to the config file (see documentation).");
    // clang-format on
    // END_CODE_NOTES
    notes.push_back("Name file: `" + substitute(namesFile.getFullPath(), getHomeFolder(), "~/") + "` (" +
                    uint_2_Str(fileSize(namesFile.getFullPath())) + ")");

    // BEG_ERROR_MSG
    // END_ERROR_MSG
}

//--------------------------------------------------------------------------------
COptions::~COptions(void) {
}

//-----------------------------------------------------------------------
bool COptions::addIfUnique(const CAccountName& item) {
    if (isZeroAddr(item.address))
        return false;

    if (groups) {
#define to_key(item) toLower(item.group + "_" + item.subgroup)
        string_q key = to_key(item);
        CAccountName found = items[key];
        string_q fkey = to_key(found);
        if (fkey != "_")
            return false;
        items[key] = item;
        return true;
    }

    address_t key = toLower(item.address);
    if (items[key].address == key) {  // it's already in the map, but we want the last in name to win
        bool empty = item.name.empty();
        bool isDifferent = items[key].name != item.name;
        bool isOwned = startsWith(items[key].name, "Owned_");
        bool isPrefund = startsWith(items[key].name, "Prefund_");
        if (!empty && (isDifferent || isOwned || isPrefund)) {
            items[key].name = item.name;
        }
        return false;
    }

    if (searches.size() == 0) {
        items[key] = item;
        return true;
    }

    if (!match_case) {
        for (size_t i = 0; i < searches.size(); i++) {
            searches[i] = toLower(searches[i]);
        }
    }

    string_q search1 = searches.size() > 0 ? searches[0] : "";
    string_q search2 = searches.size() > 1 ? searches[1] : "";
    string_q search3 = searches.size() > 2 ? searches[2] : "";

    string_q str = item.Format(searchFields);
    if (!match_case)
        str = toLower(str);

    if ((search1.empty() || search1 == "*" || contains(str, search1)) &&
        (search2.empty() || search2 == "*" || contains(str, search2)) &&
        (search3.empty() || search3 == "*" || contains(str, search3))) {
        items[key] = item;
        return true;
    }
    items.erase(key);
    return false;
}

//-----------------------------------------------------------------------
void COptions::applyFilter() {
    //------------------------
    if (types & OWNED) {
        if (isTestMode()) {
            for (uint32_t i = 1; i < 5; i++) {
                CAccountName item;
                item.address = "0x000000000000000000000000000000000000000" + uint_2_Str(i);
                addIfUnique(item);
            }
        } else {
            CStringArray addrs;
            getAccounts(addrs);
            uint32_t cnt = 0;
            for (auto addr : addrs) {
                CAccountName item;
                item.group = "00-Active";
                item.address = addr;
                item.name = "Owned_" + padNum4(cnt++);
                addIfUnique(item);
            }
        }
    }

    //------------------------
    if (types & CUSTOM) {
        if (isTestMode()) {
            for (uint32_t i = 1; i < 5; i++) {
                CAccountName item;
                item.group = "81-Custom";
                item.address = "0x000000000000000000000000000000000000000" + uint_2_Str(i);
                item.name = "Account_" + uint_2_Str(i);
                if (!(i % 2))
                    item.symbol = "AC_" + uint_2_Str(i);
                item.source = "Testing";
                addIfUnique(item);
            }
        } else {
            CAccountName item;
            string_q customStr = getGlobalConfig("ethNames")->getConfigJson("custom", "list", "");
            while (item.parseJson3(customStr)) {
                if (item.group.empty())
                    item.group = "81-Custom";
                item.name = trim(item.name, '\"');
                item.description = trim(item.description, '\"');
                addIfUnique(item);
                item = CAccountName();
            }
        }
    }

    //------------------------
    if (types & NAMED) {
        for (auto item : namedAccounts) {
            if (isTestMode() && items.size() > 200)
                break;
            addIfUnique(item);
        }
    }

    //------------------------
    if (types & PREFUND) {
        uint32_t cnt = 0;
        ASSERT(prefunds.size() == 8893);  // This is a known value

        for (auto prefund : prefundWeiMap) {
            if (isTestMode() && items.size() > 200)
                break;
            CAccountName item;
            item.group = "80-Prefund";
            item.address = prefund.first;
            item.name = "Prefund_" + padNum4(cnt++);
            item.source = "Genesis";
            addIfUnique(item);
        }
    }

    //------------------------
    if (!isTestMode() && (types & OTHER)) {
        string_q contents = asciiFileToString(configPath("names/names_custom.txt"));
        if (!contents.empty()) {
            CStringArray fields;
            fields.push_back("group");
            fields.push_back("subgroup");
            fields.push_back("address");
            fields.push_back("name");
            fields.push_back("description");
            fields.push_back("symbol");
            fields.push_back("source");
            fields.push_back("logo");
            fields.push_back("is_contract");
            fields.push_back("is_private");
            fields.push_back("is_shared");
            CAccountName item;
            while (item.parseText(fields, contents)) {
                addIfUnique(item);
                item = CAccountName();
            }
        }
    }
}

//-----------------------------------------------------------------------
string_q shortenFormat(const string_q& fmtIn) {
    string_q ret = toUpper(fmtIn);
    replace(ret, "[{SOURCE}]", "");
    replace(ret, "[{DESCRIPTION}]", "");
    replace(ret, "[{LOGO}]", "");
    replace(ret, "[{IS_CONTRACT}]", "");
    replace(ret, "[{IS_PRIVATE}]", "");
    replace(ret, "[{IS_SHARED}]", "");
    while (startsWith(ret, "\t"))
        replace(ret, "\t", "");
    while (endsWith(ret, "\t"))
        replaceReverse(ret, "\t", "");
    return ret;
}

//-----------------------------------------------------------------------
string_q getSearchFields(const string_q& fmtIn) {
    string_q ret = toUpper(fmtIn);
    replace(ret, "[{SOURCE}]", "");
    replace(ret, "[{DESCRIPTION}]", "");
    replace(ret, "[{LOGO}]", "");
    replace(ret, "[{IS_CONTRACT}]", "");
    replace(ret, "[{IS_PRIVATE}]", "");
    replace(ret, "[{IS_SHARED}]", "");
    while (startsWith(ret, "\t"))
        replace(ret, "\t", "");
    while (endsWith(ret, "\t"))
        replaceReverse(ret, "\t", "");
    return ret;
}
