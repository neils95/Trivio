//
//  FactHistoryViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/25/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class FactHistoryViewController: UIViewController, UITableViewDelegate, UITableViewDataSource {

    var funFacts = ["Putting candles on birthday cakes is a tradition that has been around for a long time. It can be traced back to the Ancient Greeks, who often burned candles as offerings to their many gods and goddesses.", "It has become a modern ritual tradition for Japanese students to eat katsudon the night before taking a major test or school entrance exam. ", "Fireflies light up to attract a mate. To do this, the fireflies contain specialized cells in their abdomen that make light.", "Your funny bone isn’t actually a bone at all. Running down the inside part of your elbow is a nerve called the ulnar nerve. The ulnar nerve lets your brain know about feelings in your fourth and fifth fingers.", "In early 1995, Mars ran a promotion in which consumers were to vote on which of blue, pink, or purple would replace the tan M&M’s. Blue was the winner, replacing tan in the fall of 1995."]
    
    @IBOutlet weak var factTableView: UITableView!
   
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return funFacts.count
    }
    
    func numberOfSections(in tableView: UITableView) -> Int {
        return 1
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "factCell", for: indexPath)
        cell.textLabel?.text = funFacts[indexPath.row]
        return cell
    }

}
