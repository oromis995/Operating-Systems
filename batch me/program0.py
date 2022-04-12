# SOURCES
# https://computinglearner.com/how-to-create-a-menu-for-a-python-console-application/

from logging import root
import os
from tkinter import Tk
from tkinter.filedialog import askdirectory
 
#os.system("MY COMMAND HERE")
menu_options = {
    1: 'i. List jobs.',
    2: 'ii. Set jobs directory.',
    3: 'iii. Compile and run a specific program.',
    4: 'iv. Compile and run all jobs in a specific directory.',
    5: 'v. Shutdown.',
    6: 'vi. List program options.',
    7: 'vii. help.'
}


def printJobs(jobs):
    if isinstance(jobs, str):
        print(jobs + "\n")
    else:
        for job in jobs:
            print (str(jobs.index(job)+1) + " -- " + job + "\n")

def findCPPJobs(rootDir):
    list = os.listdir(rootDir)
    iterator = 0;
    while iterator < len(list):
        file = list[iterator]
        fileExt = os.path.splitext(file)[1]
        #print("file: " + file + " extension: " + fileExt)
        if fileExt != ".cpp":
            list.remove(file)
            iterator = iterator - 1
        else:
            # Removes extension from job files
            list[iterator] = os.path.splitext(file)[0]
        iterator = iterator + 1
    if len(list) == 0:
        return "No jobs Available"
    else:
        return list

def printMenu():
    for key in menu_options.keys():
        print (key, '--', menu_options[key] )

def option1(rootDir):
    jobs = findCPPJobs(rootDir)
    printJobs(jobs)
    if jobs == "No jobs Available":
        return []
    return jobs

def option2():
    Tk().withdraw()
    path = askdirectory(title = 'Select Folder') 
    Tk().update()
    print("Selected: " + path) 
    os.chdir(path)
    return path

def option3(rootDir,jobs):
    while(True):
        jobs = option1(rootDir)
        option = ''
        if len(jobs) == 0:  
            # No Jobs Available 
            break
        try:
            option = int(input('Which job would you like to run?'))
        except:
            print('Wrong input. Please enter a number ...')            
        
        if option < len(jobs):
            os.system("g++ " + "'" + rootDir + "/" + jobs[option-1] + ".cpp' -o " + jobs[option-1] )
            os.system("./" + jobs[option-1])
            break
        else:
            print("Invalid option. Please enter a number between 1 and " 
            + str(len(jobs)) + ".")

def option4():
    tempDir = option2()
    jobs = option1(tempDir)
    if not isinstance(jobs, str):
        for job in jobs:
            os.system("g++ " + "'" + tempDir + "/" + job + ".cpp' -o " + job )
            os.system("./" + job)


if __name__ == '__main__':

    rootDir = "/home/lubuntu/Desktop/CMPS ASSIGNMENT/"
    jobs = []

    while(True):
        printMenu()
        option = ''

        try:
            option = int(input('Make a Selection: '))
        except:
            print('Wrong input. Please enter a number ...')

        if option == 1:
            jobs = option1(rootDir)

        elif option == 2:
            rootDir = option2()

        elif option == 3:
            option3(rootDir,jobs)

        elif option == 4:
            option4()

        elif option == 5:
            print('Goodbye!')
            exit()

        elif option == 6:
            printMenu()

        elif option == 7:
            print("Only God can help you now \n")

        else:
            print('Invalid option. Please enter a number between 1 and 7.')

            

