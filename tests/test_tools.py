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


def get_file_property( which_property, file_name, output_of_ls ):
   for line in output_of_ls:
       if (line.split()[-1].strip() == file_name): # strip() removes the "\n"
          if(which_property == "permissions"):
            return line.split()[0];
          elif(which_property == "hard_link_number"):
            return line.split()[1];
          elif(which_property == "owner"):
            return line.split()[2];
          elif(which_property == "group"):
            return line.split()[3];
          elif(which_property == "size"):
            return line.split()[4];
          elif(which_property == "timestamp"):
            return (line.split()[5] + " " + line.split()[6] + " " + line.split()[7]);

