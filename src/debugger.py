#!/usr/bin/python
from gi.repository import Gtk
import json
import sys

filename = "debug_file"
assemblypath = sys.argv[1]

class DebugWindow(Gtk.Window):
    #The class for the window itself

    currentTitle = ""
    currentRegs = []
    currentMem = []
    assempath = ""
    size = 0

    def __init__(self):
        #Set up window size, title
        Gtk.Window.__init__(self, title="Debugger: " + assemblypath)
        self.set_size_request(800, 500)
        box = Gtk.Box(spacing=5)
        self.add(box)
        self.assempath = fix_assem_file(assemblypath)

        #The scrolling list of instructions
        instrStore = self.create_model_instrs()
        self.treeView = Gtk.TreeView(instrStore)
        self.treeView.set_rules_hint(True)
        self.create_columns_instrs(self.treeView)
        sw = Gtk.ScrolledWindow()
        sw.set_size_request(200, 0)
        box.pack_start(sw, False, False, 0)
        sw.add(self.treeView)
        self.treeView.get_selection().connect("changed", self.on_tree_selection_changed)

        #The title and UI box, and toolbar
        infobox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL)
        box.pack_start(infobox, True, True, 5)
        self.title = Gtk.Label()
        self.title.set_alignment(0, 0)
        infobox.pack_start(self.title, False, False, 5)
        innerscroll = Gtk.ScrolledWindow()
        innerbox = Gtk.Box(spacing=5, orientation=Gtk.Orientation.HORIZONTAL)
        toolbar = Gtk.Toolbar()
        self.progress = Gtk.ProgressBar()
        innerscroll.add_with_viewport(innerbox)
        infobox.pack_start(innerscroll, True, True, 5)
        infobox.pack_end(self.progress, False, False, 5)

        #The registers
        regStore = self.create_model_regs()
        regTitle = Gtk.Label("<b>Registers</b>")
        regTitle.set_use_markup(True)
        regBox = Gtk.Box(spacing=5, orientation=Gtk.Orientation.VERTICAL)
        self.regTree = Gtk.TreeView(regStore)
        self.regTree.set_rules_hint(True)
        self.create_columns_regs(self.regTree)
        innerbox.pack_start(regBox, True, True, 5)
        regBox.pack_start(regTitle, False, False, 2)
        regBox.pack_start(self.regTree, True, True, 2)

        #The memory
        memStore = self.create_model_mem()
        memTitle = Gtk.Label("<b>Non-Zero Memory</b>")
        memTitle.set_use_markup(True)
        memBox = Gtk.Box(spacing=5, orientation=Gtk.Orientation.VERTICAL)
        self.memTree = Gtk.TreeView(memStore)
        self.memTree.set_rules_hint(True)
        self.create_columns_mem(self.memTree)
        innerbox.pack_start(memBox, True, True, 5)
        memBox.pack_start(memTitle, False, False, 2)
        memBox.pack_start(self.memTree, True, True, 2)

    def create_model_instrs(self):
        listData = []
        instrs = get_instrs(self.assempath)
        self.size = len(instrs)
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

    def create_model_regs(self):
        listData = []
        for position, item in enumerate(self.currentRegs):
            listData.append(((position, item, self.tohex(item, 32))))
        store = Gtk.ListStore(int, int, str)
        for item in listData:
            store.append(item)
        return store

    def create_columns_regs(self, treeView):
        rendererText = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("#", rendererText, text=0)
        column.set_sort_column_id(0)
        treeView.append_column(column)
        column = Gtk.TreeViewColumn("Value", rendererText, text=1)
        column.set_sort_column_id(1)
        treeView.append_column(column)
        column = Gtk.TreeViewColumn("Hex Value", rendererText, text=2)
        column.set_sort_column_id(2)
        treeView.append_column(column)
        treeView.columns_autosize()

    def create_model_mem(self):
        listData = []
        for position, item in enumerate(self.currentMem):
            listData.append(((hex(item[0]), item[1], self.tohex(item[1], 32))))
        store = Gtk.ListStore(str, int, str)
        for item in listData:
            store.append(item)
        return store

    def create_columns_mem(self, treeView):
        rendererText = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Address", rendererText, text=0)
        column.set_sort_column_id(0)
        treeView.append_column(column)
        column = Gtk.TreeViewColumn("Value", rendererText, text=1)
        column.set_sort_column_id(1)
        treeView.append_column(column)
        column = Gtk.TreeViewColumn("Hex Value", rendererText, text=2)
        column.set_sort_column_id(2)
        treeView.append_column(column)
        treeView.columns_autosize()

    def on_tree_selection_changed(self, selection):
        model, treeIter = selection.get_selected()
        if treeIter != None:
            self.currentTitle = "<span size='25000'><b>" + model[treeIter][1] + "</b></span>"
            self.rebuild(model[treeIter][0])

    def rebuild(self, num):
        self.title.set_text(self.currentTitle)
        self.title.set_use_markup(True)
        values = get_outp_line(num-1)
        self.currentRegs = values[0]
        self.currentMem = values[1]
        newRegMod = self.create_model_regs()
        self.regTree.set_model(newRegMod)
        newMemMod = self.create_model_mem()
        self.memTree.set_model(newMemMod)
        self.progress.set_fraction(float(num)/float(self.size))
        self.show_all()

    def tohex(self, val, nbits):
        return hex((val + (1 << nbits)) % (1 << nbits))

class SuggestionWindow(Gtk.Window):

    assempath = ""

    def __init__(self):
        #Set up window size, title
        Gtk.Window.__init__(self, title="Assembly failed: " + assemblypath)
        self.set_size_request(800, 500)
        box = Gtk.Box(spacing=5)
        self.add(box)
        self.assempath = fix_assem_file(assemblypath)

        #The scrolling list of instructions
        instrStore = self.create_model_instrs()
        self.treeView = Gtk.TreeView(instrStore)
        self.treeView.set_rules_hint(True)
        self.create_columns_instrs(self.treeView)
        self.treeView.get_selection().connect("changed", self.on_tree_selection_changed)
        sw = Gtk.ScrolledWindow()
        sw.set_size_request(200, 0)
        box.pack_start(sw, False, False, 0)
        sw.add(self.treeView)

        #The title and UI box
        infobox = Gtk.Box(spacing=20, orientation=Gtk.Orientation.VERTICAL)
        box.pack_start(infobox, True, True, 5)
        self.title = Gtk.Label("<span size='25000'><b>Error in assembly. Some suggestions...</b></span>")
        self.title.set_alignment(0, 0)
        self.title.set_use_markup(True)
        self.label1 = Gtk.Label()
        self.label2 = Gtk.Label()
        infobox.pack_start(self.title, False, False, 5)
        infobox.pack_start(self.label1, True, True, 5)
        infobox.pack_start(self.label2, True, True, 5)

    def create_model_instrs(self):
        listData = []
        suggs = get_suggestions()
        for pos, item in enumerate(suggs):
            listData.append(((pos, get_assem_line(item[0]-1, self.assempath))))
        store = Gtk.ListStore(int, str)
        for item in listData:
            store.append(item)
        return store

    def create_columns_instrs(self, treeView):
        rendererText = Gtk.CellRendererText()
        column = Gtk.TreeViewColumn("Instruction", rendererText, text=1)
        column.set_sort_column_id(0)
        treeView.append_column(column)
        treeView.columns_autosize()

    def on_tree_selection_changed(self, selection):
        model, treeIter = selection.get_selected()
        self.rebuild(model[treeIter][0])

    def rebuild(self, num):
        sugLine = get_suggestions()[num]
        self.label1.set_text("<span size='15000'>You used an incorrect command, <b>" + sugLine[1] + "</b>.</span>")
        self.label1.set_use_markup(True)
        self.label2.set_text("<span size='15000'>Consider using: <b>" + sugLine[2] + "</b></span>")
        self.label2.set_use_markup(True)
        self.show_all()

def get_instrs(path):
    debugfile = open(filename)
    instrs = []
    for i, line in enumerate(debugfile):
        pc = json.loads(line)[0][15]
        instrs.append((i+1, get_assem_line((pc-8)/4, path)))
    debugfile.close()
    return instrs

def get_outp_line(lineNo):
    outpfile = open(filename)
    toreturn = None
    for i, line in enumerate(outpfile):
        if (i == lineNo): 
            toreturn = json.loads(line)
            break
    outpfile.close()
    return toreturn

def get_assem_line(lineNo, path):
    assemblyfile = open(path)
    toreturn = None
    for i, line in enumerate(assemblyfile):
        if (i == lineNo):
            toreturn = line
            break
    assemblyfile.close()
    return toreturn

def get_suggestions():
    suggfile = open('debug_suggestions')
    results = []
    for i, line in enumerate(suggfile):
        results.append(json.loads(line))
    return results

def fix_assem_file(filepath):
    with open(filepath) as assemblyfile:
        data = assemblyfile.readlines()
        assemblyfile.close()
    with open('fixedassemble', 'w') as newassembly:
        for item in data:
            if ':' in item:
                data.remove(item)
        newassembly.writelines(data)
        newassembly.close()
        return "fixedassemble"

if get_outp_line(0) == None:
    win = SuggestionWindow()
else: win = DebugWindow()
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()