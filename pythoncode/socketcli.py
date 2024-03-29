import os
import asyncio  #비동기
import socket
import struct
import cv2
from keras.datasets import mnist
from keras.models import Sequential
from keras.layers import Dense, Activation
import numpy as np
from tensorflow import keras
from numpy import argmax
from keras.models import load_model
from keras.preprocessing import image
from keras.src.legacy.preprocessing.image import ImageDataGenerator
from keras.preprocessing import image
model = keras.models.load_model('C:/Users/AIOT1/Desktop/deepresult/newmodel4.h5')

# ===============================
Host = '10.10.20.108'
Port = 9190

# 이미지 받기
async def recv_image(client_socket, count):
    # await asyncio.sleep(1)
    data_transferred = 0

    # fn = client_socket.recv(256).decode('utf-16')

    # 파일 사이즈 받기
    reSize = client_socket.recv(16)

    sss=int(reSize.decode('utf-16').split('\0')[0])
    # size = struct.unpack("!I", reSize)[0]
    print(reSize)
    # print(size)
    print(sss)
    # size = int((reSize.decode('utf-16')).split('&', 1)[0])


    # 현재 디렉토리
    current_dir = os.getcwd()
    with open(current_dir + '/fx/' + 'AK4700101'+str(count) +'.jpg', 'wb') as f:
        try:
            print("들어왔당~ㅎㅁㅎ!")
            remainig_size = sss;
            while remainig_size > 0:
                data = client_socket.recv(sss)
                print(len(data))

                f.write(data)
                data_transferred += len(data)
                remainig_size -= len(data)
                if remainig_size==0:
                    break
                # data = client_socket.recv(size)  # 파일 내용 수신
                # print(3)
            path = current_dir + '/fx/' + 'AK4700101'+str(count) +'.jpg'
            # print(path)
            test_img = image.load_img(path,  target_size=(300, 300,3))
            x = image.img_to_array(test_img)
            x = np.expand_dims(x, axis=0)
            images = np.vstack([x])
            classes = model.predict(images, batch_size=10)
            print(classes)
            objectstate=""
            if classes[0][0] == 1:
                print("1")
                objectstate = "불량^찌그러짐^"
            elif classes[0][1] == 1:
                print("2")
                objectstate = "불량^풀탭없음^"
            elif classes[0][2] == 1:
                print("3")
                objectstate = "정상^정상^"
            elif classes[0][3] == 1:
                print("4")
                objectstate = "불량^이물질^"
            else:
                print("5")
                objectstate = "불량^알수없는 물체^"
            print('파일 전송종료. 전송량[%d]' % (data_transferred))
            forserv="^2^AK4700101"+str(count)+"^"+objectstate
            client_socket.send(forserv.encode('utf-16'))
        except Exception as e:
            print(e)
        # finally:

        #     message = '파일 전송 완료'
        #

async def main():
    # MFC 서버와 연결
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (Host, Port)
    # 서버 소켓에 접속
    client_socket.connect(server_address)

    count = 0

    while True:
        # 메세지 구분하기
        # sendMsg = client_socket.recv(256)
        # print(type(sendMsg))
        # splitMsg = int((sendMsg.decode('utf-16')).split('&', 1)[0])
        # print(splitMsg)

        # if splitMsg == 1 :
        #     taskMsg = asyncio.create_task(send_msg(client_socket))
        #     await taskMsg
        # elif splitMsg == 2 :
        count += 1
        taskImage = asyncio.create_task(recv_image(client_socket, count))
        await taskImage

        # taskImage = asyncio.create_task(recv_image(client_socket))
        # taskMsg = asyncio.create_task(send_msg(client_socket))
        # await taskImage
        # await taskMsg

if __name__ == "__main__":
    asyncio.run(main())