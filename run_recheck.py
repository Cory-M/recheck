# the python file should be located in the "build" folder, so no need to switch path when giving system instructions

# -*- coding:utf-8 -*-
import xlrd
import os

file_pos= "/home/wch/桌面/newcheck.xlsx"
#file_pos="/Users/cory/Desktop/唇读第四批反馈924人chenck.xlsx"

video path = '/home/wch/桌面/video'

files=os.listdir(dst_path)
for filename in files:
	print(filename)
	portion = os.path.splitext(filename)
	#newname = portion[0]+'.jpg'
	newname = portion[0].split('_')[0]+'.mp4'
	print(newname)
	os.chdir(dst_path)
	os.rename(filename,newname)
	pass

data = xlrd.open_workbook(file_pos)
table = data.sheets()[0]
data_list = []
print(table)
nrows = table.nrows
count=0
for index in range(500):
	data_list = table.row_values(index)
	#print(data_list)
	if not data_list[5].find('newcheck') == -1:
		print(data_list[5])
		count=count+1
		print(count,str(index))
		os.system('echo '+str(index)+'>> 4_file.out')
		os.system('./Example '+str(index))

# recheck_list=[512,575,605,630,686,726,808,835,846,851,856,866,868,869,871,872,890,899,904,919]
# for item in recheck_list:
# 	os.system('echo '+str(item)+'>> 4_file.out')
# 	os.system('./Example '+str(item))
