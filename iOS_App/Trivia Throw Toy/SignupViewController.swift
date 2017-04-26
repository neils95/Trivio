//
//  SignupViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/31/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class SignupViewController: UIViewController {

   
    @IBOutlet weak var firstNameTextField: UITextField!
    @IBOutlet weak var lastNameTextField: UITextField!
    @IBOutlet weak var emailTextField: UITextField!
    @IBOutlet weak var passwordTextField: UITextField!
    @IBOutlet weak var retypePasswordTextField: UITextField!
    @IBOutlet weak var registerButton: UIButton!
    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        NotificationCenter.default.addObserver(self, selector: #selector(SignupViewController.keyboardWillShow), name: NSNotification.Name.UIKeyboardWillShow, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(SignupViewController.keyboardWillHide), name: NSNotification.Name.UIKeyboardWillHide, object: nil)
        
        let tap: UITapGestureRecognizer = UITapGestureRecognizer(target: self, action: "dismissKeyboard")
        view.addGestureRecognizer(tap)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func registerPress(_ sender: Any) {
        activityIndicator.startAnimating()
        activityIndicator.isHidden = false
        let name = firstNameTextField.text
        let email = emailTextField.text
        let password = passwordTextField.text
        let rePassword = retypePasswordTextField.text
        checkInputs(name: name!, email: email!, password: password!, rePassword: rePassword!)
    }

    func checkInputs(name: String, email: String, password: String, rePassword: String){
        if (name == "" || email == "" || password == "" || rePassword == ""){
            displayAlertMessage(message: "please enter all text fields")
            activityIndicator.stopAnimating()
            
        }else if (password != rePassword){
            displayAlertMessage(message: "passwords do not match")
            activityIndicator.stopAnimating()
        }else{
            registerUser(name: name, email: email, password: password)
        }
    }
    
    func registerUser(name: String, email: String, password: String){
        
        var request = URLRequest(url: URL(string: "https://triviotoy.azurewebsites.net/User/Register")!)
        request.httpMethod = "POST"
        
        let json: [String: Any] = ["name": name,
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
                print(responseJSON)
                let preferences = UserDefaults.standard
                if let userID = responseJSON["id"] as? Int{
                    OperationQueue.main.addOperation {
                        preferences.set(userID, forKey: "userID")
                        self.activityIndicator.stopAnimating()
                        self.displayAlertMessage(message: "Registered")
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
