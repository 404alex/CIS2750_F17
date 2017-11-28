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
        try:
            matchName = re.search(r'CN[ ]*=[ ]*', str, re.IGNORECASE)
            matchContact = re.search(r'[ ]*[;:][a-z ]+[;:][ ]*', str, re.IGNORECASE)
            mContact = re.search(r'[;:][a-z ]+[;:][ ]*', str, re.IGNORECASE)
            name = str[matchName.span()[1]:matchContact.span()[0]]
            contact = str[(mContact.span()[0] + 1):len(str)]
            _cursor = self._conn.cursor()
            str1 = "INSERT INTO ORGANIZER (name,contact) VALUES ('" + name + "','" + contact + "')"
            _cursor.execute(str1)
            id = _cursor.lastrowid
            _cursor.close()
            return id
        except mysql.connector.Error as err:
            print('Something wrong, exiting {}', format(err))
            exit(0)

    def saveEvent(self, str):
        try:
            _cursor = self._conn.cursor()
            _cursor.execute(str)
            id = _cursor.lastrowid
            _cursor.close()
            self._conn.commit()
            return 'Insert Success\n'
        except mysql.connector.Error as err:
            print('Something wrong, exiting {}', format(err))
            exit(0)

    def deleteAllData(self):
        try:
            _cursor = self._conn.cursor()
            _cursor.execute('DELETE FROM EVENT')
            _cursor.execute('DELETE FROM ORGANIZER')
            _cursor.close()
            self._conn.commit()
            stat = self.status()
            return 'Delete Success\n' + stat
        except mysql.connector.Error as err:
            print('Something wrong, exiting {}', format(err))
            exit(0)

    def closeConnection(self):
        self._conn.close()

    def status(self):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute('SELECT * FROM EVENT')
            eventRowCount = _cursor.rowcount
            _cursor.execute('SELECT * FROM ORGANIZER')
            orgRowCount = _cursor.rowcount
            _cursor.close()
            return 'Database has ' + str(orgRowCount) + ' organizers and ' + str(eventRowCount) + ' events\n'
        except mysql.connector.Error as err:
            print('Something wrong, exiting {}', format(err))
            exit(0)

    def allEventOrderByDate(self):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute('SELECT * FROM EVENT ORDER BY start_time')
            event = _cursor.fetchall()
            _cursor.close()
            str = self.formatResult(event)
            return str
        except mysql.connector.Error as err:
            print('Something wrong, exiting {}', format(err))
            exit(0)

    def formatResult(self, results):
        str = '|' + 'ID'.ljust(5) + '|' + 'Start Date'.ljust(20) + '|' + 'Location'.ljust(20) + '|' + 'Organizer'.ljust(
            10) + '|' + 'Alarms Count'.ljust(14) + '|' + 'Summary'.ljust(50) + '|\n'
        for value in results:
            str = str + ("|" + "{}".format(value[0]).ljust(5) + "|" + "{:%Y-%m-%d %H:%m:%S}".format(
                value[2]).ljust(20) + "|" + "{}".format(value[3]).ljust(20) + "|" + "{}".format(
                value[4]).ljust(10) + "|" + "{}".format(value[5]).ljust(14) + "|" + "{}".format(value[1]).ljust(
                50) + "|\n")
        return str

# 'mysql+mysqlconnector://' + USR + ':' + PWD + '@131.104.48.64:3306/' + USR
