README IP.TV Client 0.2.3 BETA - Linux
======================================

RUNNING
-------
Just change into the application's directory and run the binary "iptv_client", or do it using your favorite file browser.

REQUIREMENTS
------------
- A relatively recent Linux distribution, with a 2.6 series kernel.
- An X server (XFree86/X.org).
- An ALSA-supported sound card.
- A video card capable of hardware-accelerated OpenGL with a minimum of 16 MB video RAM. Recommended GeForce-class cards.

Additionally, you will need the following library packages and their dependencies to run the program:

- wxWidgets 2.8/GTK+ (libwxgtk2.8-0)
- libx264 (libx264-54)
- the Speex audio codec (libspeex1)
- Mesa OpenGL (libgl1-mesa-glx, libglu1-mesa)

Additionally, it is necessary to install the library ChartDirector, which is not included in Ubuntu official repositories.

Chart Director library installation:

1. Download the library in the following link: http://download2.advsofteng.com/chartdir_cpp_linux.tar.gz;
2. Extract the file to a temporary directory;
3. Copy all the contents of the directory lib to the directory /usr/local/lib. (sudo cp -a * /usr/local/lib);
4. Run the following command: sudo ldconfig.

Notes
-----
The packages dependencies should be solved by the package manager.

Package names are for Debian/Ubuntu systems. They should be installed with "sudo apt-get install <package>".

If you use another distro (Slackware, Fedora etc.), adapt the package names to its package system.
