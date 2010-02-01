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
##############################################################

sub procArray {
    my ($body,$elType) = @_;
    if($elType eq "") {
	my @el = split(", ",$body);
	$" = "; ";
	printf "proc Array: >$elType<\n\t@el\n#################6\n";
    }
    return $elType;
}

sub procVector {
    my ($body,$elType) = @_;
    if($elType eq "") {
	my @el = split(", ",$body);
	$" = "; ";
	printf "proc Vector: >$elType<\n\t@el\n#################7\n";
    }
    return $elType;
}

##############################################################

sub matchVar {
    my ($dt) = @_;
    my $sDt;
    my $sVar;
    ($sVar,$sDt,$dt) = $dt =~ m/^([^ ]+) = ([^ ;]+);(.*)$/;
    $dt =~ s/^\s+//;           # no leading white
    $dt =~ s/\s+$//;           # no trailing white
    printf "*** $sVar $sDt\n#################2\n";
    #printf "*** $sVar $sDt\n$dt\n#################2\n";
    return $dt;
}

# []
sub matchArray {
    my ($dt,$elType) = @_;
    my ($x,$rest) = $dt =~ m/(\[(?:[^\[\]]++ | (?1))*\]);(.*)$/xg;
    $x =~ s/^\[//; $x =~ s/\]$//;
    $x =~ s/^ //; $x =~ s/ $//;
    $rest =~ s/^ //; $rest =~ s/ $//;
    procArray($x,$elType);
    printf "$x\n#################4\n";
    #printf "$x\n$rest\n#################4\n";
    return $rest;
}

# []
sub matchVector {
    my ($dt,$elType) = @_;
    my ($x,$rest) = $dt =~ m/(\[(?:[^\[\]]++ | (?1))*\]);(.*)$/xg;
    $x =~ s/^\[//; $x =~ s/\]$//;
    $x =~ s/^ //; $x =~ s/ $//;
    $rest =~ s/^ //; $rest =~ s/ $//;
    procVector($x,$elType);
    printf "$x\n#################5\n";
    #printf "$x\n$rest\n#################5\n";
    return $rest;
}

# {}
sub matchStruct {
    my ($body) = @_;
    my ($struct,$rest) = $body =~ m/(\{(?:[^{}]++ | (?1))*\});(.*)$/xg;
    $struct =~ s/^\{//; $struct =~ s/\}$//;
    $struct =~ s/^ //; $struct =~ s/ $//;
    $rest =~ s/^ //; $rest =~ s/ $//;
    printf "$struct\n#################4\n";
    #printf "$struct\n$rest\n#################3\n";
    while(length($struct) > 0) {
	$struct = procBody($struct);
    }
    return $rest;
}

##############################################################

sub procBody {
    my $rr;
    my ($body) = @_;
    if($body =~ m/^TArray/) {
	my $elType = "";
	my ($var,$body) = $body =~ m/TArray_([^\[]+)(.*)$/;
	printf "matchArray ->$var< >elType<\n";
	$rr = matchArray($body,$elType);
    } elsif($body =~ m/^(TFVector|TVector)/) {
	my $elType = "";
	my ($vType,$var,$body) = $body =~ m/(TFVector|TVector)_([^\[]+)(.*)$/;
	printf "matchVector ->$vType< >$var< >elType<\n";
	$rr = matchVector($body,$elType);
    } elsif($body =~ m/^(CStruct|CRecrd)_/) {
	printf "matchStruct ->\n";
	#printf "matchStruct ->$body\n";
	$rr = matchStruct($body);
    } else {
	printf "matchVar ->\n";
	#printf "matchVar ->$body\n";
	$rr = matchVar($body);
    }
    #printf "procBody >$rr<\n#################38\n";
    return $rr;
}

##############################################################

my $data = "";
my $Proc = "";
my $Body0 = "";

while (<STDIN>) {
    chomp;              # no newline
    s/#.*//;            # no comments
    s/^\s+//;           # no leading white
    s/\s+$//;           # no trailing white
    next unless length; # anything left?

    $data .= " $_";     # All in one line
    $data =~ s/ ;/;/g;

    $data =~ s/}/ }/g;
    $data =~ s/\{/{ /g;
    $data =~ s/]/ ]/g;
    $data =~ s/\[/[ /g;
    $data =~ s/\s+/ /g; # reduce to one space
}
printf "\n$data\n\n";

# {} - Top level
my ($type,$var,$body) = $data =~ m/(CMsg|CStruct|CRecrd)_([^{} ]+) (.+)$/;
#printf "Type-$type  Var-$var\n";
my @gr = $body =~ m/(\{(?:[^{}]++ | (?1))*\})/xg;
#$" = "\n==>>\n";
#printf "Found:\n@gr\n#################\n";

##############################################################

$Proc = "using namespace MsgModel;\n\nchar set$type\_$var($type\Gen_$var \& _gMsg_) {
  $Body0
}\n";
#printf "$Proc\n#################\n";

($Body0) = @gr;
$Body0 =~ s/^\{//; $Body0 =~ s/\}$//;
$Body0 =~ s/^ //; $Body0 =~ s/ $//;

my $d = 0;
if($Body0 =~ /^(?:\{(?{$d++})|\}(?{$d--})(?(?{$d<0})(?!))|(?>[^{}]*))*(?(?{$d!=0})(?!))$/x) {
    while($Body0 ne "") {
	printf "$Body0\n#################0\n\n";
	$Body0 = procBody($Body0);
    }
} else {
    printf "Error:\n\tNot balanced >$Body0< $d\n";
}

##############################################################

if(0) {
$d = 0;
if($data =~ /^(?:\{(?{$d++})|\}(?{$d--})(?(?{$d<0})(?!))|(?>[^{}]*))*(?(?{$d!=0})(?!))$/x) {

    my $regex = qr/(\{(?:[^{}]++ | (?1))*\})/x;
    my @queue = $data;
    $" = "\n==>>\n";
    while( @queue ) {
	my $string = shift @queue;
	my @groups = $string =~ m/$regex/g;
	printf ">>Found:\n@groups\n******\n" if @groups;
	unshift @queue, map { s/^\{//; s/\}$//; $_ } @groups;
    }
} else {
    printf "Error:\n\tNot balanced >$data< $d\n";
}
}
##############################################################
# $Id: modelGen.pl 321 2010-01-16 14:32:18Z asus $
