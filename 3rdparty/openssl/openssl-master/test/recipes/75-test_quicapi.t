#! /usr/bin/env perl
# Copyright 2022 The OpenSSL Project Authors. All Rights Reserved.
#
# Licensed under the Apache License 2.0 (the "License").  You may not use
# this file except in compliance with the License.  You can obtain a copy
# in the file LICENSE in the source distribution or at
# https://www.openssl.org/source/license.html


use OpenSSL::Test::Utils;
use OpenSSL::Test qw/:DEFAULT srctop_file srctop_dir bldtop_dir/;

BEGIN {
setup("test_quicapi");
}

use lib srctop_dir('Configurations');
use lib bldtop_dir('.');

my $no_fips = disabled('fips') || ($ENV{NO_FIPS} // 0);

plan skip_all => "QUIC protocol is not supported by this OpenSSL build"
    if disabled('quic');

plan tests =>
    ($no_fips ? 0 : 1)          # quicapitest with fips
    + 1;                        # quicapitest with default provider

ok(run(test(["quicapitest", "default",
             srctop_file("test", "default.cnf")])),
             "running quicapitest");

unless ($no_fips) {
    ok(run(test(["quicapitest", "fips",
                 srctop_file("test", "fips-and-base.cnf")])),
                 "running quicapitest");
}
