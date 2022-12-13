# OpenCL Haversines

This is a tool for calculating haversine distances between coordinates. This is a tool being used as part of a personal project and thus may have limited applicability beyond that. The tool is designed to take 2 CSV files (lets say A and B) and compute all paired distances from A to B.

## RapidCSV

In this project I make use of Kristofer Berggren's rapidcsv header. This is available [here]{https://github.com/d99kris/rapidcsv}. This source code is made available under BSD 3-Clause license.


## Usage

General usage is as below:

```./haversines file1.csv file2.csv [options]```

The output will compute the distance from every point in file1 to every point in file2 and return it in a list written to `out.csv`.

## Options

Options are only required if using a different format csv.

 - `-v` verbose mode. Prints input parameters or defaults where not specified.
 - `-c` and `-r` turn off column headers and row headers. CSVs are assumed to have both of these as the first row and column respectively. If row headers are available, the output file will use these names toto show each pair. If column headers are available, you can specify column names for lat/lon.
 - `-n` nearest point mode. Use this to return only the nearest pairs of points. All points in file1 will be in the output, and each corresponding nearest point in file2.
 - `s` single csv mode. Use this if distance matrix on a single csv is required. Will skip $D(x,x)$ and one of $D(x,y),D(y,x)$ in the output file.
 - `-a` and `-A` specify the column name for the latitudes. Use `-a` for file1 and `-A` for file2. Alternatively can use `-i` and `-I` for the column index. If no row headers, the first column is indexed 0, otherwise the second column is indexed 0.
 - `-b` and `-B` specify the column name for the longitudes. Use `-b` for file1 and `-B` for file2. Similarly as above can use `-j` and `-J` to specify the column index.



