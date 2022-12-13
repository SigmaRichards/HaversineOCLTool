# OpenCL Haversines

This is a tool for calculating haversine distances between coordinates. This is a tool being used as part of a personal project and thus may have limited applicability beyond that. The tool is designed to take 2 CSV files (lets say A and B) and compute all paired distances from A to B. The tool is partially working however there are a number of caveats based on the very early stage of this project. See below.

## RapidCSV

In this project I make use of Kristofer Berggren's rapidcsv header. This is available [here]{https://github.com/d99kris/rapidcsv}. This source code is made available under BSD 3-Clause license.


## Usage

General usage is as below:

```./haversines fileA.csv fileB.csv```

The output will compute the distance from every point in A to every point in B and return it in a list written to `out.csv`.

## Current limitations

Currently the program is very limited in terms of usage. The input CSVs must have a header row and a row name. Additionally, the column for Latitude must be labelled "Lat" and the column for Longitude must be labelled "Lon". This is for both CSVs. For reference, you can look at the test CSVs.

Additionally the output labels are based on index from A to B, they do not currently depend on any labels from the CSVs.

## Future work

Here are a list of future modifications I plan to make to the tool to aid usability. Some of these are specific to my future use cases and some are just general usability improvements.

 - [ ] Improve CSV support - this includes things like header options, label support on output and default values (to avoid seg faults).
 - [ ] Single argument format - this is so the distance matrix can be computed for a single csv instead of requiring a second CSV. This can be achieved by making A and B the same CSV however this should remove redundant computations i.e. $D(x,x)$ = 0 and $D(x,y)=D(y,x)$.
 - [ ] Closest point mode - where only the distance to the closest other point is reported as opposed to all points.



