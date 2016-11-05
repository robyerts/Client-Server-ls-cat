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

On the other hand, **cat** takes the FULL PATH of a file as input ; whether the file is inside the server directory is irrelevant.


![cat sample](https://github.com/robyerts/Client-Server-ls-cat/blob/master/slashes.png)
If everything runs smooth, the **server** will display only the initialization and termination of connections between threads and clients.


![client closing](https://github.com/robyerts/Client-Server-ls-cat/blob/master/connections-closed.png)

In order to close the **client.exe** you simply enter a command that's different from 1 or 2


![client closing](https://github.com/robyerts/Client-Server-ls-cat/blob/master/client-closing.png)

In case an **error** is encountered, the server/client will print the error code and the function that generated the error; afterwards it enters sleep for 10 sec and closes itself.

NOTE: if the server encounters the error in one thread, **only that ONE THREAD will become UNUASABLE**, while the rest will still be working properly.

In case of reaching the maximum limit a server can handle

