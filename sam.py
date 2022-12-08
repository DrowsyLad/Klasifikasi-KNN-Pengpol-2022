import json
import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
# import tensorflow.keras as keras
import random

DATA_PATH = "Hasil_Ekstraksi_Fitur.json"
SAVED_MODEL_PATH = "Model_ANN.h5"
EPOCHS = 50
BATCH_SIZE = 32
PATIENCE = 10
LEARNING_RATE = 0.0001

def load_data(data_path):
    """Loads training dataset from json file."""

    with open(data_path, "r") as fp:
        data = json.load(fp)

    # convert lists to numpy arrays
    X = np.array(data["mfcc"])
    y = np.array(data["labels"])

    print("Data succesfully loaded!")

    return  X, y

def prepare_dataset(data_path, test_size=0.3, validation_size=0.3):
    """Creates train, validation and test sets."""

    # load dataset
    X, y = load_data(data_path)

    # create train, validation, test split
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_size)
    X_train, X_validation, y_train, y_validation = train_test_split(X_train, y_train, test_size=validation_size)

    # add an axis to nd array
    X_train = X_train[..., np.newaxis]
    X_test = X_test[..., np.newaxis]
    X_validation = X_validation[..., np.newaxis]

    return X_train, y_train, X_validation, y_validation, X_test, y_test

def build_model(input_shape, loss="sparse_categorical_crossentropy", learning_rate=0.0001):
    """Build neural network using keras."""
    # build network architecture using convolutional layers
    model = tf.keras.Sequential([
        # input layer
        tf.keras.layers.Flatten(input_shape=input_shape),
        # 1st dense layer
        tf.keras.layers.Dense(512, activation='relu'),
        # 2nd dense layer
        tf.keras.layers.Dense(256, activation='relu'),
        # 3rd dense layer
        tf.keras.layers.Dense(128, activation='relu'),
        # 4rd dense layer
        tf.keras.layers.Dense(64, activation='relu'),
        # 5rd dense layer
        tf.keras.layers.Dense(32, activation='relu'),
        tf.keras.layers.Dropout(0.2),
        # output layer
        tf.keras.layers.Dense(256, activation='softmax')
    ])
    # compile model
    optimiser = tf.keras.optimizers.Adam(learning_rate=0.0001)
    model.compile(optimizer=optimiser,
                  loss='sparse_categorical_crossentropy',
                  metrics=['accuracy'])

    model.summary()
    return model

def train(model, epochs, batch_size, patience, X_train, y_train, X_validation, y_validation):
    """Trains model"""
    earlystop_callback = tf.keras.callbacks.EarlyStopping(monitor="accuracy", min_delta=0.001, patience=patience)

    # train model
    history = model.fit(X_train,
                        y_train,
                        epochs=epochs,
                        batch_size=batch_size,
                        validation_data=(X_validation, y_validation),
                        callbacks=[earlystop_callback])
    return history

def plot_history(history):
    """Plots accuracy/loss for training/validation set as a function of the epochs"""

    fig, axs = plt.subplots(2)

    # create accuracy subplot
    axs[0].plot(history.history["accuracy"], label="accuracy")
    axs[0].plot(history.history['val_accuracy'], label="val_accuracy")
    axs[0].set_ylabel("Accuracy")
    axs[0].legend(loc="lower right")
    axs[0].set_title("Accuracy evaluation")

    # create loss subplot
    axs[1].plot(history.history["loss"], label="loss")
    axs[1].plot(history.history['val_loss'], label="val_loss")
    axs[1].set_xlabel("Epoch")
    axs[1].set_ylabel("Loss")
    axs[1].legend(loc="upper right")
    axs[1].set_title("Loss evaluation")

    plt.show()

def predict(model, X, y):
    X = X[np.newaxis, ...] 
    prediction = model.predict(X)
    predicted_index = np.argmax(prediction, axis=1)

    print("Data Asli: {}, Hasil Prediksi: {}".format(y, predicted_index))

def main():
    # generate train, validation and test sets
    X_train, y_train, X_validation, y_validation, X_test, y_test = prepare_dataset(DATA_PATH)

# create network
    input_shape = (X_train.shape[1], X_train.shape[2], 1)
    model = build_model(input_shape, learning_rate=LEARNING_RATE)
    # train network
    history = train(model, EPOCHS, BATCH_SIZE, PATIENCE, X_train, y_train, X_validation, y_validation)

    # plot accuracy/loss for training/validation set as a function of the epochs
    plot_history(history)

    # evaluate network on test set
    test_loss, test_acc = model.evaluate(X_test, y_test)
    print("\nTest loss: {}, \nTest accuracy: {}".format(test_loss, 100*test_acc))

    # save model
    model.save(SAVED_MODEL_PATH)

    # pick a sample to predict from the test set then predict
    for i in range(50):
        r = random.randint(0,150)
        X_to_predict = X_test[r]
        y_to_predict = y_test[r]

        predict(model, X_to_predict, y_to_predict)

if __name__ == "__main__":
    # main()
    # load_data()
    print("hello world")