//
//  ViewController.swift
//  Trivia Throw Toy
//
//  Created by Jacob Dansey on 1/25/17.
//  Copyright © 2017 Jacob Dansey. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    @IBOutlet weak var manageWifiButton: UIButton!
    @IBOutlet weak var pickACategoryButton: UIButton!
    @IBOutlet weak var viewFunFactsButton: UIButton!
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        self.navigationItem.hidesBackButton = true
        let preferences = UserDefaults.standard
        if (preferences.object(forKey: "userID") == nil){
            self.navigationController?.popViewController(animated: true)
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func logOutPressed(_ sender: Any) {
        let preferences = UserDefaults.standard
        preferences.removeObject(forKey: "userID")
        self.navigationController?.popViewController(animated: true)
    }

}

