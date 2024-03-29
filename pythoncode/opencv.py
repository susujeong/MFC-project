import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import tensorflow as tf
import os
import PIL
import shutil
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

from keras.preprocessing import image
from keras.src.legacy.preprocessing.image import ImageDataGenerator
from keras.callbacks import EarlyStopping,ModelCheckpoint
from keras.datasets import mnist
from tensorflow.keras.layers import Input, Dense

base_dir="C:/Users/AIOT1/Desktop/buburi/newpic/"
train_pass_dir = os.path.join(base_dir, 'pass')
train_none_dir = os.path.join(base_dir, 'none')
train_nocircle_dir = os.path.join(base_dir, 'nocircle')
train_substance_dir = os.path.join(base_dir, 'substance')

train_pass_fnames = os.listdir(train_pass_dir)
train_none_fnames = os.listdir(train_none_dir)
train_nocircle_fnames = os.listdir(train_nocircle_dir)
train_substance_fnames = os.listdir(train_substance_dir)
print(train_pass_fnames)
print(train_none_fnames)
print(train_nocircle_fnames)
print(train_substance_fnames)
print('Total training pass images :', len(os.listdir(train_pass_dir)))
print('Total training none images :', len(os.listdir(train_none_dir)))
print('Total training nocircle images :', len(os.listdir(train_nocircle_dir)))
print('Total training substance images :', len(os.listdir(train_substance_dir)))
print('Total training total images :', len(os.listdir(train_pass_dir))+len(os.listdir(train_none_dir))+len(os.listdir(train_nocircle_dir))+len(os.listdir(train_substance_dir)))

nrows, ncols = 4, 4
pic_index = 0
# fig = plt.gcf()
# fig.set_size_inches(ncols * 3, nrows * 3)
pic_index += 8
next_pass_pix = [os.path.join(train_pass_dir, fname) for fname in train_pass_fnames[pic_index - 8:pic_index]]
# next_none_pix = [os.path.join(train_none_dir, fname) for fname in train_none_fnames[pic_index - 8:pic_index]]
# # next_nocircle_pix = [os.path.join(train_nocircle_dir, fname) for fname in train_nocircle_fnames[pic_index - 8:pic_index]]+next_nocircle_pix
# # next_substance_pix = [os.path.join(train_substance_dir, fname) for fname in train_substance_fnames[pic_index - 8:pic_index]]+next_substance_pix
# for i, img_path in enumerate(next_pass_pix + next_none_pix):
#     sp = plt.subplot(nrows, ncols, i + 1)
#     sp.axis('OFF')
#     img = mpimg.imread(img_path)
#     plt.imshow(img)
#
# plt.show()

train_datagen= ImageDataGenerator(rescale=1/255,fill_mode='nearest',validation_split=0.2)
validation_datagen = ImageDataGenerator(rescale = 1/255)
# test_datagen = ImageDataGenerator(rescale = 1./255)

train_generator = train_datagen.flow_from_directory(base_dir,batch_size=512,class_mode='categorical',target_size=(150,150),subset='training',shuffle=True)
val_gen = train_datagen.flow_from_directory(base_dir,batch_size=512,class_mode='categorical',target_size=(150,150),subset='validation',shuffle=True)

print(train_generator.class_indices.keys())
# sample_img = mpimg.imread(next_pass_pix[0])
# plt.imshow(sample_img)
# plt.show()
model = tf.keras.models.Sequential([
    tf.keras.layers.Conv2D(32, (3,3), activation='relu', input_shape=(300, 300, 3)),
    tf.keras.layers.MaxPooling2D(2,2),
    # tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Conv2D(64, (3,3), activation='relu'),
    tf.keras.layers.MaxPooling2D(2,2),
    # tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Conv2D(128, (3,3), activation='relu'),
    tf.keras.layers.MaxPooling2D(2,2),
    # tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Flatten(),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(1024, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(512, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(256, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(128, activation='relu'),
    tf.keras.layers.Dropout(0.2),
    tf.keras.layers.Dense(5, activation='softmax')
])
model.summary()

model.compile(optimizer='adam', loss='categorical_crossentropy', metrics= ['accuracy'])
modelpath="C:/Users\AIOT1\Desktop/buburi/normal360/mmm.keras"
checkpoint = ModelCheckpoint(filepath=modelpath,monitor='val_loss',verbose=1,save_best_only=True,mode='min')
early_stop = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=20)
history = model.fit(train_generator,validation_data=val_gen,steps_per_epoch=100,epochs=200, verbose=1,validation_steps=50,callbacks=[checkpoint,early_stop])


model.evaluate(train_generator)
model.evaluate(val_gen)

acc = history.history['accuracy']
val_acc = history.history['val_accuracy']
loss = history.history['loss']
val_loss = history.history['val_loss']

epochs = range(len(acc))

plt.plot(epochs, acc, 'bo', label='Training accuracy')
plt.plot(epochs, val_acc, 'b', label='Validation accuracy')
plt.title('Training and validation accuracy')
plt.legend()

plt.figure()

plt.plot(epochs, loss, 'go', label='Training loss')
plt.plot(epochs, val_loss, 'g', label='Validation loss')
plt.title('Training and validation loss')
plt.legend()

# plt.show()
test_dir="C:/Users/AIOT1/Desktop/buburi/testfordeep"
# fortest_dir = os.path.join(test_dir, 'pass')
test_filenames = os.listdir(test_dir)
# print(test_filenames)
dic_test_filenames = {}
dic_test_filenames['who'] = test_filenames

for oo,filenames in dic_test_filenames.items():
    fig= plt.figure(figsize=(16,10))
    rows,cols=1,6
    print(filenames)
    for i,fn in enumerate(filenames):
        # print(oo)
        path = test_dir+"/"+fn
        # print(path)
        test_img = image.load_img(path, target_size=(300, 300,3))
        x = image.img_to_array(test_img)
        x = np.expand_dims(x, axis=0)
        images = np.vstack([x])

        classes = model.predict(images, batch_size=32)
        print(classes)
        # fig.add_subplot(rows, cols, i + 1)
        if classes[0][0] == 1:
            print("1")
            plt.title(fn + " is nocircle")
            plt.axis('off')
            plt.imshow(test_img, cmap='gray')
        elif classes[0][1] == 1:
            plt.title(fn + " is none")
            plt.axis('off')
            plt.imshow(test_img, cmap='gray')
        elif classes[0][2] == 1:
            plt.title(fn + " is normal")
            plt.axis('off')
            plt.imshow(test_img, cmap='gray')
        elif classes[0][3] == 1:
            plt.title(fn + " is substance")
            plt.axis('off')
            plt.imshow(test_img, cmap='gray')
        else:
            print("2")
            plt.title(fn + " is X")
            plt.axis('off')
            plt.imshow(test_img, cmap='gray')
        plt.show();
model.save("C:/Users\AIOT1\Desktop/buburi/normal360/newmodel.h5")