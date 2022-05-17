# MultiThreading - ResourcesManager
### MOUCHEL Noé 

<br>

UML link : https://drive.google.com/file/d/1QsyD-BrqudgrOcB-TH17lsecZwWbKtMG/view?usp=sharing

Assets Folder : https://drive.google.com/drive/folders/12CcSTrHMDis5f-w_U4zQQoPStOVFRtht?usp=sharing

## Instruction


<br>

To Enable multiThread and re-enable it, go in "Include/Core/MultiThread.hpp", then comment / uncomment the macro MULTITHREAD_ENABLED.

![Alt text](Readme_Media/image_1.png?)

<br>

To see the scene loading directly, I advise you to go in edit mode and try loading *Sponza* (This scene is an obj alone so the scene is initialized before the load end while for the other scene that have lot of gameobjects the scene is initialized short before the end of the load)

![Alt text](Readme_Media/image_7.png?)

<br>

When in EDITOR mode, 'WASD' to move and 'Q' and 'E' to decrease / increase camera speed.

<br>

## How does the ThreadPool work ?

<br>

The threadpool contains a queue which stock functions waiting to be executed, and a list of thread.

![Alt text](Readme_Media/image_2.png?)
   
<br>

The thread stocked in the list are looping and checking the queue to execute the function stocked one per one without causing data races.  

![Alt text](Readme_Media/image_3.png?)
   
<br>

Each thread can execute one function from the queue at a time.

The queue is safely filled by the function **addInQueue**.

![Alt text](Readme_Media/image_4.png?)
   
<br>

An atomic boolean value called "done" control the end of the multithread and when it is set to true, all the thread will be joined in the function **stop** so we can close the program safely.

![Alt text](Readme_Media/image_5.png?)

<br>


Here a comparaison between multithread and monothread :

![Alt text](Readme_Media/image_6.png?)

<br>


And lastly a GIF of sthe sponza loading directly using multithreads :

![Alt text](Readme_Media/gif_1.gif?)
