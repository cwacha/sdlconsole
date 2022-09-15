# Version 2.1 (2003-08-28)
SDL_console 2.1 released. I have made lots of bugfixes in the command line code
and killed some segfaults. But most important: The console now uses a configure
script for improved compatibility. Also check out the new screenshots.

# Version 2.0 (2003-03-14)
New release SDL_console 2.0! New Homepage! New Documentation using DOXYGEN! New
everything. Now you know what I do in my vacation :-)
Clemens Wacha

# Version 1.3 (2002-09-19)
SDL_console 1.3 is released! Wacha Clemens again couldn't stop adding stuff to
the code. I has too many features and bugfixes to mention them here. For a
complete list see below. He has completely rewritten the command line and added
lots of useful control keys. 

# Version 1.2 (2002-09-13)
SDL_console 1.2 is released thanks to Wacha Clemens who added a prompt,
autoscrolling commandline, and buxfixes. I also added CON_Free() which equals
to CON_Destroy but without destroying text support of DT (thanks to Paul
Wighton), CON_Destroy still in the lib for compatibility. 

# Version 1.1 (2002-09-03)
Garrett gave me the SDL_console project and I hope I could add nice changes to
the lib !  But's it's already stable and I think that only a few things would
be added. For now I added a debian package of SDL_Console, the SDL_image
support for PNG, JPG, ... images formats, and a shared library of the lib.
SDL_console is now version 1.1. 
Boris Lesner. 

# 2002-07-12
SDL_Console needs a new author! I no longer have the time to work on
SDL_Console with my new job and other interests and due to the popularity of
SDL_Console, I do not want to let the project stagnate. I'm going to give the
project away to someone else to maintain and improve as they see fit. The new
author will have full control of the project and may do with it as they please.
If you are interested in taking over the project, please email me. Give me some
information about your computer science skills, projects you've worked on, etc.
After/if I get a sufficient response from interested people, I will decide who
will take over the project. Thanks for all the support from everyone. I hope
this project is still useful in the future. 

# 2001-11-14
Cort Stratton added a new MSVC project for the console with the new source. 

# Version 1.0 (2001-10-08)
Finally an update to SDL_console! SDL_console has now reached version 1.0. I
figured it's about time since there have not been any bug fixes for a while so
the code is pretty solid. There are also lots of new additions to the code and
a decent Makefile system now. If anyone wants to write an autoconf system
though, be my guest.
 - Backward compatibilty has been completely broken. See the API below.
 - Multiple independant consoles can be displayed on the screen. Checkout the
   screenshot.
 - Better Makefile system. Should be a little easier to work with now.
 - Fixed a few performance issues.
 - Cleaned up the code. Cutting and pasting everyones different coding styles
   into the source was getting messy.


# 2001-06-12 
Seung Chan Lim Has donated a version of SDL_Console with the demo in MSVC
project format. You can download it below. 

# 2001-05-15
Now the console really works with OpenGL. =) Thanks to Cort Stratton for fixing
the OpenGL support. The console example source now demonstrates it's use with
OpenGL. There is also the non-OpenGL sample source included in the archive.
As usual there were a few cosmetic modifications and bug fixes also. 

# 2001-04-29
I added a blinking cursor showing the current editing position due to popular
demand. Ugh I didn't realize how ugly this code was. =) It's efficient, just
not very neat.
 - Added a blinking cursor showing the current text position.
 - Reorganized some of the code to be more readable.

# 2001-02-20
Thanks to Patricia Cruz for adding an OpenGL fix to the Console source.
 - Console now works within an OpenGL program and can be blitted to OpenGL
   surfaces.

# 2000-11-18
Lots of new changes here, mainly thanks to Lee Thomason for submitting some of
them and motivating me to work on this again.

 - Naming conventions have been changed. Console commands now start with the
   CON_ prefix so as not to be confused with SDL_. Also the text drawing
   routines now start with DT_ so that they are distinguished as being
   seperate from the console.
 - Console is no longer `attached' to the top of the screen, it is now a box
   that can be located anywhere. CON_Position(int x, int y)
 - The console can be resized without reinitializing. CON_Resize(SDL_Rect rect)
 - Tab completion lists all the matching commands rather than just completing
   with the first command it partially matches with. (Ya I know, this ones been
   overdue.)
 - An option can be set to send back the command with the parameters from the
   user rather than just the parameters. So now multiple commands can be sent
   to one function and be distinguished from each other.
   CON_SendFullCommand(int sendOn)
 - New command CON_Destroy() now shuts down and frees the console from the
   program. DT_DestroyDrawText() does the same thing for the text drawing
   routines.
 - CON_SetConsoleBorder() has been removed. This is because of the new free
   moving console. Setting a border around the edges is much more complicated
   so I won't bother implementing this unless people ask for it.
 - All of the help resources are going onto the webpage now rather than in the
   README file.

# 1999-04-16
 - Scroll back buffer amount can be specified at init.
 - PageUP PageDOWN scrolling through the buffer.
 - Printing to the console is done with printf() similiar arguments.
 - Tab completion of commands.
 - Arbitrary number of commands can be added.
 - Arbitrary number of arguments to each command from the input.
 - Alpha blending onto the destination surface.(optional for speed)
 - Border/Background image.(optional for speed)
 - Any bitmap font can be used.


