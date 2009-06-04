Compilation of iptv
-------------------

To compile the iptv, you must follow this instructions:

1 - Windows
-----------

   1.1) First you must set the sbVB to the path:
	<sbVB_home directory>/AddmeToPath
   
   1.2) Then open the IPTV solution (IPTV.sln) located at iptv_root directory;
   1.3) Open the Batch build and mark all options, excepts:
	   iptv_appsharing WIN DLL debug
	   iptv_appsharing WIN DLL release
   1.4) The compilation must occur without errors.
   1.5) After the compilation end, if run the application, this don't find some dll's, then you must put these on            the path(rule 1.1). Now the application runs normally.

2 - Linux
---------
   2.1) You must access <iptv_root> directory and execute rebuild_all.sh

3 - Conclusion
--------------

   3.1) If the compilation return some error, you can find more informations at the file 'How to Build.txt' located 	at <iptv_root>/documentation.