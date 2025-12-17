.PHONY: all dist test-compile libs clean demos

CC = cc
CC_STANDARD = c89

HOST_CC = cc
HOST_CC_STANDARD = c11
HOST_CFLAGS = -std=$(HOST_CC_STANDARD) -Wall -Wextra -g -O2

# pretty normal CFLAGS
CFLAGS = -std=$(CC_STANDARD) -pedantic -g -O2 -Wall -Wextra -Wdouble-promotion -Winline -fPIC
LDFLAGS = -lm

# hardcore CFLAGS and LDFLAGS for debugging/testing/etc
#CFLAGS = -std=$(CC_STANDARD) -pedantic -g -O0 -Wall -Wextra -Wdouble-promotion -Winline -fPIC -DMAAC_COMPACT -DMAAC_NO_STDMATH -DMAAC_NO_STDSTRING -DMAAC_ENABLE_ASSERT -fsanitize=undefined
#LDFLAGS = -fsanitize=undefined

HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.c)

GENERATED_HEADERS = \
	src/bit_reverse_data.h \
	src/huffman_codebooks.h \
	src/scalefactor_band_data.h \
	src/imdct_window_data.h \
	src/imdct_twiddle_factors.h \
	src/tns_invquant_data.h \
	src/inv_quant_tables.h

MAIN_SOURCES = \
	src/adts.c \
	src/bitreader.c \
	src/channel.c \
	src/channel_config.c \
	src/cpe.c \
	src/fil.c \
	src/filterbank.c \
	src/huffman.c \
	src/ics.c \
	src/ics_info.c \
	src/imdct.c \
	src/maac_frequency.c \
	src/maac_math.c \
	src/maac_memcpy.c \
	src/maac_memset.c \
	src/maac_rand.c \
	src/pns.c \
	src/pulse.c \
	src/raw.c \
	src/scalefactor_bands.c \
	src/sce.c \
	src/tns.c \
	src/window_group_lengths.c

MAIN_OBJS = $(MAIN_SOURCES:%.c=%.o)

EXTRAS_SOURCES = \
	src/adts_extras.c \
	src/bitreader_extras.c \
	src/channel_extras.c \
	src/cpe_extras.c \
	src/fil_extras.c \
	src/ics_extras.c \
	src/ics_info_extras.c \
	src/maac_consts_extras.c \
	src/maac_result_extras.c \
	src/raw_extras.c \
	src/sce_extras.c

EXTRAS_OBJS = $(EXTRAS_SOURCES:%.c=%.o)

OBJS = $(SOURCES:%.c=%.o)

CODEBOOK_FILES = \
	data/codebook_sf.txt \
	data/codebook_1.txt \
	data/codebook_2.txt \
	data/codebook_3.txt \
	data/codebook_4.txt \
	data/codebook_5.txt \
	data/codebook_6.txt \
	data/codebook_7.txt \
	data/codebook_8.txt \
	data/codebook_9.txt \
	data/codebook_10.txt \
	data/codebook_11.txt

SCALEFACTOR_OFFSET_FILES = \
	data/swb_offset_short_window_96000.txt \
	data/swb_offset_short_window_88200.txt \
	data/swb_offset_short_window_8000.txt \
	data/swb_offset_short_window_64000.txt \
	data/swb_offset_short_window_48000.txt \
	data/swb_offset_short_window_44100.txt \
	data/swb_offset_short_window_32000.txt \
	data/swb_offset_short_window_24000.txt \
	data/swb_offset_short_window_22050.txt \
	data/swb_offset_short_window_16000.txt \
	data/swb_offset_short_window_12000.txt \
	data/swb_offset_short_window_11025.txt \
	data/swb_offset_long_window_96000.txt \
	data/swb_offset_long_window_88200.txt \
	data/swb_offset_long_window_8000.txt \
	data/swb_offset_long_window_64000.txt \
	data/swb_offset_long_window_48000.txt \
	data/swb_offset_long_window_44100.txt \
	data/swb_offset_long_window_32000.txt \
	data/swb_offset_long_window_24000.txt \
	data/swb_offset_long_window_22050.txt \
	data/swb_offset_long_window_16000.txt \
	data/swb_offset_long_window_12000.txt \
	data/swb_offset_long_window_11025.txt

all: dist libs demos

src/huffman_codebooks.h: scripts/generate-huffman.pl $(CODEBOOK_FILES)
	perl $< > $@

src/scalefactor_band_data.h: scripts/generate-scalefactor-bands.pl $(SCALEFACTOR_OFFSET_FILES)
	perl $< > $@

src/inv_quant_tables.h: scripts/generate-quant-tables
	./scripts/generate-quant-tables > $@

scripts/generate-quant-tables: scripts/generate-quant-tables.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -lm

src/bit_reverse_data.h: scripts/generate-bitreverse-tables
	./scripts/generate-bitreverse-tables > $@

scripts/generate-bitreverse-tables: scripts/generate-bitreverse-tables.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -lm

src/imdct_window_data.h: scripts/generate-windows
	./scripts/generate-windows > $@

scripts/generate-windows: scripts/generate-windows.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -lm

src/tns_invquant_data.h: scripts/generate-tns-tables
	./scripts/generate-tns-tables > $@

scripts/generate-tns-tables: scripts/generate-tns-tables.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -lm

src/imdct_twiddle_factors.h: scripts/generate-imdct-twiddles
	./scripts/generate-imdct-twiddles > $@

scripts/generate-imdct-twiddles: scripts/generate-imdct-twiddles.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ $< -lm

libs: libmaac.so libmaac.a libmaac_extras.so libmaac_extras.a libmaac-singlefile.so

src/ics.o: src/ics.c src/inv_quant_tables.h
	$(CC) $(CFLAGS) -o $@ -c $<

src/huffman.o: src/huffman.c src/huffman_codebooks.h src/scalefactor_band_data.h
	$(CC) $(CFLAGS) -o $@ -c $<

src/filterbank.o: src/filterbank.c src/imdct_window_data.h
	$(CC) $(CFLAGS) -o $@ -c $<

src/imdct.o: src/imdct.c src/imdct_twiddle_factors.h src/bit_reverse_data.h
	$(CC) $(CFLAGS) -o $@ -c $<

src/tns.o: src/tns.c src/tns_invquant_data.h
	$(CC) $(CFLAGS) -o $@ -c $<

libmaac-singlefile.so: .build/maac.o
	$(CC) -shared -o $@ $(LDFLAGS) $^

libmaac.so: $(MAIN_OBJS)
	$(CC) -shared -o $@ $(LDFLAGS) $^

libmaac.a: $(MAIN_OBJS)
	$(AR) rcs $@ $^

libmaac_extras.so: $(EXTRAS_OBJS)
	$(CC) -shared -o $@ $(LDFLAGS) $^

libmaac_extras.a: $(EXTRAS_OBJS)
	$(AR) rcs $@ $^

.build/maac.o: dist/maac.c dist/maac.h | .build
	$(CC) $(CFLAGS) -DMAAC_PRIVATE=static -c -o $@ $(LDFLAGS) $<

.build:
	mkdir -p .build

demos: demos/decode-raw demos/decode-adts

demos/decode-raw: demos/decode-raw.o demos/adts_reader.o demos/wav.o
	$(CC) -o $@ $^ $(LDFLAGS)

demos/decode-adts: demos/decode-adts.o demos/wav.o
	$(CC) -o $@ $^ $(LDFLAGS)

demos/%.o: demos/%.c dist/maac.h dist/maac_extras.h
	$(CC) $(CFLAGS) -Idist -o $@ -c $<

test-compile: $(OBJS)

dist: dist/maac.h dist/maac_extras.h

dist/maac.c: dist/maac.h
	echo "#define MAAC_IMPLEMENTATION" > $@
	echo '#include "maac.h"' >> $@

dist/maac.h: scripts/build.pl $(HEADERS) $(SOURCES) $(GENERATED_HEADERS)
	mkdir -p dist
	perl scripts/build.pl src/maac.h $(MAIN_SOURCES) > $@

dist/maac_extras.h: scripts/build.pl $(HEADERS) $(SOURCES) $(GENERATED_HEADERS)
	mkdir -p dist
	perl scripts/build-extras.pl src/maac.h $(MAIN_SOURCES) -- src/maac_extras.h $(EXTRAS_SOURCES) > $@


test-struct-sizes: tests/struct-sizes
	./tests/struct-sizes

tests/struct-sizes: tests/struct-sizes.o tests/pfi.o
	$(CC) -o $@ $^ $(LDFLAGS)

tests/struct-sizes.o: tests/struct-sizes.c dist/maac.h dist/maac_extras.h
	$(CC) $(CFLAGS) -Idist -o $@ -c $<

tests/imdct-validate: tests/imdct-validate.o tests/imdct.o tests/maac_memcpy.o tests/maac_math.o
	$(CC) -o $@ $^ $(LDFLAGS) -lm

tests/math-validate: tests/math-validate.o tests/maac_math.o tests/maac_memcpy.o
	$(CC) -o $@ $^ $(LDFLAGS) -lm

tests/math-validate.o: tests/math-validate.c
	$(CC) $(CFLAGS) -Isrc -o $@ -c $<

tests/maac_math.o: src/maac_math.c src/maac_math.h src/maac_float.h
	$(CC) $(CFLAGS) -o $@ -c $<

tests/imdct-validate.o: tests/imdct-validate.c
	$(CC) $(CFLAGS) -Isrc -o $@ -c $<

tests/imdct.o: src/imdct.c src/imdct.h src/imdct_twiddle_factors.h src/bit_reverse_data.h
	$(CC) $(CFLAGS) -o $@ -c $<

tests/maac_memcpy.o: src/maac_memcpy.c src/maac_memcpy.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f dist/maac.h dist/maac.c
	rm -f dist/maac_extras.h dist/maac_extras.c
	rm -rf $(GENERATED_HEADERS)
	rm -f scripts/generate-windows.o scripts/generate-windows scripts/generate-windows.exe
	rm -f scripts/generate-bitreverse-tables.o scripts/generate-bitreverse-tables scripts/generate-bitreverse-tables.exe
	rm -f scripts/generate-imdct-twiddles.o scripts/generate-imdct-twiddles scripts/generate-imdct-twiddles.exe
	rm -f scripts/generate-tns-tables.o scripts/generate-tns-tables scripts/generate-tns-tables.exe
	rm -f scripts/generate-quant-tables.o scripts/generate-quant-tables scripts/generate-quant-tables.exe
	rm -f $(OBJS)
	rm -f libmaac.a libmaac.so
	rm -f libmaac_extras.a libmaac_extras.so
	rm -f libmaac-singlefile.a libmaac-singlefile.so
	rm -rf .build
	rm -f tests/struct-sizes tests/struct-sizes.exe
	rm -f tests/imdct-validate tests/imdct-validate.exe
	rm -f tests/math-validate tests/math-validate.exe
	rm -f tests/*.o
	rm -f demos/decode-raw decode-raw.exe
	rm -f demos/decode-adts decode-adts.exe
	rm -f demos/*.o
