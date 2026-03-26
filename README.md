# COIL-CONESTOGA



# adding object files for testing:

* right click COIL-TEST and enter properties
* go to Linker -> Input -> Additioanl Dependencies
* add a new line "$(SolutionDir)COIL\\x64\\Debug<className>", you'll see other examples of it in the list



running docker image:

replace paths and image names with your local versions

docker run -ti --name development -v "D:\\Programming\\COIL-CONESTOGA:/shared" webserver\_dev\_linux:latest

