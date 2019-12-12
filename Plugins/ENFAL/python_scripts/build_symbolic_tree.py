import sys
import os
from tree import *

if __name__=="__main__":
    if len(sys.argv) != 2:
        print "USAGE:"
        print " python %s path/to/logfile" % sys.argv[0]
        sys.exit(1)

    logfile = sys.argv[1]
    t = build_tree(logfile)
    t.dump_graphviz("graph.dot")
    os.system("dot -Tpng graph.dot > graph.png")