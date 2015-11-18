# FlashAlgo

## Setup
Skip any step where a compatible tool already exists

1. Install [Python 2.7.x](https://www.python.org/downloads/)
2. Install [pip](https://pip.pypa.io/en/latest/installing.html)
3. Install FlashAlgo dependencies

  ```
  > pip install -r requirements.txt
  ```
4. Install [Keil MDK-ARM](https://www.keil.com/download/product/)

## Develop

1. Add a folder in source/ for a new vendor if necessary.  Add source files FlashDev.c, FlashPrg.c based on template.  Modify FlashDev.c, FlashPrg.c for new target device.  Add additional header and source files as necessary.
2. Add a folder in records/projects/ for a new vendor if necessary.  Add a yaml file for a new target under the appropriate records/projects/<vendor> folder
3. Update projects.yaml to reference the new target yaml file
4. Generate project files. This should be done everytime you pull new changes
  ```
  > pgen export -t uvision
  ```
5. Open Keil uVision, open the appropriate project file at projectfiles/uvision/projectname.  Build.  

## Contribute
Check out the issue tracker.

##ToDo
- Create a test
- Document how to use
- Document how to contribute
  
