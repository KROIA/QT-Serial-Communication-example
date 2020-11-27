# Installation
***
## QT-Software
1. Download the repository.
2. Extract the files somewhere but let the file structure the same.
3. Open QT-Creator and open the project by browsering to the extractet .pro file.
4. I usually compile with **MinGW** so check if you have that or try other compilers.
5. Compile the code.
6. The example uses only qDebug() outputs for displaying any data  
so let the console in QT-Creator open while the program is running.
***
## Device-Software
* I used a Arduino to communicate to the GUI.  
The code for that is in the **Device-code** folder
***

## Building
# Standalone? 
* **YES** For more Information click [here](https://wiki.qt.io/Build_Standalone_Qt_Application_for_Windows).
* **NO** When you don't need a standalone version putt all needed **.dll** files to the **.exe**.  
In the **Compiled** folder, you can find all needed **.dll** files for the program. You can find these in your QT install folder.  
C:\Qt\5.10.1\mingw53_32\bin  
It depends on your installation and compiler.  
The folder **platforms** is allso needed.  
The **.dll** in there you can find  
E:\Qt\5.10.1\mingw53_32\plugins\platforms  

