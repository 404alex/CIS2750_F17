from tkinter import *
from tkinter.messagebox import *
import sys
from iCalGUIPro.view import MainWindow
from iCalGUIPro.DAL import dbOperation
import mysql.connector


def dbExceptionHandle(err):
    if ('timed out' in err) or ('101' in err):
        print('Can not connect to server, please check internet connection, or using school VPN, exit')
    if ('1045' in err):
        print('Wrong password or username')
        return 0
    if ('1044' in err):
        print('Wrong database name')
        return 1
    exit(0)


if __name__ == '__main__':
    count = 0
    status = -1
    global _dbContext
    if (len(sys.argv) > 2):
        print('Wrong arguments number. Exit.')
        exit(0)
    elif len(sys.argv) == 2:
        count = count + 1
        password = input('Please input password: ')
        database = input('Please input database name: ')
        try:
            _dbContext = dbOperation.dbOperation(database, sys.argv[1], password)
        except mysql.connector.Error as err:
            status = dbExceptionHandle(str(err))
            if status == 0 or status == 1:
                while True:
                    if (count > 2):
                        print('Too much attampts, exit')
                        exit(0)
                    count = count + 1
                    username = input('Please input username: ')
                    password = input('Please input password: ')
                    database = input('Please input database name: ')
                    try:
                        _dbContext = dbOperation.dbOperation(database, username, password)
                        break
                    except mysql.connector.Error as err:
                        status = dbExceptionHandle(str(err))
    else:
        try:
            _dbContext = dbOperation.dbOperation('cchen22', 'cchen22', '0951136')
        except mysql.connector.Error as err:
            status = dbExceptionHandle(str(err))

    root = MainWindow.MainWindow(_dbContext)
    root.title('iCalGUI')
    root.mainloop()
