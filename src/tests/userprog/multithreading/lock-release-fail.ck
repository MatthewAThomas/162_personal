# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected (IGNORE_USER_FAULTS => 1, [<<'EOF']);
(lock-release-fail) begin
lock-release-fail: exit(1)
EOF
pass;
