#!/usr/bin/python
from gi.repository import Gtk
import json
import sys

filename = "debug_file"
assemblypath = sys.argv[1]

class DebugWindow(Gtk.Window):
    #The class for the window itself

    currentTitle = ""

    def __init__(self):
        #Set up window size, title
        Gtk.Window.__init__(self, title="Debugger: " + assemblypath)
        self.set_size_request(800, 500)
        box = Gtk.Box(spacing=5)
        self.add(box)

        #The scrolling list of instructions
        instrStore = self.create_model_instrs()
        self.treeView = Gtk.TreeView(instrStore)
        self.treeView.set_rules_hint(True)
        self.create_columns_instrs(self.treeView)
        sw = Gtk.ScrolledWindow()
        sw.set_size_request(200, 0)
        box.pack_start(sw, False, True, 0)
        sw.add(self.treeView)
        self.treeView.get_selection().connect("changed", self.on_tree_selection_changed)

    def create_model_instrs(self):
        listData = []
        instrs = get_instrs()
        for i in instrs:
            listData.append(((i[0]), i[1]))
        store = Gtk.ListStore(int, str)
        for item in listData:
            store.append(item)
        return store

    def create_columns_instrs(self, treeView):
        rendererText = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("#", rendererText, text=0)
        column.set_sort_column_id(0)
        treeView.append_column(column)
        column = Gtk.TreeViewColumn("Instruction", rendererText, text=1)
        column.set_sort_column_id(1)
        treeView.append_column(column)
        treeView.columns_autosize()

    def on_tree_selection_changed(self, selection):
        model, treeIter = selection.get_selected()
        if treeIter != None:
            self.rebuild(model[treeIter][0])

    def rebuild(self, num):
        values = get_outp_line(num-1)
        print values[0]

def get_instrs():
    debugfile = open(filename)
    instrs = []
    for i, line in enumerate(debugfile):
        pc = json.loads(line)[0][15]
        print pc
        instrs.append((i+1, get_assem_line((pc-8)/4)))
    debugfile.close()
    return instrs

def get_outp_line(lineNo):
    outpfile = open(filename)
    for i, line in enumerate(outpfile):
        if (i == lineNo): return json.loads(line)
    outpfile.close()
    return None

def get_assem_line(lineNo):
    assemblyfile = open(assemblypath)
    for i, line in enumerate(assemblyfile):
        if (i == lineNo): return line
    assemblyfile.close()
    return None

win = DebugWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()