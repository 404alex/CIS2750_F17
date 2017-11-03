from tkinter import *
from tkinter.messagebox import *
import sys


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

    def __init__(self):
        Tk.__init__(self)
        menubar = Menu(self)
        menuFile = Menu(menubar, tearoff=False)
        menuFile.add_command(label='Open...', command=self.open, accelerator='Ctrl+O', underline=0)
        menuFile.add_command(label='Save', command=self.save, accelerator='Ctrl+S', underline=0)
        menuFile.add_command(label='Save as...', command=self.saveAs, underline=0)
        menuFile.add_command(label='Exit', command=self.close, accelerator='Ctrl+X', underline=0)
        menubar.add_cascade(label='File', menu=menuFile, underline=0)
        self.config(menu=menubar)
        self.bind_all('<Control-x>', self.close)
        self.bind_all('<Control-s>', self.save)
        self.bind_all('<Control-o>', self.open)

if __name__ == '__main__':
    root = MainWindow()
    root.title('iCalGUI')
    root.mainloop()
