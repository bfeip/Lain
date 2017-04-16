import sys
import os
import subprocess

def main():
    filenames = [] # files to be compiled
    for arg in sys.argv[1:]:
        if os.access(arg, os.R_OK):
            # arg is a file that will be compiled
            filenames.append('../' + arg)

    os.chdir('./build')
    retCode = subprocess.call(['./lain'] + filenames)
    if retCode is not 0:
        exit(retCode)

    retCode = subprocess.call(['./llvm/bin/llc', '-filetype=obj', './a.ll'])
    if retCode is not 0:
        print 'Error encountered while converting llvm ir'
        exit(retCode)

    retCode = subprocess.call(['gcc', './a.o', '-o', './a.out'])
    if retCode is not 0:
        print 'Error encountered while converting .o to .out'
        exit(retCode)

    exit(0)

if __name__ == '__main__':
    main()
