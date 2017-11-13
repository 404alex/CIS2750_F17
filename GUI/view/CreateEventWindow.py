from tkinter import *
from GUI.business import HelpMethod
from tkinter.messagebox import *
import uuid
from datetime import datetime, timezone


class CreateEventWindow:
    isCancel = False
    eventArgs = {}

    def cancel(*args):
        result = askyesno('Cancel Action', 'Do you want to cancel?', parent=args[0].top)
        if result:
            args[0].isCancel = True
            args[0].eventArgs = {}
            args[0].top.destroy()

    def save(*args):
        _year = args[0].top.nametowidget('_textStartYear').get()
        _month = args[0].top.nametowidget('_textMonth').get()
        _day = args[0].top.nametowidget('_textDay').get()
        _hour = args[0].top.nametowidget('_textHours').get()
        _min = args[0].top.nametowidget('_textMinutes').get()
        _second = args[0].top.nametowidget('_textSeconds').get()
        if len(_year) == 0 or len(_month) == 0 or len(_day) == 0 or len(_hour) == 0 or len(_min) == 0 or len(
                _second) == 0:
            result = askyesno('Info', 'Not provided full start date time, use creation date?', parent=args[0].top)
            if result:
                args[0].eventArgs['startDateTime'] = args[0].eventArgs['creationDateTime']
            else:
                return
        else:
            _year = _year.zfill(4)
            _month = _month.zfill(2)
            _day = _day.zfill(2)
            _hour = _hour.zfill(2)
            _min = _min.zfill(2)
            _second = _second.zfill(2)
            if int(_min) >= 60 or int(_min) < 0:
                showerror('Error', 'Wrong Minutes(0~59)')
                return
            elif int(_year) < 1000:
                showerror('Error', 'Wrong Year(1000~9999)')
                return
            elif int(_month) < 1 or int(_month) > 12:
                showerror('Error', 'Wrong Month(1~12)')
                return
            elif int(_day) < 1 or int(_day) > 31:
                showerror('Error', 'Wrong Days(1~31)')
                return
            elif int(_hour) < 0 or int(_hour) > 23:
                showerror('Error', 'Wrong hours(0~23)')
                return
            elif int(_second) >= 60 or int(_second) < 0:
                showerror('Error', 'Wrong Seconds(0~59)')
                return
            else:
                _dateString = _year + _month + _day + _hour + _min + _second + 'GMT-0500'
                _UTCStartDateTime = datetime.strptime(_dateString, '%Y%m%d%H%M%SGMT%z')
                args[0].eventArgs['startDateTime'] = _UTCStartDateTime.strftime('%Y%m%dT%H%M%SZ')
        args[0].top.destroy()

    def __init__(self, parent):
        win = self.top = Toplevel(parent)
        fourDigiVali = win.register(fourDigiValidate)
        twoDigiVali = win.register(twoDigiValidate)
        HelpMethod.centerOfWindow(win, 375, 189)
        win.resizable(width=False, height=False)
        win.focus_set()
        win.grab_set()
        win.protocol("WM_DELETE_WINDOW", self.cancel)
        win.title('Create Event')
        win.grid_columnconfigure(1, weight=1)

        Label(win, text='UUID: ').grid(row=0, column=0)
        Entry(win, name='_textUUID', width=33).grid(row=0, column=1, columnspan=2)
        Label(win, text='Creation Date: ').grid(row=1, column=0)
        Entry(win, name='_textDate', width=33).grid(row=1, column=1, columnspan=2)

        Label(win, text='Start Year: ').grid(row=2, column=0)
        Entry(win, name='_textStartYear', width=33, validate='key', validatecommand=(fourDigiVali, '%P')).grid(row=2,
                                                                                                               column=1,
                                                                                                               columnspan=2)
        Label(win, text='Start Month: ').grid(row=3, column=0)
        Entry(win, name='_textMonth', width=33, validate='key', validatecommand=(twoDigiVali, '%P')).grid(row=3,
                                                                                                          column=1,
                                                                                                          columnspan=2)
        Label(win, text='Start Day: ').grid(row=4, column=0)
        Entry(win, name='_textDay', width=33, validate='key', validatecommand=(twoDigiVali, '%P')).grid(row=4, column=1,
                                                                                                        columnspan=2)
        Label(win, text='Start Hours (24H): ').grid(row=5, column=0)
        Entry(win, name='_textHours', width=33, validate='key', validatecommand=(twoDigiVali, '%P')).grid(row=5,
                                                                                                          column=1,
                                                                                                          columnspan=2)
        Label(win, text='Start Minutes: ').grid(row=6, column=0)
        Entry(win, name='_textMinutes', width=33, validate='key', validatecommand=(twoDigiVali, '%P')).grid(row=6,
                                                                                                            column=1,
                                                                                                            columnspan=2)
        Label(win, text='Start Seconds: ').grid(row=7, column=0)
        Entry(win, name='_textSeconds', width=33, validate='key', validatecommand=(twoDigiVali, '%P')).grid(row=7,
                                                                                                            column=1,
                                                                                                            columnspan=2)

        calUUID = str(uuid.uuid4())
        win.nametowidget('_textUUID').insert(0, calUUID)
        win.nametowidget('_textUUID').config(state='readonly')
        _dateTimeNow = datetime.utcnow()
        _displayDateTime = utc_to_local(_dateTimeNow).strftime('%Y-%m-%d %H:%M:%S')
        win.nametowidget('_textDate').insert(0, _displayDateTime)
        win.nametowidget('_textDate').config(state='readonly')

        self.eventArgs['UID'] = calUUID
        self.eventArgs['creationDateTime'] = _dateTimeNow.strftime('%Y%m%dT%H%M%SZ')

        Button(win, text='Ok', command=self.save, width=5).grid(row=8, column=1, sticky=E)
        Button(win, text='Cancel', command=self.cancel).grid(row=8, column=2, sticky=W)


def utc_to_local(utc_dt):
    return utc_dt.replace(tzinfo=timezone.utc).astimezone(tz=None)


def local_to_utc(local_dt):
    return local_dt.replace(tzinfo=None).astimezone(tz=timezone.utc)


def fourDigiValidate(content):
    if (content.isdigit() or (content == '')) and (len(content) <= 4):
        return True
    else:
        return False


def twoDigiValidate(content):
    if (content.isdigit() or (content == '')) and (len(content) <= 2):
        return True
    else:
        return False
