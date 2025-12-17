#!/usr/bin/env perl

use strict;
use warnings;

use Data::Dumper;

sub trim {
    my $str = shift;
    $str =~ s/\s+$//;
    $str =~ s/^\s+//;
    return $str;
}

sub fmt_freq {
    my $freq = int(shift(@_));
    my $maj = int($freq / 1000);
    my $min = $freq % 1000;

    if($min == 0) {
        return "$maj kHz";
    }
    if($min % 100 == 0) {
        $min /= 100;
        return "$maj.$min kHz";
    }
    if($min % 10 == 0) {
        $min /= 10;
    }
    return "$maj.0$min kHz";
}

my @freqs = qw[
  96000
  88200
  64000
  48000
  44100
  32000
  24000
  22050
  16000
  12000
  11025
  8000
  7350
  0
  0
  0
];

my $freq_rev = {};
foreach my $i (0..$#freqs) {
    if($freqs[$i] > 0) {
        $freq_rev->{$freqs[$i]} = $i;
    }
}

sub compare_arrays {
    my $arr1 = shift;
    my $arr2 = shift;

    if($#{ $arr1 } != $#{ $arr2 }) {
        return 0;
    }

    foreach my $i (0 .. $#{ $arr1} ) {
        if($arr1->[$i] != $arr2->[$i]) {
            return 0;
        }
    }
    return 1;
}

print "/* SPDX-License-Identifier: 0BSD */\n";
print "#ifndef MAAC_SCALEFACTOR_WINDOW_BANDS_DATA_INCLUDE_GUARD\n";
print "#define MAAC_SCALEFACTOR_WINDOW_BANDS_DATA_INCLUDE_GUARD\n\n";

print "#include \"maac_stdint.h\"\n";
print "\n";

foreach my $window (qw[long short]) {
    my $swbs = {};
    my $swbs_index = {};
    my $tot = 0;
    my $group_len = 1024;
    if($window eq 'short') {
        $group_len = 128;
    }

    foreach my $freq (@freqs) {
        next if($freq == 0);

        my $swb_offsets = [];
        my $swb_file = "data/swb_offset_${window}_window_${freq}.txt";
        open(my $fh, '<', $swb_file) or die "Unable to open $swb_file: $!";
        while(my $line = <$fh>) {
            $line = trim($line);
            my @vals = split(/\s+/,$line);
            while(@vals >= 2) {
                my $idx = int(shift(@vals));
                my $offset = int(shift(@vals));
                $swb_offsets->[$idx] = $offset;
            }
        }
        close($fh);

        my $match;
        foreach my $other_freq (keys %$swbs) {
            if(compare_arrays($swbs->{$other_freq}->{'values'},$swb_offsets) == 1) {
                $match = $other_freq;
                last;
            } 
        }

        if(defined($match)) {
            push(@{ $swbs->{$match}->{'freqs'} }, $freq);
            $swbs_index->{$freq} = $swbs->{$match};
        } else {
            $swbs->{$freq} = {
                values => $swb_offsets,
                freqs => [ $freq ],
                offset => 0,
            };

            my $o = 0;
            while($o < $freq_rev->{$freq}) {
                if(exists($swbs->{$freqs[$o]})) {
                    $swbs->{$freq}->{'offset'} += @{ $swbs->{$freqs[$o]}->{'values'} };
                    # we'll be including a sentinal value */
                    $swbs->{$freq}->{'offset'} += 1;
                }
                $o++;
            }
            $swbs_index->{$freq} = $swbs->{$freq};
            # add one because we'll include a sentinal value
            $tot += @$swb_offsets + 1;
        }
    }

    print "static const maac_u8\n";
    print "maac_num_swb_${window}_window\[" . @freqs . "] = {";
    foreach my $i (0..$#freqs) {
        my $freq = $freqs[$i];
        if($i > 0) {
            print ",";
        }
        print "\n    ";
        if(exists($swbs_index->{$freq})) {
            print '' . @{ $swbs_index->{$freq}->{'values'} } . " /* " . fmt_freq(${freq}) . " */";
        } else {
            print "0 /* reserved */";
        }
    }
    print "\n};\n\n";

    print "static const maac_u16\n";
    print "maac_swb_offset_${window}_window_index\[" . @freqs . "] = {";
    foreach my $i (0..$#freqs) {
        my $freq = $freqs[$i];
        if($i > 0) {
            print ",";
        }
        print "\n    ";
        if(exists($swbs_index->{$freq})) {
            print '' . $swbs_index->{$freq}->{'offset'} . " /* " . fmt_freq($freq) . " */";
        } else {
            print "0 /* reserved */";
        }
    }
    print "\n};\n\n";

    print "static const maac_u16\n";
    print "maac_swb_offset_${window}_window\[$tot] = {";
    my $idx = 0;
    foreach my $freq (@freqs) {
        next if(not defined($swbs->{$freq}));
        next if($freq == 0);

        print "\n    /*";
        my $fidx = 0;
        foreach my $f (sort { $b cmp $a } @{ $swbs->{$freq}->{'freqs'} } ) {
            if($fidx > 0) {
                print ",";
            }
            print " " . fmt_freq($f);
            $fidx++;
        }
        print " */";
        push(@{ $swbs->{$freq}->{'values'} }, $group_len);
        foreach my $i (0..$#{ $swbs->{$freq}->{'values'} }) {
            if($i % 8 == 0) {
                print "\n   ";
            }
            my $swb = $swbs->{$freq}->{'values'}->[$i];
            print " ".$swb;
            if($idx != $tot) {
                print ",";
            }
            $idx++;
        }

        print "\n";

    }
    print "};\n\n";

}

print "#endif /* INCLUDE_GUARD */\n";
