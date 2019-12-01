#!/usr/bin/python

import sys
import subprocess

def ls_output(directory):
   temp_file = open('temp_ls_output', "w")
   with temp_file as outfile:
      subprocess.call(["ls", "-ltr", directory], stdout=outfile)
   temp_file.close();

   lines = [];
   read_file = open("temp_ls_output", "r");
   for line in read_file:
       lines.append( line );
   read_file.close();
   return lines;
   

def file_exists( file_name, output_of_ls ):
   for line in output_of_ls:
       if (line.split()[-1].strip() == file_name): # strip() removes the "\n"
          return True;
   return False; 


