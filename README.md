# PBSpewFiltering
This is an application I wrote for the helping me trackdown issues with the WindowsCE Bluetooth stack.
We have to diagnose issues and come up with code fixes if possible.  One of the problems in analyzing 
issues is that the debug spews have a massive number of messages that don't pertain to Bluetooth and 
thus make diagnosing issues with Bluetooth more difficult.  I wrote this application to filter out messages
I am not interested in.  It has worked very well for me and has saved me a lot of time.

The reason I put it on GitHub has to do with sharing the work with another developer on our project.  As 
is often the case he wasn't that interested in helping as he higher priority activities.  I just left it 
on GitHub as I hope to have prospective employers few the source code and unit tests to evaluate my programming
style.  

The application is command line application.  I have tested it more on Windows than on Linux but it does execute
successfully on both operating systems.  I compiled the Windows code with the Visual Studio 2013 Community version.
I compiled the Linux version with GCC.  I used Eclipse on Linux to build the application by creating a project file
and selecting the c++11 compiling options.  As I went along I found out that arguably C++11 is better supported in 
GCC than in Visual Studio.  Therefore I had to make some compromises to get the same code to compile and build on 
both platforms.  

This application is utilizes the boost::program_options library to parse the command line, boost::filesystems for 
file related activities, and boost test to do unit testing.  

Future activites that I intend to do:
  1. Add unit tests incrementally.  I don't have very many but I have the infracture to the point where I can I can
    add to them easily.  I intend to try to add one per day for a while anyway.
    
  2.  Provide the means to rite the results of unit tests to a file.  
  
  3.  "Pretty up" the unit test output.
  
  4.  Add Visual Studio project file for making the application and the unit tests.
  
  5.  Add a makefile to for making the application and the unit tests.
  
  6.  Interface to Jenkins to do continuous building and testing.
  
  7.  TBD
  
  It should be noted that this application isn't concerned about file race conditions because it is situation
  that will happen rarely and when it does it will only affrect an in house application that makes a job easier
  to do.  
