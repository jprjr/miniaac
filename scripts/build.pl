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
            $cdecl = 1;
            next;
        }

        next if $line =~ m/SPDX-License-Identifier/;
        next if $line =~ m/INCLUDE_GUARD/;
        if($trim and $line =~ m/^\s+$/) {
            next;
        }
        my ($inc) = ($line =~ m/#include "([^"]+)"/);
        if(defined($inc)) {
            process_source(File::Spec->catfile($dir,$inc));
            next;
        }

        $trim = 0;
        print $line;
        $last_line = $line;
    }

    if(defined($last_line) and $last_line !~ m/^\s*$/) {
        print "\n";
    }

    close($fh);

}

if(@ARGV == 0) {
    print STDERR "Usage: $0 /path/to/header.h source.c\n";
    exit(1);
}

my $entrypoint = shift @ARGV;

print << 'EOS';
/* SPDX-License-Identifier: 0BSD */
#ifndef MAAC_INCLUDE_GUARD
#define MAAC_INCLUDE_GUARD

/*

# M(ini)aac

- a single-file, no allocation AAC decoder.

## Building

To use:

In one C file define MAAC_IMPLEMENTATION before including this header:

    #define MAAC_IMPLEMENTATION
    #include "maac.h"

You can customize parts of the build with a few more defines:
    #define MAAC_DOUBLE_PRECISION

    - By default math is done with 32-bit floats, this enables 64-bit doubles.

    #define MAAC_NO_STDMATH

	- this will disable all standard math library functions.
	- If you define this - I think there may be some undefined
	  behavior involved (the library will manually create some floats
	  when computing scale factors).

	#define MAAC_NO_STDSTRING
	- disables the use of memset and memcpy

    #define MAAC_ENABLE_ASSERT
	- enables assertions throughout the code

    #define MAAC_COMPACT_CODEBOOKS
	- attempts to reduce the size of huffman codebooks, last I checked
	  this reduces the storage from about 11kb to 4kb give or take.

	#define MAAC_COMPACT
	- tries to reduce some struct sizes by using bitfields. In hindsight
	  I don't think this really saves all that much space.

	#define MAAC_INVQUANT_TABLES
    - replaces inverse quantization functions with a lookup table. This
      is another one where in hindsight, I don't know if it actually
      speeds anything up, and it winds up making the library about 32kb
      larger. It may not be worth it.

You'll want to ensure you have the same defines enabled anytime you
include the header - whether or not you have MAAC_IMPLEMENTATION defined -
because some of those defines affect struct definitions.

There's also an "maac_extras.h" library, which has functions that aren't
100% essential for decoding, but which may make your life easier. It
provides function for turning various enums into strings, and for FFI-type
purposes, querying the sizes and alignments of structs, functions to
ensure structs are aligned, and setters and getters for struct fields.

    #define MAAC_EXTRAS_IMPLEMENTATION
    #include "maac_extras.h"

## Using

You'll need a total of 3 structs which you can allocate however
you like:

1. Either an maac_adts or maac_raw, depending on if you plan to decode
   AAC in ADTS, or raw data blocks of AAC respectively.

   - If you use maac_raw, you have to configure the sampling frequency index.
     This can be done via suppling AudioSpecificConfig bytes to maac_raw_config,
     or just setting the sf_index value directly. This should be the only config
     needed.

2. An maac_bitreader, which needs three parameters set:
   - a pointer to a data buffer (maac_bitreader.data)
   - a length value (maac_bitreader.len)

   Throughout decoding you'll "refill" the buffer, usually by loading new
   data into your buffer and updating the "pos" and "len" values.

3. An array of maac_channel objects, one for each channel of audio you plan
   to decode.

You'll call sync and decode functions. If they return 0 (MAAC_CONTINUE),
that means they require more data. Refill the bitreader and try again.

Otherwise - 1 (MAAC_OK) indicates that things were successful and you can now do stuff.

I have examples for decoding ADTS streams and raw AAC data blocks in the demos folder,
but a high-level pseudocode overview of decoding is:

MAAC_RESULT res;

maac_adts a;
maac_bitreader br;
maac_channel ch[2];

maac_adts_init(&a);
maac_bitreader_init(&br);
maac_channel_init(&ch[0]);
maac_channel_init(&ch[1]);

a.raw.out_channels = ch;
// or maac_adts_set_out_channels(&a, ch);
a.raw.num_out_channels = 2
// or maac_adts_set_num_out_channels(&a, 2);

while(i_have_data()) {
    while( (res = maac_adts_decode(&adts, &br)) == MAAC_CONTINUE) {
        br.data = get_data_somehow();
        br.len = length_of_that_data();
        br.pos = 0;
    }
    if(res != MAAC_OK) {
        ... error out
    }
    for(c=0;c<2;c++) {
        for(i=0;i<ch[c].n_samples;i++) {
            // do something with samples in ch[c].samples[i]
        }
    }
}

## Limitations

Right now pulse data isn't supported, I'm unsure how to create files
that have it, so I haven't been able to test.

LC profile only. This does not support any other profiles, though
I do like the idea of trying to implement SBR and/or PS to support
HE-AAC and HE-AACv2.

This is *probably* only going to work with mono and stereo streams. I've
only implemented decoding for Single Channel Element, Channel Pair Element,
and Fill Element. I think I can add LFE pretty easily - my understanding
is it's just a Single Channel Element with some limitations.
I think higher channel counts may rely on channel coupling elements which
I've not implemented and not sure if I plan to.

## LICENSE

0BSD

*/

EOS
process_source($entrypoint);

print "#ifdef __cplusplus\n";
print "extern \"C\" {\n";
print "#endif\n\n";
print join('',@cdecls);
print "#ifdef __cplusplus\n";
print "}\n";
print "#endif\n\n";

print << 'EOS';
#endif /* INCLUDE_GUARD */

#ifdef MAAC_IMPLEMENTATION
#ifndef MAAC_IMPLEMENETATION_DEFINED
#define MAAC_IMPLEMENETATION_DEFINED

EOS

foreach my $source (@ARGV) {
    process_source($source);
}

print << 'EOS';
#endif /* IMPLEMENTATION_DEFINED */
#endif
EOS

open(my $lic_fh, '<', 'LICENSE') or die $!;
print "\n\n/*\n\n";
while(my $lic_line = <$lic_fh>) {
    print $lic_line;
}
close($lic_fh);
print "\n*/\n";
