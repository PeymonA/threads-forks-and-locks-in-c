# threads-forks-and-locks-in-c
A program that demonstrates how you can safely use the same resource in different system-wide processes 

A1part3.c uses the pthread_atfork function to set up functions which acquire the lock , before 
the fork and release it after the fork (in both the parent and child processes). 
