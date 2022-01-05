
# List of targets
# Each target has a user-defined name and an associated command to be executed 

targets = [

    { "name" : "date" , "command" : "./date.exe"  },
    { "name" : "periodes_ids" , "command" : "./periodes_ids.exe"  },
    { "name" : "heures" , "command" : "./heures.exe"  },
    { "name" : "outils" , "command" : "./outils.exe"  },
    { "name" : "afficheur" , "command" : "./afficheur.exe"  },
    { "name" : "par_lettres" , "command" : "./par_lettres.exe"  },
    { "name" : "type_entier" , "command" : "./type_entier.exe"  },
    { "name" : "collection_rangee" , "command" : "./collection_rangee.exe"  },
    { "name" : "fichier_table" , "command" : "./fichier_table.exe ../input/candidats.csv"  },
    { "name" : "fichier_pdf" , "command" : "./fichier_pdf.exe"  },
    { "name" : "configurable" , "command" : "./configurable.exe"  },
    { "name" : "random" , "command" : "./random.exe"  },

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

