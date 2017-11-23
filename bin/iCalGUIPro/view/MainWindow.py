# some code from the book: programming python 4th edition
from tkinter.messagebox import *
from tkinter.filedialog import *
from tkinter.simpledialog import *
from tkinter.ttk import Treeview
from . import AboutWindow
from ..business import HelpMethod
from ..business import Calendar
from . import CreateCalendarWindow
from . import CreateEventWindow
from ctypes import *

import sys


class MainWindow(Tk):
    _filepath = ''
    _calObject = POINTER(Calendar.Calendar)()
    _currentIndex = 0
    _dbContext = None

    def close(*args):
        result = askyesno('Close program', 'Do you want to close?', parent=args[0])
        if result:
            if args[0]._calObject:
                Calendar.deleteCal(args[0]._calObject)
            args[0]._dbContext.closeConnection()
            sys.exit(0)

    def save(*args):
        if not args[0]._calObject:
            showerror('Error', 'You can not save, you need create or open a calendar first')
            return
        if len(args[0]._filepath) == 0:
            args[0].saveAs(args)
        else:
            returnResult = Calendar.writeCal(args[0]._filepath.encode('UTF-8'), args[0]._calObject)
            args[0].logInfoText(Calendar.printError(returnResult).decode('UTF-8'))
            if returnResult != 0:
                showerror('Error', 'Cannot Save...')
            return

    def saveAs(*args):
        if not args[0]._calObject:
            showerror('Error', 'You can not save, you need create or open a calendar first')
            return
        args[0]._filepath = asksaveasfilename(filetypes=[("ICal File", "*.ics")])
        if (len(args[0]._filepath) <= 4):
            return 'Continue'
        strArray = args[0]._filepath.split('/')
        args[0].changeTitle(strArray[len(strArray) - 1])
        returnResult = Calendar.writeCal(args[0]._filepath.encode('UTF-8'), args[0]._calObject)
        args[0].logInfoText(Calendar.printError(returnResult).decode('UTF-8'))
        if returnResult != 0:
            showerror('Error', 'Cannot Save...')
        return

    def deleteEvent(*args):
        if len(_fileViewTree.get_children()) == 1:
            showerror('Error', 'Only one event remining, you cannot delete it')
            return
        selection = _fileViewTree.selection()
        resultofDelete = askyesno('Info',
                                  'Are you sure to delete No. ' + str(
                                      _fileViewTree.item(selection)['values'][0]) + 'event?')
        if not resultofDelete:
            return
        result = Calendar.deleteEve(c_int(_fileViewTree.item(selection)['values'][0]), args[0]._calObject)
        args[0].logInfoText(Calendar.printError(result).decode('UTF-8'))
        rowResult = Calendar.getRowResult(args[0]._calObject)
        list = []
        arrayLength = int(rowResult[0].decode('UTF-8'))
        for i in range(arrayLength):
            list.append(rowResult[i + 1].decode('UTF-8'))
        _fileViewTree.delete(*_fileViewTree.get_children())
        for item in list:
            _fileViewTree.insert('', 'end', values=item.split('*SP*'))

    def open(*args):
        if args[0]._calObject:
            result = askyesno('Create New Calendar',
                              'One Cal obj in program, if continue, it will lost, make sure you already saved it.',
                              parent=args[0])
            if result == False:
                return
            else:
                Calendar.deleteCal(args[0]._calObject)
        args[0]._filepath = askopenfilename(filetypes=[("ICal File", "*.ics")])
        if (len(args[0]._filepath) == 0):
            return
        strArray = args[0]._filepath.split('/')
        args[0].changeTitle(strArray[len(strArray) - 1])
        returnResult = Calendar.createCal(args[0]._filepath.encode('UTF-8'), byref(args[0]._calObject))
        args[0].logInfoText(Calendar.printError(returnResult).decode('UTF-8'))
        if returnResult != 0:
            showerror('Error', 'File not valid, read fail.')
            return
        rowResult = Calendar.getRowResult(args[0]._calObject)
        list = []
        arrayLength = int(rowResult[0].decode('UTF-8'))
        for i in range(arrayLength):
            list.append(rowResult[i + 1].decode('UTF-8'))
        _fileViewTree.delete(*_fileViewTree.get_children())
        for item in list:
            _fileViewTree.insert('', 'end', values=item.split('*SP*'))

    def createCalendar(*args):
        if args[0]._calObject:
            result = askyesno('Create New Calendar',
                              'One Cal obj in program, if continue, it will lost, make sure you already saved it.',
                              parent=args[0])
            if result == False:
                return
            else:
                Calendar.deleteCal(args[0]._calObject)
        calWin = CreateCalendarWindow.CreateCalendarWindow(args[0])
        args[0].wait_window(calWin.top)
        if (len(calWin.calArgs) == 0):
            return
        array = HelpMethod.newCalParameterArray(calWin.calArgs)
        Calendar.createCalGui(2.0, array[2].value, array[0].value, array[4].value, array[1].value,
                              byref(args[0]._calObject))
        rowResult = Calendar.getRowResult(args[0]._calObject)
        list = []
        arrayLength = int(rowResult[0].decode('UTF-8'))
        for i in range(arrayLength):
            list.append(rowResult[i + 1].decode('UTF-8'))
        _fileViewTree.delete(*_fileViewTree.get_children())
        _fileViewTree.insert('', 'end', values=list[0].split('*SP*'))
        args[0].logInfoText(Calendar.printError(c_int(0)).decode('UTF-8'))

    def createEvent(*args):
        if not args[0]._calObject:
            showerror('Error', 'Can not create event without create calender')
            return
        eventWin = CreateEventWindow.CreateEventWindow(args[0])
        args[0].wait_window(eventWin.top)
        array = HelpMethod.newEventParameterArray(eventWin.eventArgs)
        Calendar.createEve(array[0].value, array[2].value, array[1].value, args[0]._calObject)
        rowResult = Calendar.getRowResult(args[0]._calObject)
        list = []
        arrayLength = int(rowResult[0].decode('UTF-8'))
        for i in range(arrayLength):
            list.append(rowResult[i + 1].decode('UTF-8'))
        _fileViewTree.delete(*_fileViewTree.get_children())
        for item in list:
            _fileViewTree.insert('', 'end', values=item.split('*SP*'))
        args[0].logInfoText(Calendar.printError(c_int(0)).decode('UTF-8'))

    def showAlarms(*args):
        selection = _fileViewTree.selection()
        alarm = Calendar.printEveAl(c_int(_fileViewTree.item(selection)['values'][0]), args[0]._calObject).decode(
            'UTF-8')
        args[0].logInfoText(alarm)

    def extractProps(*args):
        selection = _fileViewTree.selection()
        property = Calendar.printEveOp(c_int(_fileViewTree.item(selection)['values'][0]), args[0]._calObject).decode(
            'UTF-8')
        args[0].logInfoText(property)

    def about(*args):
        AboutWindow.dialog()

    def saveAllEvent(*args):
        if not args[0]._calObject:
            showerror('Error', 'Can not storage to db, you need create or open a calendar first')
            return
        eventListForDB = Calendar.getEventForDB(args[0]._calObject)
        list = []
        arrayLength = int(eventListForDB[0].decode('UTF-8'))
        for i in range(arrayLength):
            list.append(eventListForDB[i + 1].decode('UTF-8').split('*SP*'))
        for items in list:
            if len(items[3] != 0):
                args[0]._dbContext.saveOrganizer(items[3])

        # queryValueList = ''
        # for item in items:
        #     queryValueList = queryValueList + item + ','
        # saveAllEventCommand = 'INSERT INTO EVENT'
        # todo save to db

    def saveCurrEvent(*args):
        if not args[0]._calObject:
            showerror('Error', 'Can not storage to db, you need create or open a calendar first')
            return
        selection = _fileViewTree.selection()
        if (len(selection) == 0):
            showerror('Error', 'Can not storage to db, you need select an Event first')
            return
        # todo save to db

    def clearDB(*args):
        # todo delete all data in db
        showinfo('Not finished', 'Not finished')

    def disDBStatus(*args):
        # todo db status line
        showinfo('Not finished', 'Not finished')

    def exeQuery(*args):
        # todo db query function
        showinfo('Not finished', 'Not finished')

    def cleanOnClick(*args):
        _globelLogInfoText.config(state=NORMAL)
        _globelLogInfoText.delete('1.0', END)
        _globelLogInfoText.config(state=DISABLED)

    def __init__(self, dbContext):
        self._dbContext = dbContext
        Tk.__init__(self)
        HelpMethod.centerOfWindow(self, 580, 485)
        self.resizable(width=False, height=False)
        menuBar = Menu(self)
        menuFile = Menu(menuBar, tearoff=False)
        menuCreate = Menu(menuBar, tearoff=False)
        menuDataBase = Menu(menuBar, tearoff=False)
        menuHelp = Menu(menuBar, tearoff=False)
        self.makeFileViewPanel()
        self.makeLogPanel()
        self.makeMenuFile(menuFile, menuBar)
        self.makeMenuCreate(menuCreate, menuBar)
        self.makeMenuDB(menuDataBase, menuBar)
        self.makeMenuHelp(menuHelp, menuBar)
        self.config(menu=menuBar)
        self.bind_all('<Control-x>', self.close)
        self.bind_all('<Control-s>', self.save)
        self.bind_all('<Control-o>', self.open)
        self.bind_all('<Control-m>', self.saveAs)
        self.makeRightClickMenu()
        self.protocol('WM_DELETE_WINDOW', self.close)

    def makeMenuFile(self, menuFile, menuBar):
        menuFile.add_command(label='Open...', command=self.open, accelerator='Ctrl+O', underline=0)
        menuFile.add_command(label='Save', command=self.save, accelerator='Ctrl+S', underline=0)
        menuFile.add_command(label='Save as...', command=self.saveAs, accelerator='Ctrl+M', underline=1)
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

    def makeMenuDB(self, menuDataBase, menuBar):
        menuDataBase.add_command(label='Store All Events', command=self.saveAllEvent, underline=0)
        menuDataBase.add_command(label='Store Current Events', command=self.saveCurrEvent, underline=1)
        menuDataBase.add_command(label='Clear All Data', command=self.clearDB, underline=0)
        menuDataBase.add_command(label='Display DB Status', command=self.disDBStatus, underline=0)
        menuDataBase.add_command(label='Execute Query', command=self.exeQuery, underline=0)
        menuBar.add_cascade(label='Database', menu=menuDataBase, underline=0)

    def makeFileViewPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=0, width=580, height=280)
        scrollBar = Scrollbar(panel)
        scrollBar.pack(side=RIGHT, fill=Y)
        global _fileViewTree
        _fileViewTree = Treeview(panel, columns=('c1', 'c2', 'c3', 'c4'), show='headings',
                                 yscrollcommand=scrollBar.set)
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

    def makeLogPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=285, width=580, height=195)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=580, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=580, height=165)
        label = Label(panelInsideUp, text='Log Info:', width=10, anchor='w', font=('Helvetica', 12)).grid(row=0,
                                                                                                          column=0)
        button = Button(panelInsideUp, text='Clear', width=5, command=self.cleanOnClick).grid(row=0, column=1)
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
        _globelLogInfoText.insert(INSERT, text)
        _globelLogInfoText.config(state=DISABLED)
        _globelLogInfoText.see(END)

    def makeRightClickMenu(self):
        menu = Menu(self, tearoff=0)
        menu.add_command(label='Show Alarms', command=self.showAlarms)
        menu.add_command(label='Extract optional props', command=self.extractProps)
        menu.add_command(label='Delete this event', command=self.deleteEvent)

        def displayMenu(event):
            item = _fileViewTree.identify_row(event.y)
            if item:
                _fileViewTree.selection_set(item)
                menu.tk_popup(event.x_root, event.y_root)
            else:
                pass

        _fileViewTree.bind('<ButtonRelease-3>', displayMenu)

    def changeTitle(self, fileName):
        self.title("iCalGUI - " + fileName)
