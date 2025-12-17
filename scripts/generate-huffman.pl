#!/usr/bin/env perl

use strict;
use warnings;

sub trim {
    my $str = shift;
    $str =~ s/\s+$//;
    $str =~ s/^\s+//;
    return $str;
}


my @dims = (
  0,
  4,
  4,
  4,
  4,
  2,
  2,
  2,
  2,
  2,
  2,
  2
);

my @unsigned_cb = (
  0,
  0,
  0,
  1,
  1,
  0,
  0,
  1,
  1,
  1,
  1,
  1
);

my @lavs = (
  0,
  1,
  1,
  2,
  2,
  4,
  4,
  7,
  7,
  12,
  12,
  16
);

my @codebooks = ();

sub process_codebook {
    my $codebook_number = shift;

    my $unsigned = $unsigned_cb[$codebook_number];
    my $dim = $dims[$codebook_number];
    my $lav = $lavs[$codebook_number];
    my $max_l = 0;

    my @codebook;

    my $codebook_filename;
    if($codebook_number > 0) {
        $codebook_filename = "data/codebook_${codebook_number}.txt";
    } else {
        $codebook_filename = "data/codebook_sf.txt";
    }

    open(my $fh, '<', $codebook_filename) or die $!;
    while(my $line = <$fh>) {
        $line = trim($line);
        my @vals = split(/\s+/, $line);
        while(@vals >= 3) {
            my $index = int(shift @vals);
            my $length = int(shift @vals);
            my $codeword = hex(shift @vals);
            if($length > $max_l) {
                $max_l = $length;
            }

            my $entry = { index => $index, length => $length, codeword => $codeword };
            push(@codebook, $entry);
        }
    }
    @codebook = sort { $a->{'codeword'} <=> $b->{'codeword'} } @codebook;
    $codebooks[$codebook_number] = { codebook => \@codebook, dims => $dim, length => $max_l };
}

my $total_entries = 0;

sub generate_indexes {
    my $compact = shift;

    if($compact) {
        print "/* indexed by byte value, which we'll manually unpack into the proper types */\n";
    } else {
        print "/* indexed by struct index */\n";
    }

    $total_entries = 0;
    print "static const maac_codebook_index_entry maac_codebook_indexes[12] = {";
    foreach my $i (0 .. 11) {
      my @entries = @{$codebooks[$i]->{'codebook'}};
      my $t = @entries + 0;
      if($compact) {
          if($i == 0 || $i == 11) {
              $t *= 4;
          } else {
              $t *= 3;
          }
      }
      if($i > 0) {
          print ",";
      }
      print "\n  { " . $total_entries . ", " . ($total_entries + $t) . " }";

      $total_entries += $t;
    }
    print "\n};\n\n";

    print "static const maac_codebook_entry maac_codebook\[${total_entries}\] = {";

    foreach my $i (0 .. 11) {
      my @entries = @{$codebooks[$i]->{'codebook'}};
      my $total = @entries + 0;
    
      if($i > 0) {
          print ",";
      }
    
      if($i == 0) {
          print "\n  /* scalefactor codebook -- 3-byte codeword, 1-byte index */";
      } else {
          print "\n  /* codebook $i --";
          if($i == 11) {
              print " 2-byte codeword, 2-byte index */";
          } else {
              print " 2-byte codeword, 1-byte index */";
          }
      }
    
      my $j = 0;
      foreach my $e (@entries) {
          if($j > 0) {
              print ",";
          }
          my $fmt;
          my $codeword;
          my $value;
          if($compact) {
            if($i == 0) {
                $codeword = sprintf('0x%02x, 0x%02x, 0x%02x', int($e->{'codeword'} / (256 * 256)), int($e->{'codeword'} / 256) % 256, int($e->{'codeword'} % 256));
            } else {
                $codeword = sprintf('0x%02x, 0x%02x', int($e->{'codeword'} / 256), int($e->{'codeword'} % 256));
            }
            if($i == 11) {
              $value = sprintf('0x%02x, 0x%02x', int($e->{'index'} / 256), int($e->{'index'} % 256));
            } else {
              $value = sprintf('0x%02x', $e->{'index'});
            }
            $fmt = "\n  /* %2d bits */ %s, %s";
          } else {
            $codeword = sprintf('0x%05x', $e->{'codeword'});
            $value = sprintf('%s', $e->{'index'});
            $fmt = "\n  { /* %2d bits */ %s, %s }";
          }
    
    
          my $line = sprintf($fmt,
            $e->{'length'},
            $codeword,
            $value);
          print $line;
          $j++;
      }
    }
    print "\n};\n\n";
}

foreach my $i (0 .. 11) {
  process_codebook($i);
}

print "/* SPDX-License-Identifier: 0BSD */\n";
print "#ifndef MAAC_HUFFMAN_CODEBOOKS_INCLUDE_GUARD\n";
print "#define MAAC_HUFFMAN_CODEBOOKS_INCLUDE_GUARD\n\n";

print "#include \"maac_stdint.h\"\n";
print "#include \"maac_pure.h\"\n";
print "#include \"maac_inline.h\"\n";
print "\n";

print "/* we use index 0 for the scalefactors codebook */\n";

my $total_items = 0;
print "struct maac_codebook_bits_index_entry {\n";
print "  maac_u8 start;\n";
print "  maac_u8 end;\n";
print "};\n\n";
print "typedef struct maac_codebook_bits_index_entry maac_codebook_bits_index_entry;\n\n";
print "static const maac_codebook_bits_index_entry maac_codebook_bits_indexes[12] = {";
foreach my $i (0 .. 11) {
  if($i > 0) {
      print ",";
  }
  print "\n  { " . $total_items . ", " . ( $total_items + $codebooks[$i]->{'length'}) . " }";
  $total_items += $codebooks[$i]->{'length'};
}
print "\n};\n\n";

print "static const maac_u8 maac_codebook_bits\[${total_items}\] = {";

foreach my $i (0 .. 11) {
  my @entries = @{$codebooks[$i]->{'codebook'}};
  my $j = 0;
  my $k = 0;
  my $bits = 1;

  if ($i > 0) {
      print ",";
  }

  if($i == 0) {
      print "\n  /* scalefactor codebook */";
  } else {
      print "\n  /* codebook $i */";
  }
  foreach my $e (@entries) {
      if($e->{'length'} > $bits) {
          if( ($bits-1) > 0) {
              print ",";
          }
          if( ($bits-1) % 8 == 0) {
              print "\n   ";
          }
          print " " . ( $j - $k );
          $k = $j;
          $bits++;

          while($bits < $e->{'length'}) {
              if( ($bits-1) > 0) {
                  print ",";
              }
              if( ($bits-1) % 8 == 0) {
                  print "\n   ";
              }
              print " 0";
              $bits++;
          }
          $bits = $e->{'length'};
      }
      $j++;
  }

  print ",";
  if( ($bits - 1) % 8 == 0) {
      print"\n   ";
  }
  print " " . ($j - $k);
  $bits++;
}
print "\n};\n\n";

print "struct maac_codebook_index_entry {\n";
print "  maac_u16 start;\n";
print "  maac_u16 end;\n";
print "};\n\n";
print "typedef struct maac_codebook_index_entry maac_codebook_index_entry;\n\n";


print "#ifdef MAAC_COMPACT_CODEBOOKS\n\n";

print "typedef maac_u8 maac_codebook_entry;\n\n";

generate_indexes(1);

  print "/* for scalefactor codebook and codebook 11, each entry is 4 bytes. 3 bytes otherwise */\n";
  print "static const maac_u32 maac_codebook_stride_tbl[12] = {\n";
  print  "  4,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  3,\n";
  print  "  4\n";
  print "};\n\n";
  print "#define maac_codebook_stride(x) maac_codebook_stride_tbl[x]\n\n";
  print "maac_pure static maac_inline maac_u32 maac_unpack_u24be(const maac_u8* x) {\n";
  print "  return (((maac_u32)(x[0])) << 16) |\n";
  print "         (((maac_u32)(x[1])) << 8) |\n";
  print "         (((maac_u32)(x[2])));\n";
  print "}\n\n";
  print "maac_pure static maac_inline maac_u16 maac_unpack_u16be(const maac_u8* x) {\n";
  print "  return (((maac_u16)(x[0])) << 8) |\n";
  print "         (((maac_u16)(x[1])));\n";
  print "}\n\n";
  print "maac_pure static maac_inline maac_u32 maac_codebook_codeword(maac_u32 cb, maac_u32 index) {\n";
  print "    const maac_u8* b = &maac_codebook[maac_codebook_indexes[cb].start + (index * maac_codebook_stride(cb))];\n";
  print "    return (maac_u32)(cb == 0 ? maac_unpack_u24be(b) : maac_unpack_u16be(b));\n";
  print "}\n\n";
  print "maac_pure static maac_inline maac_u32 maac_codebook_index(maac_u32 cb, maac_u32 index) {\n";
  print "    const maac_u8* b = &maac_codebook[maac_codebook_indexes[cb].start + (index * maac_codebook_stride(cb))];\n";
  print "    b += (cb == 0 ? 3 : 2);\n";
  print "    return (maac_u32)(cb == 11 ? maac_unpack_u16be(b) : b[0]);\n";
  print "}\n\n";
print "#else\n\n";

  print "struct maac_codebook_entry {\n";
  print "  maac_u32 code;\n";
  print "  maac_u16 index;\n";
  print "};\n\n";
  print "typedef struct maac_codebook_entry maac_codebook_entry;\n\n";

generate_indexes(0);

  print "#define maac_codebook_stride(x) (1)\n";
  print "#define maac_codebook_codeword(cb, i) (maac_codebook[maac_codebook_indexes[(cb)].start + (i)].code)\n";
  print "#define maac_codebook_index(cb, i) (maac_codebook[maac_codebook_indexes[(cb)].start + (i)].index)\n";

print "#endif\n\n";


print "#endif /* INCLUDE_GUARD */\n";
