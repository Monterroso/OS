# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(filesize) begin
(filesize) create "test.txt"
(filesize) open "test.txt"
(filesize) end
filesize: exit(0)
EOF
pass;
