## Creating weird ADTS files

There is a code patch in this folder (fdkaac-subframes.patch), released under a Zero-Clause BSD (0BSD),
that can be applied against [fdkaac](https://github.com/nu774/fdkaac) v1.0.6, which will add a new CLI flag:

```
-r, --raw-data-blocks <n>
```

This allows setting the number of raw data blocks (the underlying
library, fdk-aac, refers to these as "subframes") within a single
ADTS frame. I don't know of any other library/program that will
create ADTS files with multiple raw data blocks in a single frame.

Once applied, you can create a set of samples like:

```
fdkaac -b 96000 -f 2 -r 1 source.wav dest-1rdb.aac
fdkaac -b 96000 -f 2 -r 4 source.wav dest-4rdb.aac
fdkaac -b 96000 -f 2 -r 1 -C source.wav dest-1rdb-crc.aac
fdkaac -b 96000 -f 2 -r 4 -C source.wav dest-4rdb-crc.aac
```

To create ADTS files with different combinations of raw data blocks and CRC protection.

I believe the max number of raw data blocks depends on having the right
combination of channels, sample rate, bit rate, etc - you basically need
to ensure your data blocks don't exceed the max size of a single ADTS frame.

You may need to play around with different settings to successfully create
high blocks-per-frame files.
