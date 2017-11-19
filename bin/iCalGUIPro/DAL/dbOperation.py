import mysql.connector
from mysql.connector import errorcode
import re


class dbOperation():
    _dbName = ''
    _userName = ''
    _password = ''
    _conn = None

    def __init__(self, dbName, userName, Passwd):
        self._dbName = dbName;
        self._userName = userName;
        self._password = Passwd
        self._conn = mysql.connector.connect(host='131.104.48.64', database=self._dbName, user=self._userName,
                                             password=self._password, connect_timeout=3)
        _cursor = self._conn.cursor()
        try:
            _cursor.execute(
                "CREATE TABLE ORGANIZER(org_id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY,"
                "name VARCHAR(60) NOT NULL,contact VARCHAR(60) NOT NULL)")
        except mysql.connector.Error as err:
            if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
                pass
            else:
                print('Something wrong {}', format(err))
                _cursor.close()
                self._conn.close()
                exit(0)
        try:
            _cursor.execute(
                "CREATE TABLE EVENT(event_id INTEGER NOT "
                "NULL AUTO_INCREMENT PRIMARY KEY,summary VARCHAR(60) NOT NULL,start_time DATETIME NOT NULL,location "
                "VARCHAR(60),organizer INTEGER REFERENCES ORGANIZER(org_id) ON DELETE CASCADE,num_alarms INTEGER NOT "
                "NULL DEFAULT 0)")
        except mysql.connector.Error as err:
            if err.errno == errorcode.ER_TABLE_EXISTS_ERROR:
                pass
            else:
                print('Something wrong {}', format(err))
                _cursor.close()
                self._conn.close()
                exit(0)
        _cursor.close()

    def saveOrganizer(self, str):
        matchName = re.search(r'CN[ ]*=[ ]*', str, re.IGNORECASE)
        matchContact = re.search(r'[ ]*:[a-z ]+:[ ]*', str, re.IGNORECASE)
        mContact = re.search(r':[a-z ]+:[ ]*', str, re.IGNORECASE)
        name = str[matchName[1]:(matchContact[0] + 1)]
        contact = str[(mContact[0] + 1):len(str)]
        _cursor = self._conn.cursor()
        _cursor.execute('SELECT * FROM ORGANIZER ')

        _cursor.execute('INSERT INTO ORGANIZER (name,contact) VALUES (' + name + ',' + contact + ')')
# 'mysql+mysqlconnector://' + USR + ':' + PWD + '@131.104.48.64:3306/' + USR
