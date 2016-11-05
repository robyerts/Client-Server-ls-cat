## Client-Server : ls & cat
### Windows multithreading app

The application imitates the **ls** and **cat** commands from the Unix Shell terminal 

**server.exe** is run with 2 parameters , the first being the directory we are currently in (used as reference director in the **ls** command) and the second represents the number of threads the server should run on.

![server parameters](https://github.com/robyerts/Client-Server-ls-cat/blob/master/serverParameters.png)

The slashes in the path name can either be '\' or '/' or even a combination of those. The path can even be put between double quotes.

#### Both commands are represented as integers :

- **ls** - 1
- **cat** -2

The **client** will have to enter one of the above and then select the path of the directory/file.
For **ls**, the path is relative to the directory given as first argument to the server .

![ls sample](https://github.com/robyerts/Client-Server-ls-cat/blob/master/ls-run-sample.png)

On the other hand, **cat** takes the FULL PATH of a file as input , whether the file is inside the server directory is irrelevant.
![cat sample](https://github.com/robyerts/Client-Server-ls-cat/blob/master/slashes.png)



