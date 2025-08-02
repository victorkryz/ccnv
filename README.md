# $\color{MidnightBlue}\textit{\textbf{ccnv}}$


![C++](https://img.shields.io/badge/C++-17-purple?logo=C++)
![curl](https://img.shields.io/badge/curl-7.58+-lightgray?logo=curl)
![JSON](https://img.shields.io/badge/json-nlohmann-red?logo=json)
![cmake](https://img.shields.io/badge/cmake-3.30-brightgreen)
![Ubuntu](https://img.shields.io/badge/Ubuntu-18.04+-red?logo=Ubuntu)
![Windows](https://img.shields.io/badge/Windows-11-blue?logo=Windows)


Currency Converter (ccnv) is a lightweight cross-platform C++-based utility that performs currency conversion between international currencies.

#### What the app does:
Internally, the app uses a REST API to fetch exchange rates from a remote financial service [*Free Currency Exchange Rates*](https://github.com/fawazahmed0/exchange-api).  
The utility prepares and sends an HTTP request to the remote service using curl library and parses the JSON response to extract the relevant exchange rate.

### Command line arguments:

```
-l, --list     list all available currencies
-f, --from     currency convert from (usd, eur, bgn, uah, etc.)
-t, --to       currency convert to (usd, eur, bgn, uah, etc.)
-a, --amount   amount (10, 50, 100, etc.)
-v, --version  print version
-h, --help     print usage
```


### Samples of usage:

```
ccnv -l 
ccnv -f eur -t usd
ccnv -f usd -a 10 -t eur
ccnv -f usd -a 25 -t uah
ccnv --from bgn --amount 200 --to uah
```

### Used third-party code:

- "Lightweight C++ command line option parser (https://github.com/jarro2783/cxxopts)
-  Header-only nlohmann json  (https://github.com/nlohmann/json)


### How to build:
-------------------------------------------------------------------------

Project building is managed and defined by CMake.

To build the project under Linux OS use build.sh script with build type specification:

```
    ./build.sh release (debug)
```

On Windows:
  - Microsoft Visual Studio with supporting CMake-based projects feature and 
    compiler enables C++ 17 standard (since VS 2019)




