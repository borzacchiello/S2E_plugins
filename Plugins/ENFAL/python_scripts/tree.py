skeleton_graphviz = """digraph {

&&&&
}"""

class Node(object):
    def __init__(self, data):
        self.data     = data
        self.children = []
    
    def __str__(self):
        return "Node("+str(self.data)+")"
    
    def __repr__(self):
        return self.__str__()
    
class Tree(object):
    def __init__(self):
        self.root = None

    def __contains__(self, el):
        fringe = [self.root] if self.root is not None else []
        while fringe:
            node = fringe.pop()
            if el == node.data:
                return True
            fringe = node.children + fringe
        return False
    
    def add_root(self, el):
        self.root = Node(el)

    def add_as_child_of(self, father, child):
        assert father is not None        
        fringe = [self.root] if self.root is not None else []
        while fringe:
            node = fringe.pop()
            if father == node.data:
                node.children.append(Node(child))
                return True
            fringe = node.children + fringe
        return False
    
    def get_path(self, el):
        if not self.root:
            return
        fringe = [self.root] 
        paths  = {self.root.data: [self.root.data]} 
        while fringe:
            node = fringe.pop()
            if el == node.data:
                return paths[el]
            for c in node.children:
                paths[c.data] = paths[node.data] + [c.data]
            fringe = node.children + fringe
        return []
    
    def dump_graphviz(self, outfile):
        with open(outfile, "w") as out:
            lines = []
            fringe = [self.root] if self.root is not None else []
            while fringe:
                node = fringe.pop()
                lines.append("%s -> %s\n" % (str(node.data), str([el.data for el in node.children]).replace("[", "{").replace("]", "}")))
                fringe = node.children + fringe
            if lines:
                out.write(skeleton_graphviz.replace("&&&&", reduce(lambda x, y: x+y, lines)))


def build_tree(logfile):
    t = Tree()
    with open(logfile, 'r') as log:
        line = log.readline()
        while line:
            line = line.strip()
            if "Forking state" in line:
                src = int(line[line.find("[")+6:line.find("]")])
                next_line = log.readline().strip()
                dst1 = int(next_line[next_line.find("state ")+6:])
                next_line = log.readline().strip()
                dst2 = int(next_line[next_line.find("state ")+6:])

                if t.root is None:
                    t.add_root(src)
                
                if dst1 not in t:
                    t.add_as_child_of(src, dst1)
                if dst2 not in t:
                    t.add_as_child_of(src, dst2)
            line = log.readline()
    return t