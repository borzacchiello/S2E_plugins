import sys
import os

if __name__=="__main__":
    if len(sys.argv) != 2:
        print "USAGE:"
        print " python %s path/to/logfile" % sys.argv[0]
        sys.exit(1)

    logfile = sys.argv[1]
    with open(logfile, "r") as log:
        for line in log:
            if "COMMAND\n" in line and "[State " in line:
                print int(line[line.find("[State ")+len("[State "): line.find("]")]), 
        print
