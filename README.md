```
English | [简体中文](./README_zh-CN.md)
```



# UltraScan

UltraScan is a high-performance rule-matching engine, it has been created to speed up WAF rule-matching system.
Fast matching can be provided for multi-field, multi conditional combination, and rich matching operation primitives can be provided.
In production environment, the rule matching system based on UltraScan improves the performance of the original system by more than 20 times, and runs stably.

# Usage

```c  
// Define the fields to be matched and the matching operations supported by each field.
// For example, the matching operations supported by ARGS field are OPT_MATCH, OPT_LEN_EQ, and OPT_LEN_GT.
// You can see all supported matching operations in include/types.h.

field_item_t field_list[] = {
    {0, "IP", OPT_IP_IN_LIST | OPT_IP_BELONG},
    {1, "URL", OPT_INCLUDE | OPT_EQ | OPT_MATCH },
    {2, "ARGS", OPT_MATCH | OPT_LEN_EQ | OPT_LEN_GT },
    {3, "HEADER", OPT_NOT_INCLUDE | OPT_LEN_GT | OPT_LEN_LT},
};

// Create a field set
const size_t list_size = sizeof(field_list) / sizeof(field_list[0]);
fieldset = us_fieldset_create(field_list, list_size, &errptr);

// Create a rule set
ruleset = me_ruleset_create(fieldset, &errptr);

// Add a rule to the ruleset, the name of the rule is RA, and it has two conditions. 
// By default, A rule is matched only when all the conditions are matched.

// Condition 1. URL regex matches ^/foo/bar/\d$
// Condition 2. the length of ARGS is equal to 3
// RA: ( URL match ^/foo/bar/\d$) && (ARGS length_equal 3)

const char* ra_field_names[2] = {"URL","ARGS"};
match_opt_t ra_opts_ra[2] = {OPT_MATCH, OPT_LEN_EQ};
const char* ra_patterns[2] = {"^/foo/bar/\d$"，"3"};
us_ruleset_add_rule(ruleset, "RA", "deny", ra_field_names, ra_opts,ra_patterns, NULL, 2, NULL, &errptr);

// Add another rule to the ruleset, the name of the rule is RB.
// UltraScan supports logic combination. 
// RB has 3 conditions, and it will be matched when "1 and (2 or 3)"

const char* rb_field_names[3] = {"IP","HEADER", "URL"};
match_opt_t rb_opts_ra[3] = {OPT_IP_IN_LIST, OPT_NOT_INCLUDE,OPT_EQ};
const char* rb_patterns[3] = {"192.168.55.2,192.168.55.3"，"android","/test"};
const unsigned int ids[3] = {1,2,3};
const char* logic = "1 and (2 or 3)";
us_ruleset_add_rule(ruleset, "RB", "watch", ra_field_names, rb_opts,rb_patterns, ids, 2, logic, &errptr);

// After adding all the rules, compile the rule set
us_ruleset_compile(ruleset, &errptr);

// Then set the callback function, which will be called when a rule is matched
// The callback will notify the name and the tag for the rule, for example, deny and watch above.
us_ruleset_set_match_callback(ruleset, my_match_callback, NULL);

// Perform a match
input_item_t input_a[] = {
    {2, "ARGS", me_string("ABC")}, //index of ARGS is 2, see field_list above. 
    {1, "URL", me_string("/foo/bar/9")} 
};

// Obviously, input_a will match rule RA.
us_ruleset_match(ruleset, input_a, 2);

// Perform another match
input_item_t input_b[] = {
    {3, "HEADER", me_string("ios")},
    {0, "IP", me_string("192.168.55.2")}，
    {1, "URL", me_string("/xyz")}，
};

// input_b will hit rule RB because IP and HEADER were matched, and hit logic is 1 and (2 or 3).
us_ruleset_match(ruleset, input_b, 3);

// Here, you can perform other match.
// Finally, destroy the resource.
us_ruleset_destroy(ruleset);
us_fieldset_destroy(fieldset);

// The following is an implementation of the callback function.
// You can record the rule name here, stop or continue the matching process as needed, or do whatever you want.

static int my_match_callback(const char* name, const char* tag, void* user_data) {
    if(strncmp("deny",tag,4)==0) return 1; //return 1, stop the match progress.
    if(strncmp("watch",tag,5)==0) return 0; //return 0, continue the match progress.
}
```



# Getting the Source

``` bash
git clone https://github.com/chenstrace/ultrascan.git
```

# Building

This project supports [CMake](https://cmake.org/) out of the box.

## Build for POSIX

```bash
mkdir build && cd build
cmake .. && make && make install
```

### Build for Windows

First generate the Visual Studio 2017 project/solution files:

```bash
mkdir build
cd build
cmake -G "Visual Studio 15" ..
```

The default default will build for x86. For 64-bit run:

```
cmake -G "Visual Studio 15 Win64" ..
```

To compile the Windows solution from the command-line:

```
devenv /build Debug ultrascan.sln
```

or open ultrascan.sln in Visual Studio and build from within.

Please see the CMake documentation and `CMakeLists.txt` for more advanced usage.

# Repository contents

The public interface is in include/*.h. Callers should not include or rely on the details of any other header files in this package. Those internal APIs may be changed without warning.
