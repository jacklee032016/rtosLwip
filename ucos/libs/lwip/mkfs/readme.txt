
Update on May.17, 2018. Jack Lee
		Update log and some info in static web pages;
		build: make under mingw (call windows API), creating mkfsdata.exe;
		run: mkfsdata: read and create file for 'fs' directory;
		Result file: extHttpFsData.c, copy to directory and replace file in extHttp;
		

This directory contains a script ('makefsdata') to create C code suitable for
httpd for given html pages (or other files) in a directory.

There is also a plain C console application doing the same and extended a bit.

Usage: htmlgen [targetdir] [-s] [-i]s
   targetdir: relative or absolute path to files to convert
   switch -s: toggle processing of subdirectories (default is on)
   switch -e: exclude HTTP header from file (header is created at runtime, default is on)
   switch -11: include HTTP 1.1 header (1.0 is default)

  if targetdir not specified, makefsdata will attempt to
  process files in subdirectory 'fs'.
