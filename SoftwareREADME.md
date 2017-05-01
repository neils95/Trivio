<h1>Software README</h1>

<h2>Server</h2>
<p>The server code is written using ASP.NET framework. To replicate, create a new Web API with Read/Write controller classes.It is hosted on azure web services. </p>

<h3>Controllers</h3>
The binaries in this folder expose the API, hosted on http://triviotoy.azurewebsites.net/. Detailed API documenttaionsm including all the paths,response paramenters and return values can be found at http://triviotoy.azurewebsites.net/Help.
<ul>
<li><h4>TriviaController.cs</h4></li>
Base path of this controller is /Trivia. It contains necessary functions to get a new trivia for a user and vote on trivia.
<li><h4>UserController.cs</h4></li>
Base path of this controller is /User. It contains necessary functions to register, login and access a user's trivia history.

</ul>

<h3>Other binaries</h3>
<ul>
<li><h4>MappingProfile.cs</h4></li>
Required to configure AutoMapper. A tool which must be downloaded from npm and allows mapping fields from one class to another. Each model has a corresponding DTO, which is the class used in controllers.
<li><h4>Web.config.cs</h4></li>
Contains database connections strings.
<li><h4>Migrations.Configrations.cs</h4></li>
Since the database is set up using entity framewok's code first, it must be set up using migrations. As the first migration, any database seed values must be configured in this binary.
</ul>

<h2>Database</h2>
<h3>Models</h3>
These are Entity framework models. Each table maps to a corresponding class. Columns are represented as properties.
<ul>
<li><h4>Trivia.cs</h4></li>
Trivia table object. Columns: Trivia text, upvotes,downvotes and Id.
<li><h4>User.cs</h4></li>
User table object. Columns: Name,Email,Password, trivia count and history count.

<li><h4>UserTriviaHistory.cs</h4></li>
A join between the user and trivia tables. With additional properties: user vote.

</ul>

<h2>Arduino</h2>

<h3>Battery.cpp</h3>
<p>This is a simple Arduino library to monitor battery consumption of your battery powered projects, being LiPo, LiIon, NiCd or any other battery type, single or multiple cells: if it can power your Arduino you can monitor it! The library monitors battery capacity by measuring the voltage across the battery terminals. </p>

<h3>EMIC2.cpp</h3>
<p>This is an Arduino library for interfacing the Emic 2 Text-to-Speech module. With the library, one can change the characteristics of the speech on the module through the use of methods and operators, for a less technical and more natural way of control. It can send messages provided as an immediate argument to a method, or by reference through a file in an SD card.

<br>An instance of the EMIC2 class has to have been created before utilizing the Emic 2 module. Then the instance gets initialized by calling begin(uint8_t rx_pin, uint8_t tx_pin, uint8_t cs_pin) with arguments the RX and TX pins of the (software) serial port, and optionally the CS pin for the chip select line of the SD card.

<br>The class provides, among others, methods for setting the voice, the language, and the parser. It also provides methods for tweaking parameters that are independent of the choice of parser, such as volume and speaking rate.

<br>Parameters that are dependent to the choice of parser can be looked at the manual, and set by sending a direct command to the module, by using the sendCmd method.

<br>A message can be sent by calling the speak method on an instance of the class, with argument any type of data. (e.g. emic.speak("I'm the Emic 2 Module");)

<br>The speak method can also read files from an SD Card. By providing a filename as an argument, the method will read the file in the emic2 folder (that you will have to have created) and it will send a message to the Emic 2 module for every line in the file. (e.g. emic.speak("greeting.txt",SD_C);)

<br>Special use of operators, that they act upon an instance of the class, can further simplify the process of interfacing with the module:

<br>~emic; - pauses/unpauses playback
<br>!emic; - stops playback
<br>++emic; - raises volume level by 1dB
<br>--emic; - lowers volume level by 1dB
<br>emic += value; - raises volume level by value dB
<br>emic -= value; - lowers volume level by value dB
<br>emic >> value; - increases speaking rate by value words/minute
<br>emic << value; - decreases speaking rate by value words/minute</p>

<h3>ESP8266.cpp</h3>
<p> An ESP8266 library for Arduino providing an easy-to-use way to manipulate ESP8266. Online API documentation can be reached at http://docs.iteadstudio.com/ITEADLIB_Arduino_WeeESP8266/index.html.


bool    restart (void) : Restart ESP8266 by "AT+RST".

String  getAPList (void) : Search available AP list and return it.

bool    joinAP (String ssid, String pwd) : Join in AP. 

bool    leaveAP (void) : Leave AP joined before.  

String  getJoinedDeviceIP (void) : Get the IP list of devices connected to SoftAP. 

String  getIPStatus (void) : Get the current status of connection(UDP and TCP). 

String  getLocalIP (void) : Get the IP address of ESP8266. 

bool    enableMUX (void) : Enable IP MUX(multiple connection mode). 

bool    disableMUX (void) : Disable IP MUX(single connection mode). 

bool    createTCP (String addr, uint32_t port) : Create TCP connection in single mode. 

bool    releaseTCP (void) : Release TCP connection in single mode. 

bool    registerUDP (String addr, uint32_t port) : Register UDP port number in single mode. 

bool    unregisterUDP (void) : Unregister UDP port number in single mode. 

bool    startTCPServer (uint32_t port=333) : Start TCP Server(Only in multiple mode). 

bool    stopTCPServer (void) : Stop TCP Server(Only in multiple mode). 

bool    send (const uint8_t *buffer, uint32_t len) : Send data based on TCP or UDP builded already in single mode. 

uint32_t    recv (uint8_t *buffer, uint32_t buffer_size, uint32_t timeout=1000) : Receive data from TCP or UDP builded already in single mode.   </p>

<h3>I2Cdev.cpp</h3>

The I2C Device Library (i2cdevlib) is a collection of uniform and well-documented classes to provide simple and intuitive interfaces to I2C devices. Each device is built to make use of the generic "I2Cdev" class, which abstracts the I2C bit- and byte-level communication away from each specific device class, making it easy to keep the device class clean while providing a simple way to modify just one class to port the I2C communication code onto different platforms (Arduino, PIC, MSP430, Jennic, simple bit-banging, etc.). Device classes are designed to provide complete coverage of all functionality described by each device's documentation, plus any generic convenience functions that are helpful.

<h3>microprocessor.ino</h3>
The microprocessor.ino file is the file that incorporates all of the above libraries and runs the code that the Trivia Throw Toy runs on. The file runs a setup() and loop() function for initializing the code and constantly checking for acceleration. In the setup(), all modules are initialized and the wifi is setup to connect to the internet. In the loop function, the toy is constantly checking for acceleration above a certain threshold; when the acceleration is detected, a fact is pulled from the SD card, translated to speech via the EMIC2 and spoken aloud through the speaker. The toy also makes call to the cloud server when the wifi is connected and the cache on the sd has space to bring facts on as well as update the server about what facts have been heard. The state of the toy is displayed via LEDs and a FSM that lets the user know if the toy is connected to wifi, pulling a fact in or has low battery.

<h2>iOS App</h2>
<p> XCode 8.3.2 <br> Swift 3 </p>
<img width="610" alt="screen shot 2017-04-29 at 7 55 16 pm" src="https://cloud.githubusercontent.com/assets/11222602/25559993/334c8344-2d16-11e7-8a0e-02df0b6613ac.png">

<h3>AppDelegate.swift	</h3>

<p> The app delegate works alongside the app object to ensure your app interacts properly with the system and with other apps. Specifically, the methods of the app delegate give you a chance to respond to important changes. For example, you use the methods of the app delegate to respond to state transitions, such as when your app moves from foreground to background execution, and to respond to incoming notifications. In many cases, the methods of the app delegate are the only way to receive these important notifications.
Xcode provides an app delegate class for every new project, so you do not need to define one yourself initially. When your app launches, UIKit automatically creates an instance of the app delegate class provided by Xcode and uses it to execute the first bits of custom code in your app. All you have to do is take the class that Xcode provides and add your custom code.
The app delegate is effectively the root object of your app. Like the UIApplication object itself, the app delegate is a singleton object and is always present at runtime. Although the UIApplication object does most of the underlying work to manage the app, you decide your app’s overall behavior by providing appropriate implementations of the app delegate’s methods. Although most methods of this protocol are optional, you should implement most or all of them.
<br>The app delegate performs several crucial roles:
<br>It contains your app’s startup code.
<br>It responds to key changes in the state of your app. Specifically, it responds to both temporary interruptions and to changes in the execution state of your app, such as when your app transitions from the foreground to the background.
<br>It responds to notifications originating from outside the app, such as remote notifications (also known as push notifications), low-memory warnings, download completion notifications, and more.
<br>It determines whether state preservation and restoration should occur and assists in the preservation and restoration process as needed.
<br>It responds to events that target the app itself and are not specific to your app’s views or view controllers.
You can use it to store your app’s central data objects or any content that does not have an owning view controller. </p>


<h3>FunFactTableViewController.swift</h3>

<p>iOS comes with a built-in class, UITableView, designed specifically to display a scrolling list of items.
A table view displays a list of items in a single column. UITableView is a subclass of UIScrollView, which allows 
users to scroll through the table, although UITableView allows vertical scrolling only. The cells comprising the 
individual items of the table are UITableViewCell objects; UITableView uses these objects to draw the visible rows of the table. 
Cells have content—titles and images—and can have, near the right edge, accessory views.<br> A table view is managed by a table view controller 
(UITableViewController). UITableViewController is a subclass of UIViewController, which is designed to handle table view-related logic. </br>
<br>The FunFactTableViewController is the controller for the FunFactTableView. The view is used to display the history of heard fun facts
facts in a table view. When the view controller is activated, a GET request is made to the server with the url: https://triviotoy.azurewebsites.net/User/History/\(userID).
Where userID is the current ID of the user that is stored on the phone to indicate the current session.The request is then parsed into the 
different UITableViewCells within the table view. When a table view cell is selected, a segue occurs that brings up the FunFactViewController.
Before seguing to the new view, the data of which fact was selected must be sent along to the controller. This occurs in the function prepare(for segue: UIStoryboardSegue, sender: Any?)</p>

<h3>FunFactViewController.swift</h3>

<p>A view controller manages a set of views that make up a portion of your app’s user interface. It is responsible 
for loading and disposing of those views, for managing interactions with those views, and for coordinating responses with any 
appropriate data objects. View controllers also coordinate their efforts with other controller objects—including other view controllers—and 
help manage your app’s overall interface.
<br>You rarely create instances of the UIViewController class directly. Instead, you create instances of UIViewController subclasses and use 
those objects to provide the specific behaviors and visual appearances that you need.The FunFactViewController is a custom subclass of UIViewController
that controls the FunFactView. The FunFactView displays the fun fact selected from the FunFactTableViewController as well as a up vote and down vote button
for voting on the fact. The fun fact is brought in from the FunFactTablerViewController and is stored in the fun fact string variable. When the up or down vote button
is pressed, a POST request is sent to "https://triviotoy.azurewebsites.net/Trivia/Vote/\(funFactID)", where the funfactID is the ID of the fact chosen
that is also brought in from the FunFactTablerViewController. The body of the POST request includes the vote, which is 1 for up vote and -1 for down vote, as well
as the userID and funFactID.</br>

<h3>LoginViewController.swift</h3>

<p>The LoginViewController is a custom subclass of UIViewController that controls the LoginView. The LoginView has UITextFields for email and password and a UIButton for logging in.
When a user inputs their email and password, a POST request is sent to https://triviotoy.azurewebsites.net/User/Register with the body of request
including the username, email and password. If there is an error in the request, a UIAlertController alerts the user of the error that is sent back from the server.
On a successful signup, a responseJSON is sent back that includes the user's ID number. The ID is stored in the preferences of the device which acts as a marker
for the current session, so that the next time the user opens the app they are automatically logged in. After the user ID is stored, a segue prompts the main view of the application,
the ViewController.</p> 

<h3>SignupViewController.swift</h3>

<p>The SignUpViewController is a custom subclass of UIViewController that controls the SignUpView. The SignUpView has UITextFields for username, email,password and retyping the password, and a UIButton for signing up.
When a user inputs all of this information, a POST request is sent to https://triviotoy.azurewebsites.net/User/Login with the body of request
including the username and email. If there is an error in the request, a UIAlertController alerts the user of the error that is sent back from the server.
On a successful login, a responseJSON is sent back that includes the user's ID number. The ID is stored in the preferences of the device which acts as a marker
for the current session, so that the next time the user opens the app they are automatically logged in. After the user ID is stored, a segue prompts the main view of the application,
the ViewController. </p>

<h3>ViewController.swift</h3>

<p>The ViewController is a custom subclass of UIViewController that controls the main page of the application, View. On the main View, there are UIButtons for seguing
to the WiFiView and the FunFactTableView. There is also a button to take the user to the CategoriesView, but that is not currently implemented because the toy does not support
categories at the moment. Other than the segues, the ViewController has a log out button which removes the user ID from preferences and takes the user back to the LoginView.</p>

<h3>WiFiViewController.swift</h3>
<p> The WiFiViewController is a custom subclass of UIViewController that controls the WiFiView. The WifiView has UITextFields for entering the ssid and password of the wifi the user
wants the trivia toy to connect to. There are also UIlabels that instruct the user to first connect to the toy's wifi in order to send the data. When the WiFiViewController is loaded,
a socket is opened to interact with the trivia toy via a TCP connection. The TCP connection is a three-way handshake between the toy and the application to ensure
security when the message is being sent. Once the connection is made, the application sends along the ssid, password and userID to they toy so that the toy can connect to wifi and keep
track of the right user's activity. </p>
