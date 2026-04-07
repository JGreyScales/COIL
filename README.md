# COIL-CONESTOGA

This repo contains the development environment and testing configurations for the COIL project :)

---

## Project setup
To include object files for testing within Visual Studio, follow these steps:

1. Right click the COIL-TEST project and enter properties
2. Navigate to linker -> input -> additional dependencies
3. Add a new line using this:
   `$(SolutionDir)COIL\x64\Debug\<className>`

---

## Docker configuration
Replace the file paths and image names below with your local versions

### Standard development run
```
docker run -ti --name development -v "D:\\Programming\\COIL-CONESTOGA:/shared" webserver\_dev\_linux:latest
```

###
User Docker: xxxxxx
- port 5000  
```
docker run -ti --name secondary -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 6000:6000/udp webserver_dev_linux:latest
docker run -ti --name third -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 7000:7000 webserver_dev_linux:latest
```
Primary Docker: 127.0.0.1:5000
- port 5000
- port 29000  
```
docker run -ti --name primary -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 5000:5000 -p 29000:29000 -p 29500:29500/udp webserver_dev_linux:latest
```

---

## FOR UDP  
This is because by default docker only binds on the TCP port
```
docker run -ti --name primary -v "C:\\Programming\\COIL-CONESTOGA:/shared" -p 5000:5000/udp -p 29000:29000 -p 29500:29500/udp  webserver_dev_linux:latest
```


Simulator: 10.172.41.150:29000

Command to get an ip of a docker container:
```
docker inspect -f "{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}" <containerName>
```

command to kill program if already running: fuser -k 8080/tcp
