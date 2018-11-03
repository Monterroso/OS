# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(seekandtell) begin
(seekandtell) create "test.txt"
(seekandtell) open "test.txt"
(seekandtell) end
seekandtell: exit(0)
EOF
pass;
