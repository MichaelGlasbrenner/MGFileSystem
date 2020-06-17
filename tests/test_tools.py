#!/usr/bin/python

import sys
import os
import subprocess
import datetime

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
   

def directory_exists( file_name, dir_list ):
   for line in dir_list:
       if (line.split()[-1].strip() == file_name): # strip() removes the "\n"
          return True;
   return False; 


def remove_prefix(text, prefix):
    if text.startswith(prefix):
        return text[len(prefix):]
    return text;


def get_list_of_files(path):
   temp_file = open('temp_ls_output', "w")
   with temp_file as outfile:
      subprocess.call(["find", path, "-type", "f"], stdout=outfile)
   temp_file.close();
   
   read_file = open("temp_ls_output", "r");
   the_list = [];
   for line in read_file:
       the_list.append( remove_prefix(line.rstrip(), path + "/") );
   read_file.close();
    
   return the_list;


def get_list_of_directories(path):
   temp_file = open('temp_ls_output', "w")
   with temp_file as outfile:
      subprocess.call(["find", path, "-type", "d"], stdout=outfile)
   temp_file.close();
   
   read_file = open("temp_ls_output", "r");
   the_list = [];
   for line in read_file:
       if not (line.rstrip() == path or line.rstrip() == (path + "/")):
          the_list.append( remove_prefix(line.rstrip(), path + "/") );
   read_file.close();
    
   return the_list;


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


def check_creation_time(file_name, output_of_ls):
    time_stamp = get_file_property( "timestamp", file_name, output_of_ls);
    day = time_stamp.split()[1];
    hours = time_stamp.split()[2].split(":")[0];
    minutes = time_stamp.split()[2].split(":")[1];
    print(time_stamp);
    print(datetime.datetime.now());


def get_file_content( file_name ):

    the_content = [];
    the_file = open( file_name, "r");
    for line in the_file:
        the_content.append(line);

    return the_content;


def files_have_same_content( file1, file2 ):

    content1 = get_file_content(file1);
    content2 = get_file_content(file2);

    if not (len(content1)==len(content2)):
       return False;    

    content_is_identical = True;

    for i, line in enumerate(file1):
        if not (line == content2[i]):
           content_is_identical = False;

    return content_is_identical;


def run_bash_commands(list_of_commands):
    for command in list_of_commands:
        os.system( command );
