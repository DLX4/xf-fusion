#!/bin/sh  
exe="fusion" #你需要发布的程序名称
des="/home/dlx/Desktop/qt-fusion/release/" #步骤1中的目录即本文件所在目录
deplist=$(ldd $exe | awk  '{if (match($3,"/")){ printf("%s "),$3 } }')  
cp $deplist $des
