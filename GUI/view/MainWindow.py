from tkinter import *
from tkinter.messagebox import *
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
        HelpMethod.centerOfWindow(self, 600, 300)
        self.resizable(width=False, height=False)
        menuBar = Menu(self)
        menuFile = Menu(menuBar, tearoff=False)
        menuCreate = Menu(menuBar, tearoff=False)
        menuHelp = Menu(menuBar, tearoff=False)
        self.makeMenuFile(menuFile, menuBar)
        self.makeMenuCreate(menuCreate, menuBar)
        self.makeMenuHelp(menuHelp, menuBar)
        self.config(menu=menuBar)
        self.bind_all('<Control-x>', self.close)
        self.bind_all('<Control-s>', self.save)
        self.bind_all('<Control-o>', self.open)

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
