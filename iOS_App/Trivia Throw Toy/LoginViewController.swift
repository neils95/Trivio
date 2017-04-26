//
//  LoginViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/31/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class LoginViewController: UIViewController {

    @IBOutlet weak var emailTextField: UITextField!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var loginButton: UIButton!
    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!
    var keyboardShowing = false

    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        NotificationCenter.default.addObserver(self, selector: #selector(LoginViewController.keyboardWillShow), name: NSNotification.Name.UIKeyboardWillShow, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(LoginViewController.keyboardWillHide), name: NSNotification.Name.UIKeyboardWillHide, object: nil)
        
        let tap: UITapGestureRecognizer = UITapGestureRecognizer(target: self, action: "dismissKeyboard")
        view.addGestureRecognizer(tap)
        
    }
    
    override func viewDidAppear(_ animated: Bool) {
        //login info
        let preferences = UserDefaults.standard
        self.activityIndicator.isHidden = true

        if preferences.object(forKey: "userID") != nil{
            
            performSegue(withIdentifier: "loginSegue", sender: self)
            
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    @IBAction func loginPress(_ sender: Any) {
        //get data from text fields
        self.activityIndicator.startAnimating()
        let email = emailTextField.text
        let password = passwordTextField.text
        
        //error check data and attempt to log in
        if (email == "") || (password == "") {
            displayAlertMessage(message: "All text fields must be entered")
            self.activityIndicator.stopAnimating()
        }else {
        //check if user exists in db then segue
            login(email: emailTextField.text!, password: passwordTextField.text!)
            
            
        }
    }

    func login(email: String, password: String){
        var request = URLRequest(url: URL(string: "https://triviotoy.azurewebsites.net/User/Login")!)
        request.httpMethod = "POST"
        
        let json: [String: Any] = [
                                   "email": email,
                                   "password": password]
        
        let jsonData = try? JSONSerialization.data(withJSONObject: json)
        
        
        
        request.httpBody = jsonData
        request.setValue("application/json; charset=utf-8", forHTTPHeaderField: "Content-Type")
        
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            guard let data = data, error == nil else {
                print(error?.localizedDescription ?? "No data")
                self.activityIndicator.stopAnimating()
                return
            }
            let responseJSON = try? JSONSerialization.jsonObject(with: data, options: [])
            if let responseJSON = responseJSON as? [String: AnyObject] {
                print(responseJSON) //no error message sent for wrong password
                let preferences = UserDefaults.standard
                if let userID = responseJSON["id"] as? Int{
                    OperationQueue.main.addOperation {
                        preferences.set(userID, forKey: "userID")
                        self.activityIndicator.stopAnimating()
                        self.performSegue(withIdentifier: "loginSegue", sender: self)
                    }
                }else{
                    OperationQueue.main.addOperation {
                        let message = responseJSON["message"] as! String
                        self.displayAlertMessage(message: message)
                        self.activityIndicator.stopAnimating()
                    }
                }
                
            }
        }
        
        task.resume()
    }
    
    func keyboardWillShow(notification: NSNotification) {
        
        if let keyboardSize = (notification.userInfo?[UIKeyboardFrameBeginUserInfoKey] as? NSValue)?.cgRectValue {
            if self.view.frame.origin.y == 0{
                self.view.frame.origin.y -= keyboardSize.height
            }
        }
        
    }
    
    func keyboardWillHide(notification: NSNotification) {
        if let keyboardSize = (notification.userInfo?[UIKeyboardFrameBeginUserInfoKey] as? NSValue)?.cgRectValue {
            if self.view.frame.origin.y != 0{
                self.view.frame.origin.y += keyboardSize.height
            }
        }
    }
    
    func dismissKeyboard() {
        //Causes the view (or one of its embedded text fields) to resign the first responder status.
        view.endEditing(true)
    }
        
    func displayAlertMessage(message: String){
        let myAlert = UIAlertController(title: "Alert", message: message, preferredStyle: UIAlertControllerStyle.alert)
        
        let okAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.default, handler: nil)
        
        myAlert.addAction(okAction)
        
        self.present(myAlert, animated: true, completion: nil)
        
    }
    
}
