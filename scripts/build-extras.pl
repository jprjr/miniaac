#!/usr/bin/env perl

use strict;
use warnings;
use utf8;
use open qw[:std :utf8];

use File::Spec;
use File::Basename qw[dirname basename];
use Cwd;

# a script to take the various source files and create a single
# source file

my $source_files = {};
my @cdecls = ();

sub process_source {
    my $path = shift;
    my $doit = shift;

    my $dir = dirname($path);
    my $filename = basename($path);

    return if(exists($source_files->{$filename}));
    $source_files->{$filename} = 1;

    print STDERR "Processing $path\n";
    open(my $fh, '<', $path) or die die "Unable to open $path: $!";

    my $exclude = 0;
    my $cdecl = 0;
    my $trim = 1;
    my $last_line;
    while(my $line = <$fh>) {
        if($exclude) {
            if($line =~ m/MAAC_EXCLUDE_END/) {
                $exclude = 0;
            }
            next;
        }
        if($cdecl) {
            if($line =~ m/MAAC_CDECLS_END/) {
                $cdecl = 0;
            } else {
                push(@cdecls,$line);
            }
            next;
        }

        if($line =~ m/MAAC_EXCLUDE_BEGIN/) {
            $exclude = 1;
            next;
        }
        if($line =~ m/MAAC_CDECLS_BEGIN/) {
            $cdecl = $doit;
            next;
        }

        next if $line =~ m/SPDX-License-Identifier/;
        next if $line =~ m/INCLUDE_GUARD/;
        if($trim and $line =~ m/^\s+$/) {
            next;
        }
        my ($inc) = ($line =~ m/#include "([^"]+)"/);
        if(defined($inc)) {
            process_source(File::Spec->catfile($dir,$inc), $doit);
            next;
        }

        $trim = 0;
        if($doit) {
            print $line;
        }
        $last_line = $line;
    }

    if(defined($last_line) and $last_line !~ m/^\s*$/) {
        if($doit) {
            print "\n";
        }
    }

    close($fh);

}

if(@ARGV == 0) {
    print STDERR "Usage: $0 /path/to/main/header.h /path/to/main/source.c (...) -- /path/to/header.h source.c\n";
    exit(1);
}

my $sourcefile;

while($sourcefile = shift @ARGV) {
    last if($sourcefile eq '--');
    process_source($sourcefile, 0);
}

print STDERR "Switching to generation\n";

my $entrypoint = shift @ARGV;

print << 'EOS';
/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_EXTRAS_INCLUDE_GUARD
#define MAAC_EXTRAS_INCLUDE_GUARD

#include "maac.h"

EOS
process_source($entrypoint,1);

print "#ifdef __cplusplus\n";
print "extern \"C\" {\n";
print "#endif\n\n";
print join('',@cdecls);
print "#ifdef __cplusplus\n";
print "}\n";
print "#endif\n\n";

print << 'EOS';
#endif /* INCLUDE_GUARD */

#ifdef MAAC_EXTRAS_IMPLEMENTATION
#ifndef MAAC_EXTRAS_IMPLEMENTATION_DEFINED
#define MAAC_EXTRAS_IMPLEMENTATION_DEFINED

EOS

foreach my $source (@ARGV) {
    process_source($source,1);
}

print << 'EOS';
#endif /* EXTRAS_IMPLEMENTATION_DEFINED */
#endif
EOS

open(my $lic_fh, '<', 'LICENSE') or die $!;
print "\n\n/*\n\n";
while(my $lic_line = <$lic_fh>) {
    print $lic_line;
}
close($lic_fh);
print "\n*/\n";
