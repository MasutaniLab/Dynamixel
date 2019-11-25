#!/usr/bin/env python
# -*- coding: utf-8 -*-
# -*- Python -*-

from __future__ import print_function

import sys
if sys.version_info[0] == 2:
	from Tkinter import *
else:
	from tkinter import *
#import thread
import dummy_threading
import time

class SliderMulti(Frame):
	def __init__(self, channels, title="", varType="int", master=None,
		     button=False, buttonText=""):
		Frame.__init__(self, master)
		self.init(channels, varType)
		if button == True: 
		    self.initButton(text=buttonText)
		self.master.title(title)
		self.pack()


	def init(self, channels, varType):
		self._channels = channels
		self.var = [] 
		self.scales = []
		self.option_add('*font', 'system 9')
		
		i = 0
		for channel in self._channels:
			if varType == "int":
				self.var.append(IntVar())
			elif varType == "double":
				self.var.append(DoubleVar())
			elif varType == "string":
				self.var.append(StringVar())
			else:
				raise ValueError("unknown varType")
			self.scales.append(
				Scale(self,
				      label=channel[0],
				      variable=self.var[i],
				      to=channel[1],
				      orient=VERTICAL,
				      from_=channel[2],
				      resolution=channel[3],
				      length=channel[4]))
			if len(channel) == 6:
				self.scales[i].set(channel[5])
			self.scales[i].pack(side=LEFT)
			i = i + 1

	def buttonCommand(self):
		self.new = True

	def buttonNew(self):
		if (self.new == True):
			self.new = False
			return True
		return False
	       
	def initButton(self, text=None):
		self.new = False
		button = Button(self, text=text, command=self.buttonCommand)
		button.pack()

	def get(self):
		val = []
		for v in self.var:
			val.append(v.get())

		return val

	def set(self, value):
		i = 0
		for v in value:
			self.var[i].set(v)
			i = i + 1
		return




def test ():
	channels = (("angle", 0, 360, 0.1, 200, 180), ("velocity", -100, 100, 0.1, 200), ("hoge", 0.1, 0.2, 0.01, 200, 0.15))
	slider = SliderMulti(channels,
			     varType = "double", title = "hoge",
			     button=True, buttonText="foobar")
	sth = dummy_threading.Thread(target=slider.mainloop, args=())
	sth.start()
#	thread.start_new_thread(slider.mainloop, ())

	while (1):
		print(slider.get())
		time.sleep(0.5)
#		slider.update()

if  __name__ == '__main__': test()
