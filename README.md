# trim_isoseq_polyA
This is a program to trim the polyA tails of DNA sequences in a Fasta format, using HMM.

It is primarily developed to process the "isoseq_flnc.fasta" file of PacBio Iso-Seq `classify` output,
with the HMM model been trained with that data. However, with an `-G` option, it will process any fasta file.

## Prerequisite
- C++14
- Boost
- pthread

## Install

```bash
mkdir build && cd build && \
meson && ninja
```
    Executable `trima` will be saved in directory build/src/trima.

## Usage
To process Iso-Seq `classfy` output
```bash
trima -i isoseq.flnc.fa -t 8 > isoseq.flnc.atrim.fa 2> isoseq.flnc.atrim.log
```

To process generic fasta files
```bash
trima -i input.fa -t 8 -G > input.atrim.fa 2> input.atrim.log
```
`input.atrim.fa` file contain the fasta entries with polyA trimmed, based on a default HMM model trained with PacBio data.

`input.atrim.log` is a tab file with length of polyA been trimmed.

To visualize polyA (colored red when visualized by `cat`)
```bash
trima -i isoseq.flnc.fa -t 8 -c 2>/dev/null
```

DISCLAIMER
----------
THIS WEBSITE AND CONTENT AND ALL SITE-RELATED SERVICES, INCLUDING ANY DATA, ARE PROVIDED "AS IS," WITH ALL FAULTS, WITH NO REPRESENTATIONS OR WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY, NON-INFRINGEMENT OR FITNESS FOR A PARTICULAR PURPOSE. YOU ASSUME TOTAL RESPONSIBILITY AND RISK FOR YOUR USE OF THIS SITE, ALL SITE-RELATED SERVICES, AND ANY THIRD PARTY WEBSITES OR APPLICATIONS. NO ORAL OR WRITTEN INFORMATION OR ADVICE SHALL CREATE A WARRANTY OF ANY KIND. ANY REFERENCES TO SPECIFIC PRODUCTS OR SERVICES ON THE WEBSITES DO NOT CONSTITUTE OR IMPLY A RECOMMENDATION OR ENDORSEMENT BY PACIFIC BIOSCIENCES.
