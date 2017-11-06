#-*- coding:utf-8 -*-
# import sys
# # reload(sys)
# sys.setdefaultencoding('utf8')
import xlrd
import urllib.request
import os
import subprocess
try:
    from urllib import quote  # Python 2.X
except ImportError:
    from urllib.parse import quote  # Python 3+
src_path = '/home/wch/桌面/4th_check/4_924.xlsx'
#src_folder = '/Users/cory/Desktop/'
dst_folder = os.path.splitext(src_path)[0]

if not os.path.isdir(dst_folder):
    os.mkdir(dst_folder)
data = xlrd.open_workbook(src_path)
table = data.sheets()[0]
data_list = []
print(table)
nrows = table.nrows

for row_index in range(84, nrows):
    data_list = table.row_values(row_index)
    idx = 0
    print('######')
    print(row_index)

    jpg_format = ''
    video_format = ''
    st_label_str = ''
    ed_label_str = ''

    for item in data_list:
        print(item)
        idx += 1
        if idx == 1:
            fnm = str(int(item))
        elif idx == 2:
            print('---Downloading jpg....')
            jpg_url = item
            # for ch in jpg_url.rstrip('\r\n').split("/")[-1].decode('utf-8'):
            #     if u'\u4e00' <= ch <= u'\u9fff':
            #         print ('CHINESE...\n')
            #         jpg_url = quote(jpg_url.encode('utf8'), safe=':|/')

            jpg_format = jpg_url.rstrip('\r\n').split('.')[-1]
        elif idx == 3:
            print('---Downloading video....')
            video_url = item
            video_format = video_url.rstrip('\r\n').split('.')[-1]
            # for ch in video_url.rstrip('\r\n').split("/")[-1].decode('utf-8'):
            #     if u'\u4e00' <= ch <= u'\u9fff':
            #         print ('CHINESE...\n')
            #         video_url = quote(video_url.encode('utf8'), safe=':|/')
        else:
            print('----Get_the_labels...')
            st_ed_record = item
            #print(st_ed_record)
            st_label_str = st_ed_record.split('-')[0]
            #print('#############')
            #print(st_label_str)
            ed_label_str = st_ed_record.split('-')[1]
           
            # jpg_downloading

            path=dst_folder + '/' + str(row_index) + '_' + fnm + '_label_' + st_label_str + '_' + ed_label_str + '.' + jpg_format
            print(path)
            urllib.request.urlretrieve(jpg_url, dst_folder + '/' + str(row_index) + '_' + fnm + '_label_' + st_label_str + '_' + ed_label_str + '.' + 'jpg')
            #video downloading
            dst_fnm = str(row_index) + '_' + fnm + '_label_' + st_label_str + '_' + ed_label_str
            command_data = 'you-get ' + '-o ' + dst_folder + ' -O ' + dst_fnm + ' ' + video_url + ' --debug'
            print(command_data)
            subprocess.getstatusoutput(command_data)
        	
        	 
        	

