
# List of targets
# Each target has a user-defined name and an associated command to be executed 

targets = [

    { "name" : "strong_key" , "command" : "./strong_key.exe"  },
    { "name" : "strong_int" , "command" : "./strong_int.exe"  },
    { "name" : "strong_array" , "command" : "./strong_array.exe"  },
    { "name" : "frequent_strings" , "command" : "./frequent_strings.exe"  },
    { "name" : "glossary" , "command" : "./glossary.exe"  },
    { "name" : "chunks_file" , "command" : "time ./chunks_file.exe"  },
    { "name" : "ranked_collection" , "command" : "./ranked_collection.exe"  },

]

# List of line filters
# Each filter has a predefined name, with a regular expression describing
# the lines to be filtered, and an "oval expression" describing the targets
# which are affected by the filter. An "oval expression" uses the
# character "%" as wildcard.
# The predefined filters are :
# * run_filters_out : remove some lines from the target ouput
# * diff_filters_in : the regular expression describe the lines of
#   interest when we compare the current output with the expected reference one.

run_filters_out = [ ]
diff_filters_in = [ {"name": "all", "re": "^(.*)$", "apply": "%"} ]

