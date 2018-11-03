# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(nullpoint) begin
(nullpoint) create "test.txt"
(nullpoint) open "test.txt"
nullpoint: exit(-1)
EOF
pass;
