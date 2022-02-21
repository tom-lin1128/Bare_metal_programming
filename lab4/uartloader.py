import serial
import time
import os
Portname = '/dev/pts/5' 
Filename = 'kernel1.img'

def send_img():
    with open(Filename,'rb',buffering = 0) as F:
        byte = F.read() 
        tty = serial.Serial(Portname,115200)

        file_size = os.path.getsize(Filename)
        tty.write("lm\n".encode())
        #time.sleep(3)
        print(f'Sending kernel [ %d ] byte' %file_size)
        tty.write(f"{file_size}\n".encode())

 
        tty.write(byte)
        #print(byte)
        print('Send complete')
        tty.close()
        
send_img()
            