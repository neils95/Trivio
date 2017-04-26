//
//  FunFactViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/25/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class FunFactViewController: UIViewController {

 
  
    @IBOutlet weak var funFactLabel: UILabel!
    @IBOutlet weak var downVoteButton: UIButton!
    @IBOutlet weak var upVoteButton: UIButton!
    @IBOutlet weak var activityIndicator: UIActivityIndicatorView!
    
    var funFact = ""
    var funFactID = 1
    var userID: Int? = nil
    
    override func viewDidLoad() {
        super.viewDidLoad()
        funFactLabel.text = funFact
        // Do any additional setup after loading the view.
        let preferences = UserDefaults.standard
        userID = preferences.object(forKey: "userID") as? Int
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    


    @IBAction func downVotePressed(_ sender: Any) {
        
        var request = URLRequest(url: URL(string: "https://triviotoy.azurewebsites.net/Trivia/Vote/\(funFactID)")!)
        request.httpMethod = "PUT"
        
        let json: [String: Any] = ["userVote": -1,
                                   "userId": userID!,
                                   "triviaId": funFactID]
        
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
                OperationQueue.main.addOperation {
                    self.displayAlertMessage(message: "You Voted!")
                    self.activityIndicator.stopAnimating()
                }
                
                
            }
        }
        
        task.resume()
    }
    
    
    @IBAction func upVotePressed(_ sender: Any) {
        var request = URLRequest(url: URL(string: "https://triviotoy.azurewebsites.net/Trivia/Vote/\(funFactID)")!)
        request.httpMethod = "PUT"
        
        let json: [String: Any] = ["userVote": 1,
                                   "userId": userID!,
                                   "triviaId": funFactID]
        
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
                OperationQueue.main.addOperation {
                    self.displayAlertMessage(message: "You Voted!")
                    self.activityIndicator.stopAnimating()
                }
                
                
            }
        }
        
        task.resume()
    }
    
    
    func displayAlertMessage(message: String){
        let myAlert = UIAlertController(title: "Alert", message: message, preferredStyle: UIAlertControllerStyle.alert)
        
        let okAction = UIAlertAction(title: "OK", style: UIAlertActionStyle.default, handler: nil)
        
        myAlert.addAction(okAction)
        
        self.present(myAlert, animated: true, completion: nil)
        
    }


}
