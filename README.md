# COIL-CONESTOGA



# adding object files for testing:

* right click COIL-TEST and enter properties
* go to Linker -> Input -> Additioanl Dependencies
* add a new line "$(SolutionDir)COIL\\x64\\Debug<className>", you'll see other examples of it in the list



running docker image:

replace paths and image names with your local versions

docker run -ti --name development -v "D:\\Programming\\COIL-CONESTOGA:/shared" webserver\_dev\_linux:latest


// test setup
User Docker: xxxxxx
- port 5000 
docker run -ti --name user -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 5001:5000 webserver_dev_linux:latest

Primary Docker: 127.0.0.1:5000
- port 5000
- port 29000
docker run -ti --name primary -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 5000:5000 -p 29000:29000 webserver_dev_linux:latest 

Simulator: 10.172.41.150:29000
