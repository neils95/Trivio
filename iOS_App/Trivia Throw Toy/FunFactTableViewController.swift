//
//  FunFactTableViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/25/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class FunFactTableViewController: UITableViewController {

    var listData = [String: AnyObject]()
    
    
    
    @IBOutlet var funFactTableView: UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Do any additional setup after loading the view.
        let preferences = UserDefaults.standard
        let userID = preferences.object(forKey: "userID") as! Int
        let url = "https://triviotoy.azurewebsites.net/User/History/\(userID)"
        
        let urlRequest = URL(string: url)
        
        URLSession.shared.dataTask(with: urlRequest!) { (data, response, error) in
            if(error != nil){
                print(error.debugDescription)
            }else{
                do {
                    self.listData = try JSONSerialization.jsonObject(with: data!, options: .allowFragments) as! [String: AnyObject]
                    
                    OperationQueue.main.addOperation { //possible error line here
                        self.tableView.reloadData()
                    }
                    
                }catch let error as NSError{
                    print(error.debugDescription)
                }
            }
        }.resume()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        if let count = listData["triviaHistory"]?.count{
            return count
        }else{
            return 0
        }
    }
    
    override func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "factCell", for: indexPath)
        if let history = listData["triviaHistory"] as? [[String: AnyObject]] {
            let trivia = history[indexPath.row]["trivia"]
            cell.textLabel?.text = trivia?["text"] as? String
            return cell
        }else{
            cell.textLabel?.text = "error"
            return cell
        }
    }
    
    override func tableView(_ tableView: UITableView, estimatedHeightForRowAt indexPath: IndexPath) -> CGFloat {
        return UITableViewAutomaticDimension
    }
    
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        let controller = segue.destination as! FunFactViewController
        let indexPath = self.funFactTableView.indexPathForSelectedRow
        if let history = listData["triviaHistory"] as? [[String: AnyObject]] {
            controller.funFact = history[indexPath!.row]["trivia"]?["text"] as! String
            controller.funFactID = history[indexPath!.row]["trivia"]?["id"] as! Int
        
        }
    }

}
