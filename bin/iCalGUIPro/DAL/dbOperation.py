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
                print('Something wrong: {}'.format(err))
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
                print('Something wrong: {}'.format(err))
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
            print('Something wrong, exiting :{}'.format(err))
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
            print('Something wrong, exiting: {}'.format(err))
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
            print('Something wrong, exiting: {}'.format(err))
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
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def allEventOrderByDate(self):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute('SELECT * FROM EVENT ORDER BY start_time')
            event = _cursor.fetchall()
            _cursor.close()
            string = self.formatResult(event)
            return string
        except mysql.connector.Error as err:
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def getEventByOrgName(self, string):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute("SELECT * FROM ORGANIZER WHERE name = '" + string + "'")
            orgs = _cursor.fetchall()
            string = []
            for org in orgs:
                _cursor.execute("SELECT * FROM EVENT WHERE organizer = '" + str(org[0]) + "'")
                a = _cursor.fetchall()
                a[0] = a[0] + org
                string = string + a
            _cursor.close()
            string = self.formatOrgResult(string)
            return string
        except mysql.connector.Error as err:
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def getEventByLocation(self, string):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute("SELECT * FROM EVENT WHERE location = '" + string + "'")
            event = _cursor.fetchall()
            _cursor.close()
            string = self.formatResult(event)
            return string
        except mysql.connector.Error as err:
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def getEventByAlarm(self, string):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute("SELECT * FROM EVENT WHERE num_alarms= '" + string + "'")
            event = _cursor.fetchall()
            _cursor.close()
            string = self.formatResult(event)
            return string
        except mysql.connector.Error as err:
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def getEventByUser(self, string):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute(string)
            event = _cursor.fetchall()
            _cursor.close()
            string = self.formatByUser(event)
            return string
        except mysql.connector.Error as errorcode:
            return 'Something wrong: {}\n\n'.format(errorcode)

    def helpButton(self):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute("DESCRIBE ORGANIZER")
            event = []
            a = []
            a.append("Table Name: ORGANIZER")
            event.append(a)
            event = event + _cursor.fetchall()
            _cursor.execute("DESCRIBE EVENT")
            b=[]
            b.append("Table Name: EVENT")
            event.append(b)
            event = event + _cursor.fetchall()
            _cursor.close()
            string = self.formatDes(event)
            return string
        except mysql.connector.Error as errorcode:
            return 'Something wrong: {}\n\n'.format(errorcode)

    def getContactInfoByOrg(self, string):
        try:
            _cursor = self._conn.cursor(buffered=True)
            _cursor.execute("SELECT * FROM ORGANIZER WHERE name = '" + string + "'")
            event = _cursor.fetchall()
            _cursor.close()
            string = self.formatContactInfo(event)
            return string
        except mysql.connector.Error as err:
            print('Something wrong, exiting: {}'.format(err))
            exit(0)

    def formatResult(self, results):
        if len(results) == 0:
            return 'Nonthing found\n\n\n'
        str = '|' + 'ID'.ljust(4) + '|' + 'Start Date'.ljust(20) + '|' + 'Location'.ljust(20) + '|' + 'Organizer'.ljust(
            15) + '|' + 'Alarms Count'.ljust(14) + '|' + 'Summary'.ljust(50) + '|\n'
        for value in results:
            str = str + ("|" + "{}".format(value[0]).ljust(4) + "|" + "{:%Y-%m-%d %H:%m:%S}".format(
                value[2]).ljust(20) + "|" + "{}".format(value[3]).ljust(20) + "|" + "{}".format(
                value[4]).ljust(15) + "|" + "{}".format(value[5]).ljust(14) + "|" + "{}".format(value[1]).ljust(
                50) + "|\n")
        str = str + '\n\n'
        return str

    def formatOrgResult(self, results):
        if len(results) == 0:
            return 'Nonthing found\n\n\n'
        str = '|' + 'ID'.ljust(4) + '|' + 'Start Date'.ljust(20) + '|' + 'Location'.ljust(20) + '|' + 'Organizer'.ljust(
            15) + '|' + 'Alarms Count'.ljust(14) + '|' + 'Summary'.ljust(50) + '|\n'
        for value in results:
            str = str + ("|" + "{}".format(value[0]).ljust(4) + "|" + "{:%Y-%m-%d %H:%m:%S}".format(
                value[2]).ljust(20) + "|" + "{}".format(value[3]).ljust(20) + "|" + "{}".format(
                value[7]).ljust(15) + "|" + "{}".format(value[5]).ljust(14) + "|" + "{}".format(value[1]).ljust(
                50) + "|\n")
        str = str + '\n\n'
        return str

    def formatContactInfo(self, results):
        if len(results) == 0:
            return 'Nonthing found\n\n\n'
        str = '|' + 'Name'.ljust(20) + '|' + 'Contact Info'.ljust(50) + '|\n'
        for value in results:
            str = str + ("|" + "{}".format(value[1]).ljust(20) + "|" + "{}".format(
                value[2]).ljust(50) + "|\n")
        str = str + '\n\n'
        return str

    def formatByUser(self, results):
        if len(results) == 0:
            return 'Nonthing found\n\n\n'
        string = ''
        for value in results:
            for re in value:
                string = string + str(re) + '\t\t'
            string = string + '\n'
        string = string + '\n\n\n'
        return string

    def formatDes(self, result):
        if (len(result) == 0):
            return 'Nothing found\n\n\n'
        string = ''
        for value in result:
            for re in value:
                string = string + "|" + str(re).ljust(20)
            string = string + '\n'
        return string

# 'mysql+mysqlconnector://' + USR + ':' + PWD + '@131.104.48.64:3306/' + USR
