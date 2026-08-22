This tool is a C++ CLI interface that fetches a Github's user activity. 
It uses the curl header for performing HTTPs GET requests and nlohmann/json.hpp header to handle json parsing.
\n It's a solution to a [Roadmap.sh]{https://roadmap.sh/projects/github-user-activity}'s project. 

##Requirements
##Running the executable (.exe)
- Windows 11/10(64-bit)
- Need the following dependencies in the application folder
  a) `libcurl.dll` (and any other associated SSL\zlib dependencies)
  b) `curl-ca-bundle.crt`
##Building dependencies
- **Compiler**GCC v10+(C++17 standard support required)
- **Libraries** 
  - `libcurl` -> <curl/curl.h>
  - `nlohmann/json` -> <json.hpp>

#Quick Start
*(Optional)* Set your github token api in the powershell/command line to raise API limits
- In the powershell/command line, run the following:
  - `g++ -std=c++17 -Wall -Wextra -Wno-unknown-pragmas -I C:/msys64/ucrt64/include -I C:/Programming/C++/Libs/vcpkg gh.cpp -L C:/msys64/ucrt64/lib -lcurl 
    -o output/gh.exe`
**NB** Before running, ensure the dependencies a) and b) are present in the output folder. 
