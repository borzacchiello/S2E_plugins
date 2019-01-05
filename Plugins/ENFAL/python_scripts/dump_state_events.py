import sys
from tree import *

def get_events(path, logfile):
    with open(logfile, "r") as log:
        line = log.readline()
        while line:
            line = line.strip()
            if "[ENFAL-hook]" in line:
                state_number = int(line[line.find("[State ")+7: line.find("]")])
                if state_number in path:
                    print line[line.find("[ENFAL-hook]")+len("[ENFAL-hook]"):]
                line = log.readline().strip()
                if "SymbExpression" in line and state_number in path:
                    line = line[line.find(" - ")+3:]
                    while "END SYMBOL" not in line:
                        print line
                        line = log.readline().strip()
                    print
                line = log.readline()
            elif "LibraryCallMonitor: " in line:
                state_number = int(line[line.find("[State ")+7: line.find("]")])
                if state_number in path:
                    print line[line.find("LibraryCallMonitor: ")+len("LibraryCallMonitor: "):]
                line = log.readline()
            elif "ENFAL:" in line:
                state_number = int(line[line.find("[State ")+7: line.find("]")])
                if state_number in path:
                    print line[line.find("ENFAL:")+len("ENFAL:"):]
                line = log.readline()
            elif "TestCaseGenerator:" in line:
                state_number = int(line[line.find("[State ")+7: line.find("]")])
                if state_number == path[-1]:
                    line = log.readline().strip()
                    print "\n *** INPUT *** "
                    print line[line.find("TestCaseGenerator:") + len("TestCaseGenerator:"):]
                    new_line = log.readline().strip()
                    while "[State" not in new_line:
                        print new_line
                        new_line = log.readline().strip()
                    line = new_line
                else:
                    line = log.readline()
            else:
                line = log.readline()

if __name__=="__main__":
    if len(sys.argv) != 3:
        print "USAGE:"
        print " python %s path/to/logfile state_number" % sys.argv[0]
        sys.exit(1)

    logfile      = sys.argv[1]
    state_number = int(sys.argv[2])

    t    = build_tree(logfile)
    path = t.get_path(state_number)
    get_events(path, logfile)