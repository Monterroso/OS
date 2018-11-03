# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(excess-stack) Argument base: 0xbfffffb0
excess-stack: exit(0)
EOF
pass;
