<h1>Software README</h1>

<h2>Arduino</h2>

<h3>Battery.cpp</h3>

<h3>EMIC2.cpp</h3>

<h3>ESP8266.cpp</h3>

<h3>I2Cdev.cpp</h3>

<h3>MPU6050.cpp</h3>

<h3>microprocessor.ino</h3>

<h2>Android App</h2>

<h2>iOS App</h2>
<p> XCode 8.3.2 <br> Swift 3 </p>
<img width="610" alt="screen shot 2017-04-29 at 7 55 16 pm" src="https://cloud.githubusercontent.com/assets/11222602/25559993/334c8344-2d16-11e7-8a0e-02df0b6613ac.png">

<h3>AppDelegate.swift	</h3>

<p> The app delegate works alongside the app object to ensure your app interacts properly with the system and with other apps. Specifically, the methods of the app delegate give you a chance to respond to important changes. For example, you use the methods of the app delegate to respond to state transitions, such as when your app moves from foreground to background execution, and to respond to incoming notifications. In many cases, the methods of the app delegate are the only way to receive these important notifications.
Xcode provides an app delegate class for every new project, so you do not need to define one yourself initially. When your app launches, UIKit automatically creates an instance of the app delegate class provided by Xcode and uses it to execute the first bits of custom code in your app. All you have to do is take the class that Xcode provides and add your custom code.
The app delegate is effectively the root object of your app. Like the UIApplication object itself, the app delegate is a singleton object and is always present at runtime. Although the UIApplication object does most of the underlying work to manage the app, you decide your app’s overall behavior by providing appropriate implementations of the app delegate’s methods. Although most methods of this protocol are optional, you should implement most or all of them.
<br>The app delegate performs several crucial roles:
<br>It contains your app’s startup code.</br>
<br>It responds to key changes in the state of your app. Specifically, it responds to both temporary interruptions and to changes in the execution state of your app, such as when your app transitions from the foreground to the background.</br>
<br>It responds to notifications originating from outside the app, such as remote notifications (also known as push notifications), low-memory warnings, download completion notifications, and more.</br>
<br>It determines whether state preservation and restoration should occur and assists in the preservation and restoration process as needed.</br>
<br>It responds to events that target the app itself and are not specific to your app’s views or view controllers.</br>
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
