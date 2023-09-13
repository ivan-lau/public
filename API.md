Genernal
---

- Are you still currently working in Precision Robotics?

- As I see in your CV, You mentioned that you did very well and you got promoted at ARUP (Err rub). In  this case, it seems to be the firm really valued your effort, but why you moved to another firm after 1 year and 3 months? which is i guess just promoted?

- Why there is a gap between ARUP and Precision Robotics?

- Are you still currently working in Precision Robotics? 

- Why did you join Precision Robotics?

- How many people is in the firm?

- Are you working as a perm or a contractor?

- After 2 years, why do you consider a totally different industry and a contractor role?

- which part of your experience is using C++?


C
---

- malloc vs calloc

    - malloc() takes a single argument(memory required in bytes), while calloc() needs 2 arguments(block of allocated memory size in bytes of a single variable).
    - malloc() does not initialize the memory allocated, while calloc() initializes the allocated memory to ZERO.

- free vs realloc
    - free() deallocates the space previously allocated by malloc(), calloc() or realloc(). If ptr is null-pointer, the function does nothing. 
    - calling free twice ("double free"), etc., usually leads to a crash of the program.
    - realloc() If realloc cannot find enough space at all, it returns a null pointer, and leaves the previous region allocated.
      - void* new_alloc_ptr = realloc(prev_alloc_ptr, size);

C++
---

- What is the different between struct vs class?

- What is the different between std::array vs std::vector?

- what would happen if you keep push back a lot of items into a vector?

- explain what is polymorphism in C++?

- What is a virtual destructor in C++? Explain when it is necessary to declare a destructor as virtual.

- In which class I should mark the destructor as virtual?

Qt
---

- How can you handle user input, such as mouse events or keyboard events, in a Qt application?

In a Qt application, you can handle user input, including mouse events and keyboard events, by implementing event handlers or connecting signals to slots. Here's how you can handle these types of events:

Mouse Events:

Reimplement event handlers: Override the mousePressEvent(), mouseReleaseEvent(), mouseMoveEvent(), or other relevant event handler functions in your widget or QObject-derived class. These functions will be called when the corresponding mouse events occur.
Connect signals to slots: Connect the signals provided by the mouse-related widgets, such as clicked(), pressed(), released(), or moved(), to appropriate slots in your code. This allows you to respond to specific mouse events.

```
class MyWidget : public QWidget {
protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            qDebug() << "Left mouse button pressed at" << event->pos();
        }
    }

    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Escape) {
            qDebug() << "Escape key pressed";
        }
    }
};
```

- Explain the concept of signals and slots in Qt. How are they used for inter-object communication?

In Qt, signals and slots are a powerful mechanism for inter-object communication. They allow objects to communicate with each other in a loosely coupled manner, enabling flexible and modular design. Signals and slots work together to establish a connection between objects, enabling the emission and handling of events or data.

Here's how the concept of signals and slots works in Qt:

1. Signals:

    - A signal is a member function declared in a QObject-derived class that can emit a notification to other objects.
    - Signals are typically declared in the signals section of a class definition and have no implementation.
    - Signals can have parameters, allowing them to transmit data to connected slots.
    - Qt provides a mechanism for automatically connecting signals to slots based on the matching function signatures.

2. Slots:

    - A slot is a member function declared in a QObject-derived class that can be connected to a signal to handle the emitted notification.
    - Slots are typically declared in the slots section of a class definition and have an implementation.
    - Slots can have parameters matching the signal's parameters, allowing them to receive data from the signal.
    - Slots can perform any desired actions or computations in response to the emitted signals.

3. Connecting Signals and Slots:

    - To establish a connection between a signal and a slot, you use the connect() function or connect signals and slots using the signal-slot syntax.
    - The connect() function takes the sender object, the signal to connect, the receiver object, and the slot to connect as parameters.
    - With the signal-slot syntax, you use the SIGNAL() and SLOT() macros to specify the signal and slot, respectively, as strings.
    - Connections can be made between objects within the same thread or across different threads, enabling thread-safe communication.

```
class MyButton : public QPushButton {
    Q_OBJECT
signals:
    void customClicked(int buttonId);

public slots:
    void handleClick() {
        emit customClicked(42);
    }
};

class MyWidget : public QWidget {
public slots:
    void handleCustomClick(int buttonId) {
        qDebug() << "Custom click received with button ID:" << buttonId;
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MyButton *button = new MyButton("Click Me");
    MyWidget *widget = new MyWidget;

    QObject::connect(button, SIGNAL(customClicked(int)), widget, SLOT(handleCustomClick(int)));

    QObject::connect(button, &QPushButton::clicked, button, &MyButton::handleClick);

    button->show();
    return app.exec();
}
```

CMake
----

- In CMake, what approach would you take if you need to share a variable across multiple CMakeLists.txt files in different directories?

- In CMake, what is the difference between a macro and a function? How would you decide which one to use in a given scenario?

    - The only difference between a function and a macro is scope; macros don't have one. So, if you set a variable in a function and want it to be visible outside, you'll need PARENT_SCOPE. Nesting functions therefore is a bit tricky, since you'll have to explicitly set the variables you want visible to the outside world to PARENT_SCOPE in each function. But, functions don't "leak" all their variables like macros do.

OpenCV
----

- How can you detect and recognize faces in an image or video stream using OpenCV?
  To detect and recognize faces in an image or video stream using OpenCV, you can follow these steps:
  
  1. Import the necessary libraries: Begin by importing the required libraries, including OpenCV and any additional modules needed for face detection and recognition.
  
  2. Load the face detection classifier: OpenCV provides pre-trained cascades that can be used for face detection. Load the appropriate cascade file (e.g., Haar cascade) using the cv2.CascadeClassifier class.
  
  3. Load the image or video stream: Read the input image or set up a video capture stream using the cv2.imread() or cv2.VideoCapture() functions, respectively.
  
  4. Preprocess the image or video frame: If needed, perform any preprocessing steps on the image or video frame, such as resizing, converting to grayscale, or normalizing the pixel values.
  
  5. Detect faces: Apply the face detection cascade to locate faces in the image or video frame. Use the detectMultiScale() method of the CascadeClassifier object to detect faces. This method returns a list of rectangles (bounding boxes) that represent the detected faces.
  
  6. Draw bounding boxes: Iterate through the detected faces and draw bounding boxes around them using the cv2.rectangle() function.
  
  7. Extract face regions: For each detected face, you can extract the region of interest (ROI) using the coordinates from the bounding box. This ROI can be used for further processing or recognition.
  
  8. Recognize faces (optional): If you want to perform face recognition, you'll need a trained model for face recognition. This typically involves training a deep learning model on a labeled dataset of faces. Once you have a trained model, you can use it to recognize the extracted face regions.
  
  9. Display the results: Show the image or video frame with the detected faces and any additional information, such as recognized names or labels.
  
  10. Repeat for subsequent frames: If working with a video stream, repeat the above steps for each frame in the stream until the video ends or the program is terminated.
  
  Remember that face recognition typically involves additional steps such as feature extraction, embedding comparison, and classification. These steps are beyond the scope of basic face detection using OpenCV and require additional machine learning or deep learning techniques.




