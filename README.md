A set of classes so to handle csv-like content, but split into chunks that one can edit and rearrange easily in different files.

It is also expected that some strings will be reused rather often, and should be replaced with an index in a global table in memory, so to fasten strings comparison.