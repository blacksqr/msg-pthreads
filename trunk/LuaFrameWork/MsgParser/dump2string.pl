#!/usr/bin/perl
#-w
# The contents of this file are subject to the Mozilla Public License
# Version 1.1 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
# License for the specific language governing rights and limitations
# under the License.
# The Initial Developer of the Original Code is Alex Goldenstein.
# All Rights Reserved.
# Contributor(s): Alex Goldenstein.<goldale.de@googlemail.com>
#
#use warnings;
use strict;

my $data = "";

while (<STDIN>) {
    chomp;              # no newline
    s/#.*//;            # no comments
    s/^\s+//;           # no leading white
    s/\s+$//;           # no trailing white
    next unless length; # anything left?

    $data .= " $_";     # All in one line
}

$data =~ s/}/ }/g;
$data =~ s/\{/{ /g;
$data =~ s/]/ ]/g;
$data =~ s/\[/[ /g;
$data =~ s/\s+/ /g; # reduce to one space
$data =~ s/ ;/;/g;
$data =~ s/ ,/;/g;
$data =~ s/"/\\"/g;
$data =~ s/^ //;

printf "\n$data\n\n";

# $Id$
