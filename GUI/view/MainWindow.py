from tkinter import *
from tkinter.messagebox import *
from tkinter.ttk import Treeview
import sys
from GUI.view import AboutWindow
from GUI.view import HelpMethod


class MainWindow(Tk):
    def close(*args):
        result = askyesno('Close program', 'Do you want to close?')
        if (result == True):
            sys.exit(0)

    def save(*args):
        # todo savemethod
        showerror('Not available', 'Not available')

    def saveAs(*args):
        # todo save as method
        showerror('Not available', 'Not available')

    def open(*args):
        # todo open method
        showerror('Not available', 'Not available')

    def createCalendar(*args):
        # todo open method
        showerror('Not available', 'Not available')

    def createEvent(*args):
        # todo open method
        showerror('Not available', 'Not available')

    def about(*args):
        AboutWindow.dialog()

    def __init__(self):
        Tk.__init__(self)
        HelpMethod.centerOfWindow(self, 580, 485)
        self.resizable(width=False, height=False)
        menuBar = Menu(self)
        menuFile = Menu(menuBar, tearoff=False)
        menuCreate = Menu(menuBar, tearoff=False)
        menuHelp = Menu(menuBar, tearoff=False)
        self.makeFileViewPanel()
        self.makeLogPanel()
        self.makeMenuFile(menuFile, menuBar)
        self.makeMenuCreate(menuCreate, menuBar)
        self.makeMenuHelp(menuHelp, menuBar)
        self.config(menu=menuBar)
        self.bind_all('<Control-x>', self.close)
        self.bind_all('<Control-s>', self.save)
        self.bind_all('<Control-o>', self.open)
        self.makeRightClickMenu()

    def makeMenuFile(self, menuFile, menuBar):
        menuFile.add_command(label='Open...', command=self.open, accelerator='Ctrl+O', underline=0)
        menuFile.add_command(label='Save', command=self.save, accelerator='Ctrl+S', underline=0)
        menuFile.add_command(label='Save as...', command=self.saveAs, underline=1)
        menuFile.add_separator()
        menuFile.add_command(label='Exit', command=self.close, accelerator='Ctrl+X', underline=0)
        menuBar.add_cascade(label='File', menu=menuFile, underline=0)

    def makeMenuCreate(self, menuCreate, menuBar):
        menuCreate.add_command(label='Calendar', command=self.createCalendar, underline=0)
        menuCreate.add_command(label='Event', command=self.createEvent, underline=0)
        menuBar.add_cascade(label='Create', menu=menuCreate, underline=0)

    def makeMenuHelp(self, menuHelp, menuBar):
        menuHelp.add_command(label='About iCalGUI...', command=self.about, underline=0)
        menuBar.add_cascade(label='Help', menu=menuHelp, underline=0)

    def makeFileViewPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=0, width=580, height=280)
        scrollBar = Scrollbar(panel)
        scrollBar.pack(side=RIGHT, fill=Y)
        global _fileViewTree
        _fileViewTree = Treeview(panel, columns=('c1', 'c2', 'c3', 'c4'), show='headings', yscrollcommand=scrollBar.set)
        _fileViewTree.column('c1', width=70, anchor='center')
        _fileViewTree.column('c2', width=70, anchor='center')
        _fileViewTree.column('c3', width=70, anchor='center')
        _fileViewTree.column('c4', width=370, anchor='center')
        _fileViewTree.heading('c1', text='Event No')
        _fileViewTree.heading('c2', text='Props')
        _fileViewTree.heading('c3', text='Alarms')
        _fileViewTree.heading('c4', text='Summary')
        _fileViewTree.pack(side=LEFT, fill=Y)
        scrollBar.config(command=_fileViewTree.yview)

        def listClick(event):
            selection = _fileViewTree.selection()
            if len(selection) != 0:
                item = _fileViewTree.focus()
                if item in selection:
                    _fileViewTree.selection_remove(item)
                    return 'continue'

        _fileViewTree.bind('<Button-1>', listClick)

        # test value
        for i in range(50):
            _fileViewTree.insert('', i, values=[str(i)] * 6)
        # test value

    def makeLogPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=285, width=580, height=195)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=580, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=580, height=165)
        label = Label(panelInsideUp, text='Log Info:', width=10, anchor='w', font=('Helvetica', 12)).grid(row=0,
                                                                                                         column=0)
        button = Button(panelInsideUp, text='Clear', width=5).grid(row=0, column=1)
        scrollBar = Scrollbar(panelInsideDown)
        scrollBar.pack(side=RIGHT, fill=Y)
        global _globelLogInfoText
        _globelLogInfoText = Text(panelInsideDown, relief=SUNKEN, state=DISABLED)
        scrollBar.config(command=_globelLogInfoText.yview)
        _globelLogInfoText.config(yscrollcommand=scrollBar.set)
        scrollBar.pack(side=RIGHT, fill=Y)
        _globelLogInfoText.pack(side=LEFT, expand=YES, fill=BOTH)

    def logInfoText(self, text):
        _globelLogInfoText.config(state=NORMAL)
        _globelLogInfoText.delete('1.0', END)
        _globelLogInfoText.insert('1.0', text)
        _globelLogInfoText.config(state=DISABLED)

    def makeRightClickMenu(self):
        menu = Menu(self, tearoff=0)
        menu.add_command(label='Show Alarms')
        menu.add_command(label='Extract optional props')

        def displayMenu(event):
            if len(_fileViewTree.selection()) == 0:
                return 'Continue'
            menu.post(event.x_root, event.y_root)

        _fileViewTree.bind('<Button-3>', displayMenu)
