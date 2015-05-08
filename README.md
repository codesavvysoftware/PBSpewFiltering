# PBSpewFiltering
This is an application I wrote for the helping me trackdown issues with the WindowsCE Bluetooth stack.
We have to diagnose issues and come up with code fixes if possible.  One of the problems in analyzing 
issues is that the debug spews have a massive number of messages that don't pertain to Bluetooth and 
thus make diagnosing issues with Bluetooth more difficult.  I wrote this application to filter out messages
I am not interested in.  It has worked very well for me and has saved me a lot of time.

The reason I put it on GitHub has to do with sharing the work with another developer on our project.  As 
is often the case, he wasn't that interested in helping as he had higher priority activities.  I just left it 
on GitHub as I hope to have prospective employers view the source code and unit tests to evaluate my programming
style.  

The application is  a command line application.  I have tested it more on Windows than on Linux but it does execute
successfully on both operating systems.  I compiled the Windows code with the Visual Studio 2013 Community version.
I compiled the Linux version with GCC.  I used Eclipse on Linux to build the application by creating a project file
and selecting the c++11 compiling options.  As I went along I found out that arguably C++11 is better supported in 
GCC than in Visual Studio.  Therefore I had to make some compromises to get the same code to compile and build on 
both platforms.  

This application utilizes the boost::program_options library to parse the command line, boost::filesystems library for 
file related activities, and the boost test library to do unit testing.  I just finished adding the unit test driver and thus I have only a few unit tests.  That will change though, read below.  As an aside given the command line options and the number of file and folder name possibilities it could require a lot of tests.  This is an applicaiton for demonstration 
of my coding style though and I didn't really use a TDD approach to write it (I did consider how I would test it though
as I wrote it) so the unit tests are more about showing my understanding of the concept.  I really like the flexibility boost test provides for unit testing.

Future activites that I intend to do:
  1. Add unit tests incrementally.  I don't have very many but I have the infracture to the point where I can I can
    add to them easily.  I intend to try to add one per day for a while anyway.
    
  2.  Provide the means to write the results of unit tests to a file (besides just redirecting stderr and stdout from the command line which is in option). Finished, writes the results to an XML file that is supposed to be compatible with with xUnit a Jenkins plugin. 
  
  3.  "Pretty up" the unit test output.  Not Needed.
  
  4.  Add Visual Studio project file for making the application and the unit tests.
  
  5.  Add a makefile to for making the application and the unit tests.
  
  6.  Interface to Jenkins to do continuous building and testing.
  
  7.  Add a python script to run the program with various testing options  
  
  8.  TBD
  
  It should be noted that this application isn't concerned about file I/O race conditions because it is a situation
  that will happen rarely and when it does it will only affrect an in house application that makes a job easier
  to do.  
