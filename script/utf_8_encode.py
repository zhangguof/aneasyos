#-*- coding:utf-8 -*-
import os
src_path = "aneasyos"
file_ext = [".c",".cpp",".asm",".h",".inc","makefile","makefile_linux",]

def encode_utf8(file_path):
	s = ""
	with open(file_path,"rb") as f:
		s = f.read()
	with open(file_path,"wb") as f:
		f.write(s.decode("gbk").encode("utf-8"))
	print "encoding file:%s"%file_path

def walk_path(path):
	for root,dirs,files in os.walk(src_path):
		for filename in files:
			for ext in file_ext:
				if filename.endswith(ext):
					file_path = os.path.join(root,filename)
					encode_utf8(file_path)
					break

def main():
	walk_path(src_path)

if __name__ == "__main__":
	main()